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
#endif

#include "Common/Logging/Logging.hpp"
#include "Common/IStateManager.hpp"
#include "Common/BitStream.hpp"
#include "Common/BitStreamReadOnly.hpp"

#include "INetworkProvider.hpp"
#include "NetworkPacket.hpp"
#include "PacketChallenge.hpp"
#include "PacketChallengeResponse.hpp"
#include "PacketDelta.hpp"
#include "PacketTypes.hpp"
#include "XorCode.hpp"
#include "BufferSerialisation.hpp"
#include "Connection.hpp"

#include "NetworkManagerClient.hpp"

using std::string;
using namespace std::chrono;
using namespace GameInABox::Common::Network;
using namespace GameInABox::Common::Logging;

struct NetworkManagerClient::LiveConnection
{
    MotleyUniquePointer<INetworkProvider> transport;
    Connection handshake;
};

NetworkManagerClient::NetworkManagerClient(
        std::vector<MotleyUniquePointer<INetworkProvider>> networks,
        IStateManager& stateManager)
    : INetworkManager()
    , myNetworks()
    , myConnectedNetwork(nullptr)
    , myStateManager(stateManager)
    , myState(State::Idle)
    , myServerKey(GetNetworkKeyNil())
    , myServerAddress()
    , myStateHandle()
    , myFailReason()
    , myClientId(0)
    , myCompressor(stateManager.GetHuffmanFrequencies())
    , myLastSequenceProcessed(0)
    , myLastSequenceAcked(0)
    , myPacketSentCount(0)
    , myLastPacketSent()
{
    for (auto& network : networks)
    {
        myNetworks.push_back({move(network), {stateManager}});

        // RAM: TODO! Why doesn't emplace_back compile?
        //myNetworks.emplace_back(move(network), {stateManager});
        //myNetworks.emplace_back({move(network), {stateManager}});
    }
}

NetworkManagerClient::~NetworkManagerClient()
{

}

void NetworkManagerClient::Connect(boost::asio::ip::udp::endpoint serverAddress)
{
    // reset state please.
    for (auto& network : myNetworks)
    {
        network.transport->Reset();
        network.handshake.Start(Connection::Mode::Client);
    }

    myState = State::Challenging;
    myConnectedNetwork = nullptr;
    myServerKey = GetNetworkKeyNil();
    myServerAddress = serverAddress;
    myStateHandle = {};
    myFailReason = "";

    myLastSequenceProcessed = Sequence(0);
    myLastSequenceAcked = Sequence(0);

    myPacketSentCount = 0;
    myLastPacketSent = std::chrono::steady_clock::time_point::min();

    // Kick off an OOB send.
    PrivateSendState();
}

void NetworkManagerClient::Disconnect()
{
    Fail("Client disconnected.");
}

void NetworkManagerClient::PrivateProcessIncomming()
{
    if (myConnectedNetwork != nullptr)
    {
        if (myStateManager.IsConnected(*myStateHandle))
        {
            auto packets = myConnectedNetwork->transport->Receive();

            for (auto& packet : packets)
            {
                if (packet.address == myServerAddress)
                {
                    auto response = myConnectedNetwork->handshake.Process(packet.data);
                    myConnectedNetwork->transport->Send({{response, myServerAddress}});

                    if (myConnectedNetwork->handshake.HasFailed())
                    {
                        Fail(myConnectedNetwork->handshake.FailReason());
                        break;
                    }
                }
            }

            // Do some work :-)
            if (myConnectedNetwork->handshake.IsConnected())
            {
                DeltaReceive();
            }
        }
        else
        {
            // No longer connected, quit out.
            Fail("State is no longer connected.");
        }
    }
    else
    {
        // talking to all interfaces for now.
        bool fail = true;
        for (auto& network : myNetworks)
        {
            if (!network.transport->IsDisabled())
            {
                auto packets = network.transport->Receive();
                std::vector<NetworkPacket> toSend{};

                for (auto& packet : packets)
                {
                    if (packet.address == myServerAddress)
                    {
                        auto response = network.handshake.Process(packet.data);
                        toSend.emplace_back(response, myServerAddress);

                        // Win, lose or nothing?
                        if (network.handshake.IsConnected())
                        {
                            // Win
                            myConnectedNetwork = &network;
                            break;
                        }
                        else
                        {
                            if (network.handshake.HasFailed())
                            {
                                // Lose.
                                network.transport->Send(toSend);
                                network.transport->Flush();
                                network.transport->Disable();
                                break;
                            }
                            else
                            {
                                // Nothing.
                                fail = false;
                            }
                        }
                    }
                    else
                    {
                        Logging::Log(Logging::LogLevel::Debug, "Recieved packet not addressed to me.");
                    }
                }

                if ((!network.transport->IsDisabled()) && (!toSend.empty()))
                {
                    network.transport->Send(toSend);
                }
            }

            // clean up
            if (myConnectedNetwork != nullptr)
            {
                for (auto& network : myNetworks)
                {
                    if (&network != myConnectedNetwork)
                    {
                        network.transport->Disable();
                    }
                }
            }
            else
            {
                if (fail)
                {
                    Fail("Connection attempt failed.");
                }
            }
        }
    }
}

