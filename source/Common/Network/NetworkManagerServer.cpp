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
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "Common/IStateManager.hpp"

#include "INetworkProvider.hpp"
#include "NetworkPacket.hpp"
#include "PacketDelta.hpp"

#include "NetworkManagerServer.hpp"

using namespace GameInABox::Common::Network;

// TODO:
// Listen all all interfaces.
// Need a client object (address, Connection, DeltapacketClientId)
// need a map based on deltapacket client id to client
// (used if we get a delta packet for a non-existing client, test against id instead).
// need another map based on address to client (used more often)

NetworkManagerServer::NetworkManagerServer(
        std::vector<MotleyUniquePointer<INetworkProvider>> networks,
        IStateManager& stateManager)
    : INetworkManager()
    , myNetworks(move(networks))
    , myStateManager(stateManager)
    , myConnections()
    , myConnectedClients()
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
    // But, assumption == nullptr. Do metrics or shut up.
    for (auto& network : myNetworks)
    {
        std::vector<NetworkPacket> responses{};

        auto packets = network->Receive();

        for (auto& packet: packets)
        {
            if (myConnections.count(packet.address) == 0)
            {
                // if it's a delta packet, see if the client id is recognised.
                if (PacketDelta::IsPacketDelta(packet.data))
                {

                }
                else
                {
                    // new connection.
                    // Well, bother. The line below doesn't work as connection
                    // doesn't have assignment or move constructors because
                    // it requires a reference to IStateManager, which is not
                    // assignable or moveable. Crap.
                    //myConnections[packet.address] = Connection( myStateManager );
                    myConnections.at(packet.address).Start(Connection::Mode::Server);
                }
            }

            // Can't use the [] index as it requires a default constrcutor.
            auto response = myConnections.at(packet.address).Process(move(packet.data));

            if (!response.empty())
            {
                responses.emplace_back(move(response), packet.address);
            }
        }

        if (!responses.empty())
        {
            network->Send(responses);
        }
    }
}

void NetworkManagerServer::PrivateSendState()
{
    // TODO!
}

void NetworkManagerServer::Disconnect()
{
}
