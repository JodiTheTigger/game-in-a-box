/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>

    This file is part of Game-in-a-box

    Game-in-a-box is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef USING_PRECOMPILED_HEADERS
#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <array>
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "Common/Logging/Logging.hpp"
#include "Common/IStateManager.hpp"

#include "INetworkProvider.hpp"
#include "NetworkPacket.hpp"
#include "PacketDelta.hpp"
#include "XorCode.hpp"
#include "BufferSerialisation.hpp"
#include "Connection.hpp"

#include "NetworkManagerClient.hpp"

using std::string;
using namespace std::chrono;
using namespace GameInABox::Common::Network;
using namespace GameInABox::Common::Logging;


NetworkManagerClient::NetworkManagerClient(
        INetworkProvider &network,
        IStateManager& stateManager)
    : INetworkManager()
    , myNetwork(network)
    , myConnection(stateManager)
    , myStateManager(stateManager)
    , myServerAddress()
    , myClientId(0)
    , myCompressor(stateManager.GetHuffmanFrequencies())
    , myLastSequenceProcessed(0)
    , myPacketSentCount(0)
{
}

NetworkManagerClient::~NetworkManagerClient()
{
    Fail("Client shutdown.");
}

void NetworkManagerClient::Connect(boost::asio::ip::udp::endpoint serverAddress)
{
    // reset state please.
    myNetwork.Reset();
    myConnection.Start(Connection::Mode::Client);

    myServerAddress = serverAddress;

    myLastSequenceProcessed = Sequence(0);

    myPacketSentCount = 0;

    // Kick off an OOB send.
    PrivateSendState();
}

void NetworkManagerClient::Disconnect()
{
    Fail("Client disconnected.");
}

void NetworkManagerClient::PrivateProcessIncomming()
{
    // Don't test to see if the network is enabled.
    // As that is a corner case and isn't workth the check.
    auto packets = myNetwork.Receive();

    for (auto& packet : packets)
    {
        if (packet.address == myServerAddress)
        {
            auto id = myConnection.IdClient();

            if (myStateManager.CanReceive(id, packet.data.size()))
            {
                auto response = myConnection.Process(packet.data);

                if (!response.empty())
                {
                    if (myStateManager.CanSend(id, response.size()))
                    {
                        myNetwork.Send({{response, myServerAddress}});
                    }
                }
            }
        }
    }

    // Do some work :-)
    if (myConnection.IsConnected())
    {
        DeltaReceive();
    }
    else
    {
        if (myConnection.HasFailed())
        {
            Fail(myConnection.FailReason());
        }
    }
}

void NetworkManagerClient::PrivateSendState()
{
    if (myConnection.IsConnected())
    {        
        if ((myConnection.IdClient()) && (myStateManager.IsConnected(myConnection.IdClient().get())))
        {
            DeltaSend();
        }
        else
        {
            Fail("IStateManager: Not Connected.");
        }
    }
    else
    {
        if (!myNetwork.IsDisabled())
        {
            auto response = myConnection.Process({});

            if (!response.empty())
            {
                if (myStateManager.CanSend(myConnection.IdClient(), response.size()))
                {
                    myNetwork.Send({{response, myServerAddress}});
                }
            }
        }
    }
}

void NetworkManagerClient::Fail(std::string failReason)
{
    // only send the packet if we're connected.
    if (myConnection.IsConnected())
    {
        myConnection.Disconnect(failReason);

        auto lastPacket = myConnection.Process({});

        if (!lastPacket.empty())
        {
            if (myStateManager.CanSend(myConnection.IdClient(), lastPacket.size()))
            {
                myNetwork.Send({{lastPacket, myServerAddress}});
            }
        }

        myNetwork.Flush();
        myNetwork.Disable();
    }
    else
    {
        myConnection.Disconnect(failReason);
    }

    Logging::Log(
        Logging::LogLevel::Notice,
        myServerAddress.address().to_string().c_str(),
        ": ",
        myServerAddress.port(),
        " failed due to: ",
        myConnection.FailReason().c_str());
}

void NetworkManagerClient::DeltaReceive()
{
    auto delta = myConnection.GetDefragmentedPacket();

    if (delta.IsValid())
    {
        if (delta.GetSequence() > myLastSequenceProcessed)
        {
            // First copy
            std::vector<uint8_t> payload(GetPayloadBuffer(delta));

            // Decrypt (XOR based).
            // NOTE: nothing too complex for encryption, even more simple than q3.
            // As someone wanting to hack can. If we want security, use public key
            // private key to pass a super long seed to a pseudo random generator
            // that's used to decrypt. Otherwise it's just easily hackable.
            // Reason for excryption in the fist place is to prevent easy man-in-the-middle
            // attacks to control someone else's connection.
            std::array<uint8_t, 4> code;
            auto ack = delta.GetSequenceAck();
            uint16_t rawAck = ack ? ack->Value() : 0;
            Push(begin(code), delta.GetSequence().Value());
            Push(begin(code) + 2, rawAck);
            XorCode(begin(code), end(code), myConnection.Key().data);
            XorCode(begin(payload), end(payload), code);

            // Bah, I wrote Huffman and Bitstream before I knew about iterators
            // or streams. This results in lots of copies that arn't really needed.
            // Need to benchmark to see if the copies matter, and if so, rewrite
            // to use iterators or streams.

            // Decompress (2nd Copy)
            auto decompressed = move(myCompressor.Decode(payload));

            // Pass to gamestate (which will decompress the delta itself).
            auto deltaData = Delta{
                    delta.GetSequenceBase(),
                    delta.GetSequence(),
                    move(decompressed)};

           myLastSequenceProcessed = myStateManager.DeltaParse(
                myConnection.IdClient().get(),
                deltaData);
        }
    }
}

void NetworkManagerClient::DeltaSend()
{
    // Delta compression aggression should be inferred by the packet distance,
    // thus how well the packet compresses, therefore how big the packet should get.
    // A large packet delta should require more aggressive delta creation to get
    // smaller packet sizes.
    auto id = myConnection.IdClient();
    auto deltaData = myStateManager.DeltaCreate(
                *id,
                myConnection.LastSequenceAck());

    if (deltaData.deltaPayload.size() <= MaxPacketSizeInBytes)
    {
        // ignore if the delta distance is greater than 255, as we
        // store the distance as a byte.
        auto distance = deltaData.to - deltaData.base;

        if (distance <= PacketDelta::MaximumDeltaDistance())
        {
            // Compress, encrypt, send
            auto compressed = move(myCompressor.Encode(deltaData.deltaPayload));

            std::array<uint8_t, 4> code;
            Push(begin(code), deltaData.to.Value());
            Push(begin(code) + 2, myLastSequenceProcessed.Value());
            XorCode(begin(code), end(code), myConnection.Key().data);
            XorCode(begin(compressed), end(compressed), code);

            // Add the client id and payload.
            PacketDelta delta(
                    deltaData.to,
                    myLastSequenceProcessed,
                    static_cast<uint8_t>(distance),
                    myClientId,
                    move(compressed));

            // client packets are not fragmented.
            if (!delta.data.empty())
            {
                if (myStateManager.CanSend(*id, delta.data.size()))
                {                    
                    myNetwork.Send({{std::move(delta.data), myServerAddress}});
                }
            }

            // send
        }
        else
        {
            Logging::Log(Logging::LogLevel::Informational, "Delta distance > 255.");
        }
    }
    else
    {
        Logging::Log(Logging::LogLevel::Informational, "Packetsize is > MaxPacketSizeInBytes. Not sending.");
    }
}
