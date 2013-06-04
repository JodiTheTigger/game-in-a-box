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

#ifndef BITSTREAMREADONLY_H
#define BITSTREAMREADONLY_H

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <vector>
#endif

#include "No.hpp"

namespace GameInABox { namespace Common {

class BitStreamReadOnly : NoCopyMoveNorAssign
{
public:
    BitStreamReadOnly(const std::vector<uint8_t>& sourceBuffer);
    virtual ~BitStreamReadOnly();
    
    bool Pull1Bit();
    
    // These will only pull as many bits as the output data size.
    // e.g. for a uint8_t it will pull no more than 8 bits.
    uint8_t PullU8(uint8_t bitsToPull);
    uint16_t PullU16(uint8_t bitsToPull);
    uint32_t PullU32(uint8_t bitsToPull);    
    
    void Rewind(uint8_t bitsToRewind);
    void Reset(const std::vector<uint8_t>& newSourceBuffer);
    
    uint64_t SizeInBytes() const { return mySourceBuffer->size(); }
    uint64_t PositionReadBits() const { return myBitIndex; }

private:
    const std::vector<uint8_t>* mySourceBuffer;
    uint64_t myBitIndex;
};

}} // namespace

#endif // BITSTREAMREADONLY_H
