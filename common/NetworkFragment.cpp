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

#include "NetworkFragment.h"
#include "NetworkPacket.h"

using namespace std;

std::unique_ptr<NetworkFragment> NetworkFragment::GetFragmentFromData(NetworkPacket& packetData)
{
    unique_ptr<NetworkFragment> result;
    
    // Make sure we have data
    if (packetData.data)
    {
        // RAM: TODO: do the rest of the packet processing.
    }
    
    // C++ lets you do this without a std::move for return types.
    return result;
}

NetworkFragment::NetworkFragment(
    boost::asio::ip::udp::endpoint address,
    std::unique_ptr<std::vector<uint8_t>> data)
: myAddress(address)
, myData(move(data))
{
    // RAM: TODO: assert(myData->size >= PacketSizeMinimum)
    myDataRaw = &((*myData)[0]);
}

bool NetworkFragment::IsCommand() const
{
    return (myDataRaw[0] == 0xFF) && (myDataRaw[0] == myDataRaw[1]);
}

bool NetworkFragment::IsFragmented() const
{
    if (IsCommand())
    {
        return false;
    }
    else
    {
        return ((myDataRaw[0] & 0x80) != 0);
    }
}

uint16_t NetworkFragment::SequenceGet() const
{
    return U16Get(0);
}

uint16_t NetworkFragment::QPortGet() const
{
    uint16_t result;
    
    if (IsCommand())
    {
        result = 0;
    }
    else
    {
        result = U16Get(2);
    }
    
    return result;
}

uint16_t NetworkFragment::FragmentOffset() const
{
    if (!IsCommand() && IsFragmented())
    {
        return U16Get(4);
    }
    else
    {
        return 0;
    }
}

uint16_t NetworkFragment::FragmentTotalSizeInBytes() const
{
    if (!IsCommand() && IsFragmented())
    {
        return U16Get(6);
    }
    else
    {
        return 0;
    }
}

uint32_t NetworkFragment::KeyGet() const
{
    if (IsCommand())
    {
        return U32Get(4);
    }
    else
    {
        return 0;
    }    
}

NetworkFragment::Command NetworkFragment::CommandGet() const
{
    if (IsCommand())
    {
        return (NetworkFragment::Command) myDataRaw[8];
    }
    else
    {
        return Command::Invalid;
    }    
}

boost::asio::ip::udp::endpoint NetworkFragment::AddressGet() const
{
    return myAddress;
}

uint16_t NetworkFragment::U16Get(size_t offsetInBytes) const
{
    return (((uint16_t) myDataRaw[offsetInBytes]) << 8) | (uint16_t) myDataRaw[offsetInBytes + 1];
}

uint32_t NetworkFragment::U32Get(size_t offsetInBytes) const
{
    return (
        (((uint32_t) myDataRaw[offsetInBytes + 0]) << 24) | 
        (((uint32_t) myDataRaw[offsetInBytes + 1]) << 16) | 
        (((uint32_t) myDataRaw[offsetInBytes + 2]) << 8) | 
        (((uint32_t) myDataRaw[offsetInBytes + 3])));
}




