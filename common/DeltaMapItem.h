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

#ifndef DELTAMAPITEM_H
#define DELTAMAPITEM_H

#include <cstdint>
#include <string>

class DeltaMapItem
{
public:
    DeltaMapItem(std::string newName, uint32_t newByteOffset, uint8_t newNumberOfBits)
    : name(newName)
    , byteOffset(newByteOffset)
    , numberOfBits(newNumberOfBits)
    {
    }

    const std::string name;
    const uint32_t byteOffset;
    const uint8_t numberOfBits;
};

// ///////////////////
// Delta Map Item Macros
// ///////////////////
#define DELTAMAP(CLASS_TYPE, CLASS_MEMBER, NUMBER_OFBITS) #CLASS_MEMBER,(uint32_t)&((CLASS_TYPE*)0)->CLASS_MEMBER, NUMBER_OFBITS

#endif // DELTAMAPITEM_H