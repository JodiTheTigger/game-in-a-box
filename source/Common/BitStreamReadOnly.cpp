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

using namespace GameInABox::Common;

BitStreamReadOnly::BitStreamReadOnly(const std::vector< uint8_t >& sourceBuffer)
    : mySourceBuffer(&sourceBuffer)
    , myBitIndex(0) 
{
}

BitStreamReadOnly::~BitStreamReadOnly()
{
}

void BitStreamReadOnly::Reset(const std::vector<uint8_t>& newSourceBuffer)
{
    // Only do the same as the constructor
    mySourceBuffer = &newSourceBuffer;
    myBitIndex = 0;
}

bool BitStreamReadOnly::Pull1Bit()
{    
  uint32_t byteIndex;
  uint8_t bitIndex;
  uint8_t asByte;
  
  // I hate writing (*ptr)[index]
  auto& sourceBuffer = *mySourceBuffer;
  
  byteIndex = (uint32_t) (myBitIndex / 8);
  bitIndex = (uint8_t) myBitIndex & 0x07;
    
    if (byteIndex >= sourceBuffer.size())
    {
        if (myBitIndex > sourceBuffer.size() * 8)
        {
            myBitIndex = sourceBuffer.size() * 8;
        }
        
        return false;
    }
  
  asByte = sourceBuffer[byteIndex];
  
  ++myBitIndex;
  
  if (0 == (asByte & (0x80 >> bitIndex)))
  {    
    return false;
  }
  else
  {
    return true;
  }  
}

uint8_t BitStreamReadOnly::PullU8(uint8_t bitsToPull)
{
    uint32_t byteIndex;
    uint8_t bitIndex;
    uint8_t result;
    
    auto& sourceBuffer = *mySourceBuffer;
 
    if (bitsToPull > 8)
    {
        bitsToPull = 8;
    }
    
    if (bitsToPull == 0)
    {
        return 0;
    }
    
    byteIndex = (uint32_t) (myBitIndex / 8);
    bitIndex = (uint8_t) myBitIndex & 0x07;
    
    if (byteIndex >= sourceBuffer.size())
    {
        if (myBitIndex > sourceBuffer.size() * 8)
        {
            myBitIndex = sourceBuffer.size() * 8;
        }
        
        return 0;
    }
    
    result = sourceBuffer[byteIndex];

    if (bitIndex == 0)
    {
        result = result >> (8 - bitsToPull);
    }
    else
    {
        uint16_t second;
        uint16_t asU16;
        
        if ((byteIndex+1) >= sourceBuffer.size())
        {
            second = 0;
        }
        else
        {
            second = sourceBuffer[byteIndex + 1];
        }
        
        asU16 = (((uint16_t) result) << 8) + second;
        asU16 = asU16 >> (8 - bitIndex + (8-bitsToPull));    
        result = (uint8_t) asU16 & ((1 << bitsToPull) - 1);
    }
    
    myBitIndex += bitsToPull;
    
    // EOF?
    if ((myBitIndex / 8) >= sourceBuffer.size())
    {
        myBitIndex = sourceBuffer.size() * 8;
    }
  
    return result;
}

uint16_t BitStreamReadOnly::PullU16(uint8_t bitsToPull)
{
    uint16_t result;
    
    if (bitsToPull == 0)
    {
        return 0;
    }
    
    if (bitsToPull > 16)
    {
        bitsToPull = 16;
    }
    
    result = PullU8(bitsToPull);
    
    if (bitsToPull > 8)
    {
        result <<= (bitsToPull - 8);
        result |= PullU8(bitsToPull - 8);
    }
    
    return result;
}

uint32_t BitStreamReadOnly::PullU32(uint8_t bitsToPull)
{
    uint32_t result;
    
    if (bitsToPull == 0)
    {
        return 0;
    }
    
    if (bitsToPull > 32)
    {
        bitsToPull = 32;
    }
    
    result = PullU16(bitsToPull);
    
    if (bitsToPull > 16)
    {
        result <<= (bitsToPull - 16);
        result |= PullU16(bitsToPull - 16);
    }
    
    return result;
}

void BitStreamReadOnly::Rewind(uint8_t bitsToRewind)
{
    if (bitsToRewind > myBitIndex)
    {
        myBitIndex = 0;
    }
    else
    {
        myBitIndex -= bitsToRewind;
    }
}



