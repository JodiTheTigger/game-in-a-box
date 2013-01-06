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

#include "NetworkManager.h"
#include "StateManager.h"
#include "NetworkProvider.h"

using namespace std;
using namespace boost::asio::ip;

NetworkManager::NetworkManager(
        std::vector<std::unique_ptr<NetworkProvider>> networks,
        std::weak_ptr<StateManager> stateManager)
: myStateManager(stateManager)
, myNetworks()
{
    for (auto& network : networks)
    {
        if (network)
        {
            myNetworks.push_back(move(network));
        }
    }
}

void NetworkManager::ProcessIncomming()
{
    for (auto& network : myNetworks)
    {
        unique_ptr<vector<uint8_t>> data;
        udp::endpoint address;
        
        data.reset(new vector<uint8_t>());
        
        if (network->GetPacket(address, data))
        {
            // process data
            // TODO! make a packet factory that checks for a valid
            // packet and returns a packet which is either connectionless
            // or has bits filled in (but can still be a fragment).
            // GamePacket = Packet::GetPacket(address, data); if (GamePacket != nullptr) { Process packet further }
        }
    }
}

void NetworkManager::SendState()
{
    // Obviously TODO!
}


