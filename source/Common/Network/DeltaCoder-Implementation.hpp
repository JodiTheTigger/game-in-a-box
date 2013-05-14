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
#include <stdexcept>
#endif

#include "Common/BitStream.hpp"

// NOTE! This is not a normal cpp file. don't use using!
namespace GameInABox { namespace Common { namespace Network {

template<class OBJECT>
DeltaCoder<OBJECT>::DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        OBJECT identity,
        bool researchEncodeZeros,
        bool researchEncodeXorDeltas)
    : myDeltaMap(deltaMap)
    , myIdentityObject(identity)
    , myResearchEncodeZeros(researchEncodeZeros)
    , myResearchEncodeXorDeltas(researchEncodeXorDeltas)
{
    // Make sure the delta map is valid by checking if it runs out
    // of bounds of the identity structure.
    for (const DeltaMapItem& map : myDeltaMap)
    {
        if ((map.byteOffset + 4) > sizeof(OBJECT))
        {
            throw std::logic_error("DeltaMapItem is reading data past the end of identity. Buffer overrun.");
        }
    }
}

template<class OBJECT>
void DeltaCoder<OBJECT>::DeltaDecode(
    const OBJECT* base,
    OBJECT& result, 
    BitStreamReadOnly& dataIn) const
{    
    const OBJECT* basePointer;
    
    if (base == nullptr)
    {
        basePointer = &myIdentityObject;
    }
    else
    {
        basePointer = base;
    }
    
    for (const DeltaMapItem& map : myDeltaMap)
    {
        const uint32_t* in;
        uint32_t* out;
        
        // Too many braces otherwise, this isn't lisp.
        {
            const uint8_t* bytesBase;
            uint8_t* bytesDelta;
            
            bytesBase   = (const uint8_t*) basePointer;
            bytesDelta  = (uint8_t*) (&result);
                
            in  = (const uint32_t *) &(bytesBase[map.byteOffset]);
            out = (uint32_t *) &(bytesDelta[map.byteOffset]);
        }

        // Bits are:
        // Object member unchanged (1 == yes)
        // Object member is 0.0 (1 == yes) : RESEARCH!
        // Object member data (if both previous are no)

        if (dataIn.Pull1Bit())
        {
            *out = *in;
        }
        else
        {
            bool isZero;

            isZero = false;

            if (myResearchEncodeZeros)
            {
                isZero = dataIn.Pull1Bit();
            }

            if (isZero)
            {
                *out = 0;
            }
            else
            {
                // Read / Write as a uint32_t to avoid endianess issues?
                // value = data.PullU32(bitsToRead)
                // XOR value with base to get actual value  : RESEARCH!
                if (myResearchEncodeXorDeltas)
                {
                    *out = *in ^ dataIn.PullU32(map.numberOfBits); 
                }
                else
                {
                    *out = dataIn.PullU32(map.numberOfBits);
                }
            }            
        }
    }
}
template<class OBJECT>
bool DeltaCoder<OBJECT>::DeltaEncode(
    const OBJECT* base, 
    const OBJECT& toDelta, 
    BitStream& dataOut) const
{
    bool noChange;
    const OBJECT* basePointer;
    
    if (base == nullptr)
    {
        basePointer = &myIdentityObject;
    }
    else
    {
        basePointer = base;
    }
    
    noChange = true;
    
    for (const DeltaMapItem& map : myDeltaMap)
    {
        const uint32_t* itemBase;
        const uint32_t* itemDelta;

        // Too many braces otherwise, this isn't lisp.
        {
            const uint8_t* bytesBase;
            const uint8_t* bytesDelta;
            
            bytesBase   = (const uint8_t*) basePointer;
            bytesDelta  = (const uint8_t*) (&toDelta);
                
            itemBase  = (const uint32_t *) &(bytesBase[map.byteOffset]);
            itemDelta = (const uint32_t *) &(bytesDelta[map.byteOffset]);
        }

        if (*itemBase == *itemDelta)
        {
            dataOut.Push(true);
        }
        else
        {
            dataOut.Push(false);

            noChange = false;
            
            if (myResearchEncodeZeros)
            {
                if (*itemDelta == 0)
                {
                    dataOut.Push(true);
                }
                else
                {
                    dataOut.Push(false);
                    
                    if (myResearchEncodeXorDeltas)
                    {
                        dataOut.Push(*itemBase ^ *itemDelta, map.numberOfBits); 
                    }
                    else
                    {
                        dataOut.Push(*itemDelta, map.numberOfBits);
                    }
                }
            }
            else
            {                
                if (myResearchEncodeXorDeltas)
                {
                    dataOut.Push(*itemBase ^ *itemDelta, map.numberOfBits); 
                }
                else
                {
                    dataOut.Push(*itemDelta, map.numberOfBits);
                }
            }
        }
    }
    
    return noChange;
}

}}} // namespace
