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


bool BitStreamReadOnly::Pull1Bit()
{
  uint32_t byteIndex;
  uint8_t bitIndex;
  uint8_t asByte;
  
  byteIndex = (uint32_t) (myBitIndex / 8);
  bitIndex = (uint8_t) myBitIndex & 0x07;
  
  asByte = mySourceBuffer[byteIndex];
  
  ++myBitIndex;
  
  if (0 == ((asByte >> bitIndex) & 0x01))
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
    result = mySourceBuffer[byteIndex];

    if (bitIndex == 0)
    {
        result = result & ((1 << bitsToPull) - 1);
    }
    else
    {
        uint16_t asU16;
        asU16 = result + (((uint16_t) mySourceBuffer[byteIndex + 1]) << 8);
        asU16 = asU16 >> bitIndex;    
        result = (uint8_t) (asU16 & ((1 << bitsToPull) - 1));
    }
    
    myBitIndex += bitsToPull;
  
    return result;
}

uint16_t BitStreamReadOnly::PullU16(uint8_t bitsToPull)
{
    uint16_t result;
    
    if (bitsToPull == 0)
    {
        return 0;
    }
    
    result = PullU8(bitsToPull);
    
    if (bitsToPull > 8)
    {
        result |= ((uint16_t) PullU8(bitsToPull - 8)) << 8;
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
    
    result = PullU16(bitsToPull);
    
    if (bitsToPull > 16)
    {
        result |= ((uint32_t) PullU16(bitsToPull - 16)) << 16;
    }
    
    return result;
}


