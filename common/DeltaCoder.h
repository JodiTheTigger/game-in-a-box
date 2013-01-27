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
/*
    NOTES:
    ======
    This is basically a C function working on raw byte arrays
    I don't want to work with classes as down low this should 
    be working with continuous areas of memory. Classes and polymorphism
    can ruin all that. The down side is lack of type checking and array overrun protection.
    "identitySizeInBytes" is my attempt at basic array bounds checking at object creation.
*/

#ifndef IDELTACODER_H
#define IDELTACODER_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

// Argh Includes
#include "DeltaMapItem.h"

// forward declaration
class BitStreamReadOnly;
class BitStream;

class DeltaCoder
{
public:
    DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        std::unique_ptr<uint8_t[]> identity,
        size_t identitySizeInBytes,
        bool researchEncodeZeros,
        bool researchEncodeXorDeltas);
    
    void DeltaDecode(
        const uint8_t* base,
        uint8_t* result, 
        BitStreamReadOnly& dataIn) const;

    // Returns true if base == toDelta, otherwise false.
    bool DeltaEncode(
        const uint8_t* base, 
        const uint8_t* toDelta,
        BitStream& dataOut) const;
        
private:
    const std::vector<DeltaMapItem> myDeltaMap;    
    const std::unique_ptr<uint8_t[]> myIdentityObject;
    const bool myResearchEncodeZeros;
    const bool myResearchEncodeXorDeltas;
};

#endif // IDELTACODER_H