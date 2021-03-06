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

#ifndef USING_PRECOMPILED_HEADERS
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "BitStream.hpp"

using namespace std;
using namespace GameInABox::Network::Implementation;

BitStream::BitStream(uint32_t initialCapacityInBytes)
    : BitStreamReadOnly({})
    , myBuffer()
    , myBitIndexWrite(0)
    , myCurrentBitCount(0)
{
    Reset(myBuffer);
    myBuffer.reserve(initialCapacityInBytes);
}

BitStream::BitStream(std::vector<uint8_t> sourceBuffer)
    : BitStreamReadOnly({})
    , myBuffer(move(sourceBuffer))
    , myBitIndexWrite(0)
    , myCurrentBitCount(myBuffer.size() * 8)
{    
    Reset(myBuffer);
}

void BitStream::Push(bool value)
{
    // new byte needed?
    if (0 == (myBitIndexWrite & 0x07))
    {
        myBuffer.push_back(0);
    }

    if (value)
    {
        uint32_t byteIndex;
        uint8_t bitIndex;

        byteIndex = (uint32_t) (myBitIndexWrite / 8);
        bitIndex = (uint8_t) myBitIndexWrite & 0x07;

        myBuffer[byteIndex] |= 0x80 >> bitIndex;
    }

    ++myBitIndexWrite;
    ++myCurrentBitCount;
}

void BitStream::Push(uint8_t value, uint8_t bitsToPush)
{
    uint32_t byteIndex;
    uint8_t bitIndex;
    uint8_t topAligned;


    if (bitsToPush > 8)
    {
        bitsToPush = 8;
    }

    if (bitsToPush == 0)
    {
        return;
    }

    byteIndex = (uint32_t) (myBitIndexWrite / 8);
    bitIndex = (uint8_t) myBitIndexWrite & 0x07;

    value &= (1 << bitsToPush) - 1;
    topAligned = (value << (8 - bitsToPush));

    if (bitIndex == 0)
    {
        myBuffer.push_back(0);
        myBuffer[byteIndex] = topAligned;
    }
    else
    {
        myBuffer[byteIndex] |= topAligned >> bitIndex;

        if (bitsToPush > (8 - bitIndex))
        {
            myBuffer.push_back(0);
            myBuffer[byteIndex + 1] |= topAligned << (8 - bitIndex);
        }
    }

    myBitIndexWrite += bitsToPush;
    myCurrentBitCount += bitsToPush;
}

void BitStream::Push(uint16_t value, uint8_t bitsToPush)
{
    if (bitsToPush == 0)
    {
        return;
    }

    if (bitsToPush > 8)
    {
        Push((uint8_t) (value >> (bitsToPush - 8)), 8);
        Push((uint8_t) value, bitsToPush - 8);
    }
    else
    {
        Push((uint8_t) value, bitsToPush);
    }
}

void BitStream::Push(uint32_t value, uint8_t bitsToPush)
{
    if (bitsToPush == 0)
    {
        return;
    }


    if (bitsToPush > 16)
    {
        Push((uint16_t) (value >> (bitsToPush - 16)), 16);
        Push((uint16_t) value, bitsToPush - 16);
    }
    else
    {
        Push((uint16_t) value, bitsToPush);
    }
}

vector<uint8_t> BitStream::TakeBuffer()
{
    vector<uint8_t> result{move(myBuffer)};

    // class is pretty much dead after this point.
    myBitIndexWrite = 0;
    myCurrentBitCount = 0;
    Reset(myBuffer);

    return move(result);
}
