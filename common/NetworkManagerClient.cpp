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

#include "NetworkManagerClient.h"
#include "NetworkPacket.h"

#include "NetworkProvider.h"
#include "IStateManager.h"

using namespace std;
using namespace std::chrono;

// I don't understand constexpr
constexpr std::chrono::milliseconds NetworkManagerClient::HandshakeRetryPeriod;

NetworkManagerClient::NetworkManagerClient(
        std::vector<std::unique_ptr<NetworkProvider>> networks,
        std::weak_ptr<IStateManager> stateManager)
    : NetworkPacketParser(PacketEncoding::FromServer)
    , myStateManager(stateManager)
    , myNetworks()
    , myState(State::Idle)
    , myKey(0)
    , myServerAddress()
    , myServerInterface(networks.size())
    , myLastPacketSent()
    , myConnectData()
    , myClientId(nullptr)
    , myPacketSentCount(0)
{
    for (auto& network : networks)
    {
        if (network)
        {
            myNetworks.push_back(move(network));
        }
    }
}

void NetworkManagerClient::Connect(boost::asio::ip::udp::endpoint serverAddress, std::vector<uint8_t> connectData)
{
    for(auto& network : myNetworks)
    {
        network->Reset();
    }

    myConnectData = connectData;
    myClientId = nullptr;
    myState = State::Challenging;
    myKey = 0;
    myServerAddress = serverAddress;
    myPacketSentCount = 0;
    myServerInterface = myNetworks.size();
    SendChallengePacket();
}

bool NetworkManagerClient::IsConnected()
{
    return myState==State::Connected;
}

bool NetworkManagerClient::IsFailed()
{
    return myState==State::Failed;
}

void NetworkManagerClient::SendChallengePacket()
{
    // send over all compatible interfaces
    for (auto& network : myNetworks)
    {
        std::vector<NetworkPacket> packetToSend;

        packetToSend.emplace_back(myServerAddress, ChallengePacket);

        network->Send(packetToSend);
        myPacketSentCount++;
        myLastPacketSent = steady_clock::now();
    }
}

void NetworkManagerClient::SendConnectPacket()
{
    std::vector<NetworkPacket> packetToSend;

    packetToSend.emplace_back(myServerAddress, ChallengePacket);

    myNetworks[myServerInterface]->Send(packetToSend);
    myPacketSentCount++;
    myLastPacketSent = steady_clock::now();
}

void NetworkManagerClient::SendDisconnectPacket(std::string)
{
    std::vector<NetworkPacket> packetToSend;

    // RAM: TODO
    //packetToSend.push_back({myServerAddress, ChallengePacket});

    myNetworks[myServerInterface]->Send(packetToSend);

    myState = State::Failed;
}

void NetworkManagerClient::ParseCommand(NetworkPacket &packetData)
{    
    switch (myState)
    {
        case State::Connecting:
        case State::Challenging:
        {
            Command command;

            command = Command(packetData.data[OffsetCommand]);

            if (command == Command::ChallengeResponse)
            {
                myKey = KeyGet(packetData);
                myState = State::Connecting;

                myPacketSentCount = 0;
                SendConnectPacket();

                // RAM: TODO: Choose myServerInterface and disable all other interfaces please.
                // RAM: HOW? Means the packet needs to keep track of which interface sent it.
                // RAM: No it doesn't, the caller knows where the back comes from, just
                // RAM: communicate with that telling it to close all other interfaces.
            }
            else
            {
                auto state = myStateManager.lock();

                if (state)
                {
                    //IStateManager::ClientHandle* newGuy;
                    bool failed;
                    string failReason;
                    vector<uint8_t> connectData;

                    connectData = ConnectDataGet(packetData);

                    /*newGuy = */state->Connect(connectData, failed, failReason);

                    if (failed)
                    {                        
                        // Noooooo!
                        SendDisconnectPacket(failReason);
                    }
                    else
                    {
                        myState = State::Connected;
                        myConnectData.clear();
                    }
                }
                else
                {
                    // I'm in bad shape, tell the server I'm disconnecting.                    
                    SendDisconnectPacket("State Manager is a nullptr.");
                }
            }

            break;
        }

        default:
        {
            // NADA
            break;
        }
    }
}

void NetworkManagerClient::ParseDelta(NetworkPacket &)
{
    // Ignore if not connected
    if (myState == State::Connected)
    {
        // RAM: TODO: Parse the packet please.
        // TODO: Decrypt
        // TODO: Expand
        // TODO: Pass to gamestate.
    }
    else
    {
        SendDisconnectPacket("I am not connected!");
    }
}

void NetworkManagerClient::PrivateProcessIncomming()
{    
    if ((myState != State::Idle) && (myState != State::Failed))
    {
        // TODO!
    }
}

void NetworkManagerClient::PrivateSendState()
{
    switch (myState)
    {
        case State::Connecting:
        case State::Challenging:
        {
            if (myPacketSentCount > HandshakeRetries)
            {
                // RAM: TODO: do state clean up on fail or connect?
                // Put cleanup into once function please.
                SendDisconnectPacket("Server Timeout");
            }
            else
            {
                auto sinceLastPacket = steady_clock::now() - myLastPacketSent;

                if (duration_cast<milliseconds>(sinceLastPacket) > HandshakeRetryPeriod)
                {
                    if (myState == State::Challenging)
                    {
                        SendChallengePacket();
                    }
                    else
                    {
                        SendConnectPacket();
                    }
                }
            }

            break;
        }

        //case State::Connected:
        //{
            // RAM: TODO!
        //    break;
        //}

        default:
        {
            // Nothing!
            break;
        }
    }
}
