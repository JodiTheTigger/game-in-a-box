/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012-2013 Richard Maxwell <jodi.the.tigger@gmail.com>
    
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
#include <chrono>
#include <tuple>
#endif

#include "Common/IStateManager.hpp"
#include "NetworkPacket.hpp"
#include "PacketTypes.hpp"
#include "PacketDisconnect.hpp"
#include "PacketChallenge.hpp"
#include "PacketChallengeResponse.hpp"


#include "NetworkStateServer.hpp"

using namespace std::chrono;
using Timepoint = std::chrono::steady_clock::time_point;

namespace GameInABox { namespace Common { namespace Network {

enum class State
{
    Idle,
    FailedConnection,
    Connected,
    Disconnecting,

    // Server States
    Listening,

    // Client States
    Challenging,
    Connecting,
};

// pure functional state handling.
/*
std::tuple<State, NetworkPacket,uint8_t, std::chrono::steady_clock::time_point> ProcessState(
        State currentState,
        NetworkPacket packet,
        uint8_t packetCount,
        std::chrono::steady_clock::time_point timeLastPacket,
        std::chrono::steady_clock::time_point timeCurrent);
*/
using namespace GameInABox::Common::Network;

NetworkStateServer::NetworkStateServer(
        IStateManager& stateManager,
        boost::asio::ip::udp::endpoint address)
    : myStateManager(stateManager)
    , myAddress(address)
    , myState(State::Idle)
    , myFailReason("")
    , myKey(GetNetworkKeyNil())
    , myPacketCount(0)
    , myLastTimestamp(Timepoint::min())
    , myStateHandle()
{
}


void NetworkStateServer::StartClient()
{
    Reset(State::Challenging);
}

void NetworkStateServer::StartServer()
{
    Reset(State::Listening);
}

void NetworkStateServer::Disconnect()
{
    myStateManager.Disconnect(myStateHandle);
    Reset(State::Disconnecting);
}

std::vector<NetworkPacket> NetworkStateServer::Process(NetworkPacket packet)
{
    std::vector<NetworkPacket> result;

    // NOTE:
    // pure things:
    // State, NetworkPacket, packetCount, timeLastPacket, timeCurrent, failReason
    // impure things:
    // IStateManager, Disconnecting, Fail()
    // tried to have this all in a pure function, but calling IStateManager connect
    // and disconnect was impure, wrong place to try.

    switch (myState)
    {
        // ///////////////////
        // Common
        // ///////////////////
        case State::Idle:
        {
            // Nothing, ignore everything
            break;
        }

        case State::FailedConnection:
        {
            // Again, Nothing, ignore everything.
            break;
        }

        case State::Connected:
        {
            if (packet.address == myAddress)
            {
                // check to see if we disconnect.
                Disconnected(packet);
            }

            break;
        }

        case State::Disconnecting:
        {
            std::string failReason("Normal Disconnect.");

            result.emplace_back(
                PacketDisconnect(myKey, failReason).TakeBuffer(),
                myAddress);
            Fail(failReason);
            break;
        }

        // ///////////////////
        // Client
        // ///////////////////
        case State::Challenging:
        {
            if (packet.address == myAddress)
            {
                if (!Disconnected(packet))
                {
                    if (Packet::GetCommand(packet.data) == Command::ChallengeResponse)
                    {
                        PacketChallengeResponse response(packet.data);

                        if (response.IsValid())
                        {
                            // RAM: TODO! Check version!
                            myKey = response.Key();
                            Reset(State::Connecting);
                        }
                    }
                }
            }

            // state didn't change on us did it?
            if (myState == State::Challenging)
            {
                if (myPacketCount > HandshakeRetries)
                {
                    Fail("Timeout: Challenging.");
                }
                else
                {
                    auto sinceLastPacket = GetTimeNow() - myLastTimestamp;

                    if (duration_cast<milliseconds>(sinceLastPacket) > HandshakeRetryPeriod())
                    {
                        result.emplace_back(
                            PacketChallenge().TakeBuffer(),
                            myAddress);

                        myLastTimestamp = GetTimeNow();
                        ++myPacketCount;
                    }
                }
            }

            break;
        }

        case State::Connecting:
        {
            if (packet.address == myAddress)
            {
                if (!Disconnected(packet))
                {
                    if (Packet::GetCommand(packet.data) == Command::ConnectResponse)
                    {
                        PacketConnectResponse connection(packet.data);

                        if (connection.IsValid())
                        {
                            bool failed;
                            std::string failReason;
                            ClientHandle handle;

                            handle = myStateManager.Connect(connection.GetBuffer(), failed, failReason);

                            if (failed)
                            {
                                // Respond with a failed message please.
                                // Only one will do, the server can timeout if it misses it.
                                result.emplace_back(
                                    PacketDisconnect(myKey, failReason).TakeBuffer(),
                                    myAddress);

                                Fail(failReason);
                            }
                            else
                            {
                                myStateHandle = handle;
                                Reset(State::Connected);
                            }
                        }
                    }
                }
            }

            // RAM: duplicate code! remove!

            // state didn't change on us did it?
            if (myState == State::Connecting)
            {
                if (myPacketCount > HandshakeRetries)
                {
                    Fail("Timeout: Connecting.");
                }
                else
                {
                    auto sinceLastPacket = GetTimeNow() - myLastTimestamp;

                    if (duration_cast<milliseconds>(sinceLastPacket) > HandshakeRetryPeriod())
                    {
                        result.emplace_back(
                            PacketConnect(myKey).TakeBuffer(),
                            myAddress);

                        myLastTimestamp = GetTimeNow();
                        ++myPacketCount;
                    }
                }
            }

            break;
        }

        // ///////////////////
        // Server
        // ///////////////////
        case State::Listening:
        {
            // RAM: just react to packets, no resends or timeouts.
            // RAM: if i do that, how do I know we're connected?
            // RAM: TODO!
            break;
        }
    }

    /* copy and pasted from networkmanagerclient.
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
                            ClientHandle handle;

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
            if (myStateManager.IsConnected(myStateHandle))
            {
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
            }
            else
            {
                // No longer connected, quit out.
                // RAM: TODO! Need way of telling the server this too!
                Fail("State is no longer connected.");
            }

            break;
        }

        default:
        {
            break;
        }
    }*/

    return result;
}

bool NetworkStateServer::IsConnected() const
{
    return myState == State::Connected;
}

bool NetworkStateServer::HasFailed() const
{
    return myState == State::FailedConnection;
}

void NetworkStateServer::Reset(State resetState)
{
    myState         = resetState;
    myFailReason    = "";
    myPacketCount   = 0;
    myLastTimestamp = Timepoint::min();
}

void NetworkStateServer::Fail(std::string failReason)
{
    myState = State::FailedConnection;
    myFailReason = failReason;
}

bool NetworkStateServer::Disconnected(const NetworkPacket& packet)
{
    bool result(false);

    if (Packet::GetCommand(packet.data) == Command::Disconnect)
    {
        PacketDisconnect disconnect(packet.data);

        if (disconnect.IsValid())
        {
            if (disconnect.Key() == myKey)
            {
                Fail(disconnect.Message());
                result = true;
            }
        }
    }

    return result;
}

std::chrono::steady_clock::time_point NetworkStateServer::GetTimeNow()
{
    return std::chrono::steady_clock::now();
}

}}} // namespace
