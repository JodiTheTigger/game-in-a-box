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

#include "NetworkManagerClient.hpp"

#include <string>
#include <chrono>
#include <vector>
#include <memory>
#include <array>

#include "Common/Logging/Logging.hpp"
#include "Common/IStateManager.hpp"
#include "Common/BitStream.hpp"
#include "Common/BitStreamReadOnly.hpp"
#include "INetworkProvider.hpp"
#include "NetworkPacket.hpp"
#include "PacketChallenge.hpp"
#include "PacketChallengeResponse.hpp"
#include "PacketDelta.hpp"
#include "PacketSimple.hpp"
#include "XorCode.hpp"
#include "BufferSerialisation.hpp"

using std::string;
using namespace std::chrono;
using namespace GameInABox::Common::Network;
using namespace GameInABox::Common::Logging;

NetworkManagerClient::NetworkManagerClient(
        std::vector<std::unique_ptr<INetworkProvider>> networks,
        IStateManager& stateManager)
    : INetworkManager()
    , myNetworks(move(networks))
    , myStateManager(stateManager)
    , myConnectedNetwork(nullptr)
    , myState(State::Idle)
    , myServerKey(0)
    , myServerKeyAsABuffer()
    , myServerAddress()
    , myStateHandle(nullptr)
    , myFailReason()
    , myClientId(0)
    , myDeltaHelper()
    , myCompressor(stateManager.GetHuffmanFrequencies())
    , myLastSequenceProcessed(0)
    , myLastSequenceAcked(0)
    , myPacketSentCount(0)
    , myLastPacketSent()
{

}

NetworkManagerClient::~NetworkManagerClient()
{

}


void NetworkManagerClient::Connect(boost::asio::ip::udp::endpoint serverAddress)
{
    // reset state please.
    for (auto& network : myNetworks)
    {
        network->Reset();
    }

    myConnectedNetwork = nullptr;

    myState = State::Challenging;
    myConnectedNetwork = nullptr;
    myServerKey = 0;
    myServerAddress = serverAddress;
    myStateHandle = nullptr;
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
    switch (myState)
    {
        case State::Challenging:
        {
            uint32_t key(0);
            boost::asio::ip::udp::endpoint serverAddress;

            // talking to all interfaces for now.
            for (auto& network : myNetworks)
            {
                bool exit(false);

                if (!network->IsDisabled())
                {
                    auto packets = network->Receive();

                    for (auto& packet : packets)
                    {                        
                        Command commandType(Packet::GetCommand(packet.data));

                        if (commandType == Command::Disconnect)
                        {
                            PacketDisconnect disconnect(packet.data);

                            if (disconnect.IsValid())
                            {
                                Fail(disconnect.Message());
                                exit = true;
                                break;
                            }
                        }
                        else
                        {
                            if (commandType == Command::ChallengeResponse)
                            {
                                PacketChallengeResponse response(packet.data);

                                if (response.IsValid())
                                {
                                    key = response.Key();
                                }
                            }
                        }

                        if (key != 0)
                        {
                            serverAddress = packet.address;
                            break;
                        }
                    }
                }

                if (key != 0)
                {
                    myConnectedNetwork = network.get();
                    myServerAddress = serverAddress;
                    exit = true;
                }

                if (exit)
                {
                    break;
                }
            }

            // Level up?
            if (myConnectedNetwork != nullptr)
            {
                // disable all other networks
                for (auto& network : myNetworks)
                {
                    if (network.get() != myConnectedNetwork)
                    {
                        network->Disable();
                    }
                }

                // set key and change state
                myServerKey = key;
                Push(myServerKeyAsABuffer.data(), myServerKey);
                myState = State::Connecting;
            }

            break;
        }

        case State::Connecting:
        {
            auto packets = myConnectedNetwork->Receive();

            // Fail?
            if (myConnectedNetwork->IsDisabled())
            {
                Fail("Network Failed Unexpectedly.");
                break;
            }

            for (auto& packet : packets)
            {
                bool exit(false);

                switch (Packet::GetCommand(packet.data))
                {
                    case Command::ConnectResponse:
                    {
                        PacketConnectResponse connection(packet.data);

                        if (connection.IsValid())
                        {
                            bool failed;
                            string failReason;
                            IStateManager::ClientHandle* handle;

                            handle = myStateManager.Connect(connection.GetBuffer(), failed, failReason);

                            if (failed)
                            {
                                // Respond with a failed message please.
                                // Only one will do, the server can timeout if it misses it.
                                myConnectedNetwork->Send({{
                                      PacketDisconnect(failReason).TakeBuffer(),
                                      myServerAddress}});

                                Fail(failReason);
                            }
                            else
                            {
                                myStateHandle = handle;
                                myState = State::Connected;
                            }

                            // Don't support connecting to multiple servers at the same time.
                            exit = true;
                        }

                        break;
                    }

                    case Command::Disconnect:
                    {
                        PacketDisconnect disconnect(packet.data);

                        if (disconnect.IsValid())
                        {
                            Fail(disconnect.Message());
                            exit = true;
                        }

                        break;
                    }

                    default:
                    {
                        // ignore
                        break;
                    }
                }

                if (exit)
                {
                    break;
                }
            }

            break;
        }

        case State::Connected:
        {
            // Cannot get disconnected unless the gamestate tells us to.
            // That is, ignore disconnect state packets.
            auto packets = myConnectedNetwork->Receive();

            for (auto& packet : packets)
            {
                if (PacketDelta::IsPacketDelta(packet.data))
                {
                    myDeltaHelper.AddPacket(PacketDelta(packet.data));
                }
            }

            // Do the work :-)
            DeltaReceive();

            break;
        }

        default:
        {
            break;
        }
    }
}

