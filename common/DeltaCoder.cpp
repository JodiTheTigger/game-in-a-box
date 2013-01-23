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

#include "DeltaCoder.h"

using namespace std;

DeltaCoder::DeltaCoder(std::vector<DeltaMapItem> deltaMap) : myDeltaMap(deltaMap)
{
    // Nothing.
}


bool DeltaCoder::DeltaDecodeItem(
    const IGameStateObject& base,
     IGameStateObject& result, 
     BitStreamReadOnly& dataIn) const
{
    for (DeltaMapItem& map : myDeltaMap)
    {
        const uint32_t* in;
        uint32_t* out;

        in  = &(((const uint8_t*)   (&base))[map.byteOffset]);
        out = &(((uint8_t*)         (&result))[map.byteOffset]);

        // Bits are:
        // Object member unchanged (1 == yes)
        // Object member is 0.0 (1 == yes) : RESEARCH!
        // Object member data (if both previous are no)

        if (data.Pull1Bit())
        {
            *out = *in;
        }
        else
        {
            if (data.Pull1Bit())
            {
                *out = 0;
            }
            else
            {
                // Read / Write as a uint32_t to avoid endianess issues?
                // Do more research!
                // value = data.PullU32(bitsToRead)
                // XOR value with base to get actual value  : RESEARCH!
                *out = *in ^ data.PullU32(map.numberOfBits); 
            }
        }
    }
}

bool DeltaCoder::DeltaEncodeItem(
    const IGameStateObject& base, 
    const IGameStateObject& toDelta, 
    BitStream& dataOut) const
{
    // for each item in state data
    {
        // if (base==toDelta)
        {
            data.Push1Bit(true);
        }
        else
        {
            data.Push1Bit(false);

            // if (toDelta = 0)  : RESEARCH!
            {
                data.Push1Bit(true);
            }
            else
            {
                data.Push1Bit(false);

                // XOR with base to get value to write  : RESEARCH!
            }
        }
    }
}
