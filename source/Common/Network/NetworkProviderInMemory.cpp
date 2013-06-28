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

#include "NetworkPacket.hpp"

#include "NetworkProviderInMemory.hpp"

using Clock = std::chrono::high_resolution_clock;
using Oclock = Clock::time_point;

namespace GameInABox { namespace Common { namespace Network {

NetworkProviderInMemory::NetworkProviderInMemory(TimeFunction timepiece)
    : INetworkProvider()
    , myAddressToPackets()
    , myCurrentSource()
    , myNetworkIsDisabled(false)
    , myTimeNow(timepiece)
{

}

std::vector<NetworkPacket> NetworkProviderInMemory::PrivateReceive()
{
    if (myAddressToPackets.count(myCurrentSource) > 0)
    {
        std::vector<NetworkPacket> result;
        std::vector<TimePacket> keep;
        auto now = myTimeNow();

        for (auto& timePacket : myAddressToPackets.at(myCurrentSource))
        {
            if (timePacket.timeToRelease < now)
            {
                result.emplace_back(std::move(timePacket.data));
            }
            else
            {
                keep.emplace_back(std::move(timePacket));
            }
        }

        swap(keep, myAddressToPackets.at(myCurrentSource));

        return result;
    }

    return {};
}

void NetworkProviderInMemory::PrivateSend(std::vector<NetworkPacket> packets)
{
    // RAM: TODO: Add latency: but for now - no latency.
    for (auto packet : packets)
    {
        auto timepacket = TimePacket{myTimeNow(), NetworkPacket{std::move(packet.data), myCurrentSource}};
        myAddressToPackets[packet.address].emplace_back(std::move(timepacket));
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
