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

#include "NetworkProviderInMemory.hpp"

namespace GameInABox { namespace Common { namespace Network {

NetworkProviderInMemory::NetworkProviderInMemory()
    : INetworkProvider()
    , myAddressToPackets()
    , myCurrentSource()
    , myNetworkIsDisabled(false)
{

}

std::vector<NetworkPacket> NetworkProviderInMemory::PrivateReceive()
{
    if (myAddressToPackets.count(myCurrentSource) > 0)
    {
        std::vector<NetworkPacket> result;

        swap(result, myAddressToPackets.at(myCurrentSource));

        return result;
    }

    return {};
}

void NetworkProviderInMemory::PrivateSend(std::vector<NetworkPacket> packets)
{
    for (auto packet : packets)
    {
        myAddressToPackets[packet.address].emplace_back(packet.data, myCurrentSource);
    }
}

void NetworkProviderInMemory::PrivateReset()
{
    myNetworkIsDisabled = false;
    myAddressToPackets = {};
}

void NetworkProviderInMemory::PrivateFlush()
{
    // NADA
}

void NetworkProviderInMemory::PrivateDisable()
{
    myNetworkIsDisabled = true;
}

bool NetworkProviderInMemory::PrivateIsDisabled() const
{
    return myNetworkIsDisabled;
}

}}} // namespace
