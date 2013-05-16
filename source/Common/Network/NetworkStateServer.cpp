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

#include "NetworkStateServer.hpp"

namespace GameInABox { namespace Common { namespace Network {

enum class State
{
    Idle,
    FailedConnection,
    Connected,

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
        boost::asio::ip::udp::endpoint clientAddress)
    : myStateManager(stateManager)
    , myAddress(clientAddress)
    , myState(State::Idle)
    , myFailReason("")
{
}

std::vector<NetworkPacket> NetworkStateServer::Process(std::vector<NetworkPacket>)
{
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
            // Test for a valid disconnect packet, otherwise, ignore everything
            // RAM: TODO!
            break;
        }

        // ///////////////////
        // Client
        // ///////////////////
        case State::Challenging:
        {
            // RAM: TODO!
            break;
        }

        case State::Connecting:
        {
            // RAM: TODO!
            break;
        }

        // ///////////////////
        // Server
        // ///////////////////
        case State::Listening:
        {
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

    return std::vector<NetworkPacket>();
}

void NetworkStateServer::Disconnect()
{
    // RAM: TODO
}

bool NetworkStateServer::IsConnected() const
{
    return myState == State::Connected;
}

bool NetworkStateServer::HasFailed() const
{
    return myState == State::FailedConnection;
}

/*
std::tuple<State, NetworkPacket,uint8_t, std::chrono::steady_clock::time_point> ProcessState(
        State currentState,
        NetworkPacket,// packet,
        uint8_t packetCount,
        std::chrono::steady_clock::time_point timeLastPacket,
        std::chrono::steady_clock::time_point)// timeCurrent)
{
    std::tuple<State, NetworkPacket,uint8_t, std::chrono::steady_clock::time_point> result(
                currentState,
                NetworkPacket(),
                packetCount,
                timeLastPacket);

    switch (currentState)
    {
        case State::Idle:
        {
            // Nothing.
            break;
        }

        case State::Challenging:
        {
            // Check for a ChallengeResponse packet, and if we don't
            // Get it either resend or timeout.
            break;
        }

        case State::Connecting:
        {
            // Check for a ConnectResponse packet, and if we don't
            // Get it either resend or timeout.
            // If we do get it then ask the state manager if we can connect.
            // RAM: TODO: breaks pure functional - what to do?
            break;
        }

        case State::Connected:
        {
            // check for a disconnect packet only.
            break;
        }

        case State::FailedConnection:
        {
            // Nothing.
            break;
        }
    };

    return result;
}
*/
}}} // namespace