void NetworkManagerClient::PrivateSendState()
{    
    // Deal with timeout and resend logic here during
    // Connection handshaking. Otherwise get the lastest
    // state from the client and send a delta packet.
    switch (myState)
    {
        case State::Challenging:
        case State::Connecting:
        {
            if (myPacketSentCount > HandshakeRetries)
            {
                std::string failString("Timeout: ");

                if (myState == State::Challenging)
                {
                    failString += "Challenge.";
                }
                else
                {
                    failString += "Connecting.";
                }

                Fail(failString);
            }
            else
            {
                auto sinceLastPacket = steady_clock::now() - myLastPacketSent;

                if (duration_cast<milliseconds>(sinceLastPacket) > HandshakeRetryPeriod)
                {
                    // send challenge packet please.
                    if (myState == State::Challenging)
                    {
                        myConnectedNetwork->Send({{
                              PacketChallenge().TakeBuffer(),
                              myServerAddress}});
                    }
                    else
                    {
                        myConnectedNetwork->Send({{
                              PacketConnect(myServerKey).TakeBuffer(),
                              myServerAddress}});
                    }
                }
            }

            break;
        }

        case State::Connected:
        {
            DeltaSend();
            break;
        }

        default:
        {
            break;
        }
    }
}

void NetworkManagerClient::Fail(std::string failReason)
{
    for (auto& network : myNetworks)
    {
        network->Flush();
        network->Disable();
    }

    myFailReason = failReason;
    myState = State::FailedConnection;

    Logging::Log(Logging::LogLevel::Warning, failReason.c_str());
}

void NetworkManagerClient::DeltaReceive()
{
    PacketDelta delta(myDeltaHelper.GetDefragmentedPacket());
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
            Push(code.begin(), delta.GetSequence().Value());
            Push(code.begin() + 2, delta.GetSequenceAck().Value());
            XorCode(code.begin(), code.end(), myServerKeyAsABuffer);
            XorCode(payload.begin(), payload.end(), code);

            // Bah, I wrote Huffman and Bitstream before I knew about iterators
            // or streams. This results in lots of copies that arn't really needed.
            // Need to benchmark to see if the copies matter, and if so, rewrite
            // to use iterators or streams.

            // Decompress (2nd Copy)
            std::unique_ptr<std::vector<uint8_t>> decompressed;
            decompressed = move(myCompressor.Decode(payload));

            // Pass to gamestate (which will decompress the delta itself).
            BitStreamReadOnly payloadBitstream(payload);
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
    // RAM: TODO: How to find max size in advance?
    BitStream payloadBitstream(65535);
    Sequence from;
    Sequence to;

    myStateManager.DeltaGet(
                *myStateHandle,
                to,
                from,
                myLastSequenceAcked,
                payloadBitstream);

    // ignore if the delta distance is greater than 255, as we
    // store the distance as a byte.
    uint16_t distance(to - from);
    if (distance < 256)
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
        myConnectedNetwork->Send(packets);
    }
    else
    {
        Logging::Log(Logging::LogLevel::Informational, "Delta distance > 255.");
    }
}


