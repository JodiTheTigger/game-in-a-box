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

#include "NetworkManagerClientNew.h"

#include <string>

#include "NetworkProvider.h"
#include "IStateManager.h"
#include "NetworkPacket.h"

using std::string;

NetworkManagerClientNew::NetworkManagerClientNew(
        std::vector<std::unique_ptr<NetworkProvider>> networks,
        IStateManager& stateManager)
    : INetworkManager()
    , myNetworks(move(networks))
    , myStateManager(stateManager)
    , myConnectedNetwork(nullptr)
    , myState(State::Idle)
    , myServerKey(0)
    , myServerAddress()
    , myStateHandle()
    , myPacketSentCount(0)
    , myLastPacketSent()
{

}

NetworkManagerClientNew::~NetworkManagerClientNew()
{

}

void NetworkManagerClientNew::PrivateProcessIncomming()
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
                if (!network->IsDisabled())
                {
                    auto packets = network->Receive();

                    for (auto& packet : packets)
                    {
                        key = NetworkPacketHelper::GetKeyFromPacket(packet);

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
                myState = State::Connecting;
            }

            break;
        }

        case State::Connecting:
        {
            auto packets = myConnectedNetwork->Receive();

            for (auto& packet : packets)
            {
                bool exit(false);

                switch (NetworkPacketHelper::GetPacketType(packet))
                {
                    case PacketCommand::Command::ConnectResponse:
                    {
                        auto connection(NetworkPacketHelper::GetConnectResponsePacket(packet));

                        bool failed;
                        string failReason;
                        IStateManager::ClientHandle handle;

                        handle = myStateManager.Connect(connection->GetBuffer(), failed, failReason);

                        if (failed)
                        {
                            // Respond with a failed message please.
                            myConnectedNetwork->Send({{
                                  myServerAddress,
                                  PacketDisconnect(failReason).myBuffer}});

                            // wrong game type I assume.
                            myState = State::FailedConnection;
                        }
                        else
                        {
                            myStateHandle = handle;
                            myState = State::WaitingForDelta;
                        }

                        // Don't support connecting to multilpe servers at the same time.
                        exit = true;
                        break;
                    }

                    case PacketCommand::Command::Disconnect:
                    {
                        myState = State::FailedConnection;
                        exit = true;
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
        case State::WaitingForDelta:
        {
            break;
        }

        case State::Connected:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}

void NetworkManagerClientNew::PrivateSendState()
{    
    switch (myState)
    {
        case State::Challenging:
        {
            break;
        }

        case State::Connecting:
        {
            break;
        }
        case State::WaitingForDelta:
        {
            break;
        }

        case State::Connected:
        {
            break;
        }

        default:
        {
            break;
        }
    }
}

