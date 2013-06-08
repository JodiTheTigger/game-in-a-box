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
    , myConnections()
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

    // Only keep a hash of network address to connetions.
    // If we get delta packets from an unrecognised endpoint
    // check the connection array incase it's an exisiting connection
    // that's had its port changed, otherwise ignore.
    // All other packets from an unrecognised address are treated as a new
    // connection.
    for (auto& network : myNetworks)
    {
        std::vector<NetworkPacket> responses{};

        auto packets = network->Receive();

        // process all the packets.

        for (auto& packet: packets)
        {
            if (myConnections.count(packet.address) > 0)
            {
                auto &connection = myConnections.at(packet.address);
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
                // If it's a delta packet, see if it's an existing connection.
                if (PacketDelta::IsPacketDelta(packet.data))
                {
                    auto delta = PacketDelta{packet.data};
                    if (delta.IsValid())
                    {
                        auto id = delta.IdConnection();

                        if (id)
                        {
                            //for (auto &connection : myConnections)
                            for (auto &connection : myConnections)
                            {
                                // same address?
                                if (connection.first.address() == packet.address.address())
                                {
                                    auto currentId = connection.second.IdConnection();

                                    if (currentId == id)
                                    {
                                        // copy the connection, don't care. Use move if metrics
                                        // say that this is too slow.
                                        myConnections.emplace(packet.address, connection.second);

                                        // remove the last one
                                        myConnections.erase(connection.first);
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                else
                {
                    myConnections.emplace(packet.address, Connection{myStateManager});

                    auto &connection = myConnections.at(packet.address);

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

        if (!responses.empty())
        {
            network->Send(responses);
        }
    }

    // Drop any disconnects, parse any deltas.
    // Disconnections are handled in privatesendstate by testing myStateManager.IsConnected().
    for (auto& connection : myConnections)
    {
        if (connection.second.HasFailed())
        {
            myConnections.erase(connection.first);

            // RAM: TODO: Log this?
        }

        if (connection.second.IsConnected())
        {
            auto delta = connection.second.GetDefragmentedPacket();

            if (delta.IsValid())
            {
                auto client = connection.second.IdClient();
                auto deltaData = Delta{delta.GetSequenceBase(), delta.GetSequence(), GetPayloadBuffer(delta)};

                if (client)
                {
                    myStateManager.DeltaParse(*client, deltaData);
                }
            }
        }
    }
}

void NetworkManagerServer::PrivateSendState()
{
    // TODO!
    for (auto& connection : myConnections)
    {
        if (connection.second.IsConnected())
        {
            auto client = connection.second.IdClient();

            if (client)
            {
                if (myStateManager.IsConnected(*client))
                {
                    // get the packet, and fragment it, then send it.
                    // RAM: TODO!
                }
                else
                {
                    connection.second.Disconnect("IStateManager: Not Connected.");
                }
            }
            else
            {
                // RAM: LOG?
                // WTF?
                // Ah well, clean up anyway.
                connection.second.Disconnect("Connection with no ClientId, wtf?");
            }
        }
    }
}

void NetworkManagerServer::Disconnect()
{
    // RAM: TODO!
    // RAM: Disconnect all clients, send their last packet,
    // flush the network buffers, disable them then quit.
}