void NetworkManagerClient::PrivateSendState()
{        
    if (myConnectedNetwork != nullptr)
    {
        // Don't bother checkng state, as when connected it can't do
        // anything without receiving a packet first.
        DeltaSend();
    }
    else
    {
        // talking to all interfaces for now.
        for (auto& network : myNetworks)
        {
            if (!network.transport->IsDisabled())
            {
                auto response = network.handshake.Process({});
                network.transport->Send({{response, myServerAddress}});

                // Lose?
                if (network.handshake.HasFailed())
                {
                    // Lose.
                    network.transport->Flush();
                    network.transport->Disable();
                    break;
                }
            }
        }
    }
}

void NetworkManagerClient::Fail(std::string failReason)
{
    for (auto& network : myNetworks)
    {
        if (!network.transport->IsDisabled())
        {
            // Only be polite if we are already connected.
            if (network.handshake.IsConnected())
            {
                network.handshake.Disconnect(failReason);
                auto lastPacket = network.handshake.Process({});

                network.transport->Send({{lastPacket, myServerAddress}});
            }

            network.transport->Flush();
            network.transport->Disable();
        }
    }

    myFailReason = failReason;
    myState = State::FailedConnection;

    Logging::Log(Logging::LogLevel::Notice, failReason.c_str());
}

void NetworkManagerClient::DeltaReceive()
{
    auto delta = myConnectedNetwork->handshake.GetDefragmentedPacket();
    Sequence latestSequence(myLastSequenceProcessed);

    if (delta.IsValid())
    {
        if (delta.GetSequence() > latestSequence)
        {
            latestSequence = delta.GetSequence();

            // First copy
            std::vector<uint8_t> payload(delta.GetPayload());

            // Decrypt (XOR based).
            // NOTE: nothing too complex for encryption, even more simple than q3.
            // As someone wanting to hack can. If we want security, use public key
            // private key to pass a super long seed to a pseudo random generator
            // that's used to decrypt. Otherwise it's just easily hackable.
            // Reason for excryption in the fist place is to prevent easy man-in-the-middle
            // attacks to control someone else's connection.
            std::vector<uint8_t> code(4);
            Push(begin(code), delta.GetSequence().Value());
            Push(begin(code) + 2, delta.GetSequenceAck().Value());
            XorCode(begin(code), end(code), myServerKey.data);
            XorCode(begin(payload), end(payload), code);

            // Bah, I wrote Huffman and Bitstream before I knew about iterators
            // or streams. This results in lots of copies that arn't really needed.
            // Need to benchmark to see if the copies matter, and if so, rewrite
            // to use iterators or streams.

            // Decompress (2nd Copy)
            auto decompressed = move(myCompressor.Decode(payload));

            // Pass to gamestate (which will decompress the delta itself).
            BitStreamReadOnly payloadBitstream(*decompressed);
            myStateManager.DeltaSet(
                *myStateHandle,
                delta.GetSequence().Value(),
                delta.GetSequenceBase().Value(),
                payloadBitstream);

            // Now see what the last packet the other end has got.
            myLastSequenceAcked = delta.GetSequenceAck();
        }
    }
}

void NetworkManagerClient::DeltaSend()
{
    BitStream payloadBitstream(MaxPacketSizeInBytes);
    Sequence from;
    Sequence to;

    // Delta compression aggression should be inferred by the packet distance,
    // thus how well the packet compresses, therefore how big the packet should get.
    // A large packet delta should require more aggressive delta creation to get
    // smaller packet sizes.
    myStateManager.DeltaGet(
                *myStateHandle,
                to,
                from,
                myLastSequenceAcked,
                payloadBitstream);

    if (payloadBitstream.SizeInBytes() <= MaxPacketSizeInBytes)
    {
        // ignore if the delta distance is greater than 255, as we
        // store the distance as a byte.
        auto distance = to - from;
        if (distance <= PacketDelta::MaximumDeltaDistance())
        {
            PacketDelta delta(
                    from,
                    myLastSequenceProcessed,
                    uint8_t(distance),
                    &myClientId,
                    *(payloadBitstream.TakeBuffer()));

            // Fragment (if needed)
            std::vector<NetworkPacket> packets;
            auto fragments(PacketDeltaFragmentManager::FragmentPacket(delta));
            for (auto& fragment : fragments)
            {
                packets.emplace_back(fragment.TakeBuffer(), myServerAddress);
            }

            // send
            myConnectedNetwork->transport->Send(packets);
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
