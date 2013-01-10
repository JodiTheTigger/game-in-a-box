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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <vector>
#include <memory>

#include "BuildMacros.h"
#include "BitStreamReadOnly.h"

class BitStream : public BitStreamReadOnly
{
    CLASS_NOCOPY_NOASSIGN(BitStream)
    
public:
    BitStream(uint32_t initialCapacityInBytes);
    BitStream(std::unique_ptr<std::vector<uint8_t>> sourceBuffer);

    void Push(bool value);
    
    // These will only push as many bits as the input data size.
    // e.g. for a uint8_t it will push no more than 8 bits.
    void Push(uint8_t value, uint8_t bitsToPush);
    void Push(uint16_t value, uint8_t bitsToPush);
    void Push(uint32_t value, uint8_t bitsToPush);

    uint64_t SizeInBits() const { return myCurrentBitCount; }
    uint64_t PositionWrite() const { return myBitIndexWrite; }

    std::unique_ptr<std::vector<uint8_t>> TakeBuffer();

private:
    std::unique_ptr<std::vector<uint8_t>> myBuffer;
    uint64_t myBitIndexWrite;
    uint64_t myCurrentBitCount;

};

#endif // BITSTREAM_H
