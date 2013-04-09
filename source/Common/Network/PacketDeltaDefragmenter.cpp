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

#include "PacketDeltaDefragmenter.h"

PacketDeltaDefragmenter::PacketDeltaDefragmenter()
{
}

std::vector<PacketDelta> PacketDeltaDefragmenter::FragmentPacket(PacketDelta)
{
    std::vector<PacketDelta> result;
 /*
    if (toFragment.IsValid())
    {
        if (toFragment.HasClientId())
        if (toFragment.myBuffer.size() - PacketDelta::)
    }
    */
    return result;
}

void PacketDeltaDefragmenter::AddPacket(PacketDelta)
{
    /* Copy pasted from old file NetworkPacketParser.cpp
    if (myEncodingDetails == PacketEncoding::FromServer)
    {
        uint8_t fragmentId;

        fragmentId = packetData.data[OffsetFragmentId];

        // deal with the fragment.
        if (fragmentId & 0x80)
        {
            fragmentId &= 0x7F;
            myFragmentTotal = fragmentId + 1;
        }

        // watch out for packets out of range
        if (myFragmentCount < myFragmentTotal)
        {
            // don't count duplicates
            if (myFragments[fragmentId].data.size() == 0)
            {
                ++myFragmentCount;
            }

            myFragments[fragmentId] = move(packetData);

            // got them all?
            if (myFragmentCount == myFragmentTotal)
            {
                NetworkPacket notFragmented;

                notFragmented = PacketDefragment(myFragments);

                // clean up
                myFragmentCount = 0;
                myFragmentTotal = 0;
                myFragments.clear();

                // Not fragmented, yippie.
                ParseDelta(notFragmented);
            }
        }
    }
*/
}

std::vector<PacketDelta> PacketDeltaDefragmenter::GetDefragmentedPackets()
{
    return std::vector<PacketDelta>();
}


