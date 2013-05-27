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

#include "Common/IStateManager.hpp"

#include "INetworkProvider.hpp"
#include "NetworkPacket.hpp"
#include "NetworkManagerServer.hpp"

using namespace GameInABox::Common::Network;

// TODO:
// Listen all all interfaces.
// Need a client object (address, Connection, DeltapacketClientId)
// need a map based on deltapacket client id to client
// (used if we get a delta packet for a non-existing client, test against id instead).
// need another map based on address to client (used more often)
// To protect against client id remap reassing flooding DOS, need to check something else
// as well to increse the bitdepth to deter attacks (currently depth is 16bits).

NetworkManagerServer::NetworkManagerServer(
        std::vector<MotleyUniquePointer<INetworkProvider>> networks,
        IStateManager& stateManager)
    : INetworkManager()
    , myNetworks(move(networks))
    , myStateManager(stateManager)
    //, myConnections()
    //, myConnectedClients()
{
}

NetworkManagerServer::~NetworkManagerServer()
{
    Disconnect();
}

void NetworkManagerServer::PrivateProcessIncomming()
{
    // NOTE: Quake 3 does an array search to match ip address to
    // connected clients. That would work fine for 8-64 players.
    // However I would assume > 64 players hashing might be better.
    /*
    for (auto& network : myNetworks)
    {
        auto packets = network->Receive();

        for (auto& packet: packets)
        {
            if (myConnections.count(packet.address))
            {
                auto response = myConnections[packet.address].Process(move(packet.data));
            }
        }
    }
    */
}

void NetworkManagerServer::PrivateSendState()
{
    // TODO!
}

void NetworkManagerServer::Disconnect()
{
}
