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

#include "BitStreamReadOnly.h"
#include "IGameStateObject.h"

// Assume the object is needed.
bool DeltaGameStateWrite(const IGameStateObject& base, IGameStateObject& result, BitStreamReadOnly& data) const
{
    // Bits are:
    // Object member unchanged (1 == yes)
    // Object member is 0.0 (1 == yes) : RESEARCH!
    // Object member data (if both previous are no)

    if (data.Pull1Bit())
    {
        //result = base;
    }
    else
    {
        if (data.Pull1Bit())
        {
            //result = 0.0f or int
        }
        else
        {
            // Read / Write as a uint32_t to avoid endianess issues?
            // Do more research!
            // value = data.PullU32(bitsToRead)
            // XOR value with base to get actual value  : RESEARCH!
        }
    }
}

bool DeltaGameStateRead(const IGameStateObject& base, const IGameStateObject& toDelta, BitStream& data)
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