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

#include "Common/Logging/Logging.hpp"
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


// RAM: TODO! Add network metrics (bytes per second, send, recieve, rolling stats)
// RAM: TODO! Add Network throttling here
// RAM: TODO! Add Network throttling per destination as well (so global throttle, and per connection)
// RAM: TODO! use function pointers (std::function) to do throttling and stats. However that would be
// Set at constructor time, so isn't part of the interface. Should I make setters for those
// Things in the interface itself?
// eg:
// using GateKeeper = std::function<bool(size, timestamp, address)>;
// void SetGatekeeperSend(GateKeeper).
// void SetGatekeeperReceive(GateKeeper).
// where the function returns true if the send can go ahead, and is passed in details about
// that send/receive.
//
// NOTE: Network metrics are used by the client state, make a new interface to pass that info
// to the state.

NetworkManagerServer::NetworkManagerServer(
        std::vector<MotleyUniquePointer<INetworkProvider>> networks,
        IStateManager& stateManager)
    : INetworkManager()
    , myNetworks(move(networks))
    , myStateManager(stateManager)
    , myConnecting()
    , myConnected()
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
            // Most packets will be delta packets of already
            // connected clients. Deal with that first.
            if (myConnected.count(packet.address) > 0)
            {
                auto &connection = myConnected.at(packet.address);
                auto response = connection.Process(move(packet.data));

                if (!response.empty())
                {
                    if (myStateManager.CanPacketSend(connection.IdClient(), response.size()))
                    {
                        responses.emplace_back(move(response), packet.address);
                    }
                }
            }
            else
            {
                // Most packets will be connection handshake or info request packets
                if (myConnecting.count(packet.address) > 0)
                {                    
                    auto &connection = myConnected.at(packet.address);
                    auto response = connection.Process(move(packet.data));

                    if (!response.empty())
                    {
                        if (myStateManager.CanPacketSend(connection.IdClient(), response.size()))
                        {
                            responses.emplace_back(move(response), packet.address);
                        }
                    }

                    if (myConnected.at(packet.address).IsConnected())
                    {
                        myConnected.emplace(packet.address, std::move(myConnecting.at(packet.address)));
                        myConnecting.erase(packet.address);
                    }
                }
                else
                {
                    // Next are new connetion requests followed by
                    // the least frequent packets: port or address hopping
                    // connected clients.
                    if (PacketDelta::IsPacketDelta(packet.data))
                    {
                        // Client packets are not allowed to fragment.
                        //checktoseeifmoved;
                    }
                    else
                    {
                        myConnecting.emplace(packet.address, Connection{myStateManager});

                        auto &connection = myConnected.at(packet.address);

                        connection.Start(Connection::Mode::Server);

                        auto response = connection.Process(move(packet.data));

                        if (!response.empty())
                        {
                            if (myStateManager.CanPacketSend(connection.IdClient(), response.size()))
                            {
                                responses.emplace_back(move(response), packet.address);
                            }
                        }
                    }
                }
            }
        }

            /*
            if (PacketDelta::IsPacketDelta(packet))
            {
                PacketDelta delta{move(packet.data)};

                if (delta.HasClientId())
                {
                    if (myConnectedClients.count(delta.ClientId()) > 0)
                    {
                        // already connected client.
                        // Test
                    }
                }
                else
                {
                    // wtf? ignore
                    Logging::Log(Logging::LogLevel::Debug, "PacketDelta doesn't have a ClientId.");
                }
            }
            else
            {

            }
            */
            /*
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
        }*/
    }

    // RAM: TODO: Process any deltas from the connected clients.
}

void NetworkManagerServer::PrivateSendState()
{
    // TODO!
}

void NetworkManagerServer::Disconnect()
{
}
