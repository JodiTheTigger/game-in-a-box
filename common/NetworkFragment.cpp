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

// Packet Format
// =============
// Info coded high byte first.
// 
// 00: uint16_t sequence (0xFFFF == Ignored out of band command packet)
// 02: uint16_t qport
// If (sequence & 0x8000)
// {
// 04: uint16_t fragment offset
// 06: uint16_t fragment total length in bytes
// 08: uint8_t[] data
// }
// else
// {
// 04: uint8_t[] data
// }

std::unique_ptr<NetworkFragment> NetworkFragment::GetFragmentFromData(NetworkPacket&)
{
    unique_ptr<NetworkFragment> result;
    
    // Make sure we have data
    //if (packetData.data)
    {
        // RAM: TODO: do the rest of the packet processing.
    }
    
    // C++ lets you do this without a std::move for return types.
    return result;
}

NetworkFragment::NetworkFragment(NetworkPacket& packetData)
: packet(move(packetData))
, dataOffset(NetworkFragment::MinimumSize)
{
    if (packet.data[0] & 0x80)
    {
        isFragmented = true;
        dataOffset++;
        fragmentId = packet.data[4];
    }
    
    sequence =  ((uint16_t) (packet.data[0] & 0x7F)) << 8;
    sequence |= (uint16_t) packet.data[1];
    
    linkId =    ((uint16_t) packet.data[0]) << 8;
    linkId |=   (uint16_t) packet.data[1];    
}




