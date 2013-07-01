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


NetworkProviderInMemory::NetworkProviderInMemory(
        Milliseconds latencyMin,
        Milliseconds latencyAverage,
        Milliseconds latencyStandardDeviation,
        float packetLossChancePerPacket0to1,
        float packetLossChanceBurst0to1,
        float packetOutOfOrderChance0to1,
        TimeFunction timepiece)
    : INetworkProvider()
    , myAddressToPackets()
    , myCurrentSource()
    , myNetworkIsDisabled(false)
    , myTimeNow(timepiece)
    , myRandomEngine()
    , myRandom0To1(0, 1)
    , myLatency(latencyAverage.count(), latencyStandardDeviation.count())
    , myLatencyMinimum(latencyMin)
    , myPacketLossChancePerPacket0to1(packetLossChancePerPacket0to1)
    , myPacketLossChanceBurst0to1(packetLossChanceBurst0to1)
    , myPacketOutOfOrderChance0to1(packetOutOfOrderChance0to1)
    , losingPackets(false)
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
    Clock::time_point timeToRelease{};

    // latency
    if (myLatencyMinimum.count() > 0)
    {
        auto lag = std::max(
                    Milliseconds{static_cast<MillisecondStorageType>(myLatency(myRandomEngine))},
                    myLatencyMinimum);

        timeToRelease = myTimeNow() + lag;
    }
    else
    {
        timeToRelease = myTimeNow();
    }

    for (auto packet : packets)
    {
        auto timepacket = TimePacket{timeToRelease, NetworkPacket{std::move(packet.data), myCurrentSource}};
        auto& vector = myAddressToPackets[packet.address];
        vector.emplace_back(std::move(timepacket));

        // packet drop
        if (myPacketLossChancePerPacket0to1 > 0)
        {
            bool lostIt = false;

            if (losingPackets)
            {
                lostIt = (myRandom0To1(myRandomEngine) <= myPacketLossChanceBurst0to1);
            }
            else
            {
                lostIt = (myRandom0To1(myRandomEngine) <= myPacketLossChancePerPacket0to1);
            }

            if (lostIt)
            {
                vector.erase(end(vector) - 1);
                losingPackets = true;
            }
            else
            {
                losingPackets = false;
            }
        }
    }

    // packet swap.
    if (myRandom0To1(myRandomEngine) <= myPacketOutOfOrderChance0to1)
    {
        for (auto& addressPackets : myAddressToPackets)
        {
            auto& packets = addressPackets.second;

            if (packets.size() > 1)
            {
                using namespace std;

                // just swap the first and last packets.
                swap(packets.back(), packets.front());
            }
        }
    }
}

void NetworkProviderInMemory::PrivateReset()
{
    myNetworkIsDisabled = false;
    losingPackets = false;
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
