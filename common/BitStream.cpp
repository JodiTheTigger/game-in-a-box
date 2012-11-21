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

#include "BitStream.h"

BitStream::BitStream(uint32_t initialCapacityInBytes)
    : myBuffer(new vector<uint8_t>())
    , myBitIndex(0)
    , myCurrentBitCount(0)
{
  myBuffer->reserve(initialCapacityInBytes);
}

BitStream::BitStream(unique_ptr<vector<uint8_t>> sourceBuffer)
    : myBuffer(move(sourceBuffer))
    , myBitIndex(0)
    , myCurrentBitCount(myBuffer->size() * 8)
{
}

void BitStream::Push(bool value)
{
  uint32_t byteIndex;
  uint8_t bitIndex;
  
  // new byte needed?
  if (0 == (myBitIndex & 0x07))
  {
    myBuffer->push_back(0);
  }
  
  if (value)
  {    
    byteIndex = myBitIndex / 8;
    bitIndex = (uint8_t) myBitIndex & 0x07;
    (*myBuffer)[byteIndex] |= 1 << bitIndex;
  }
  
  ++myBitIndex;
  ++myCurrentBitCount;
}

void BitStream::Push(uint8_t value, uint8_t bitsToPush)
{
  if (bitsToPush > 8)
  {
    bitsToPush = 8;
  }
  
  if (bitsToPush == 0)
  {
      return;
  }
  
  if ((myBitIndex & 0x07) == 0)
  {   
    uint32_t byteIndex;
    
    byteIndex = myBitIndex / 8;
    
    myBuffer->push_back(0);
    (*myBuffer)[byteIndex] = value & ((1 << bitsToPush) - 1);
    
    myBitIndex += bitsToPush;    
    myCurrentBitCount += bitsToPush;
  }
  else
  {
    for (int i = 0; i < bitsToPush; i++)
    {
      if (0 == ((value >> i) & 0x01))
      {
	Push(false);
      }
      else
      {
	Push(true);
      }      
    }
  }
}


void BitStream::Push(uint16_t value, uint8_t bitsToPush)
{  
  if (bitsToPush == 0)
  {
      return;
  }
  
  Push((uint8_t) (value & 0xff), bitsToPush);
  
  if (bitsToPush > 8)
  {
    Push((uint8_t) ((value >> 8) & 0xff), bitsToPush - 8);
  }
}

void BitStream::Push(uint32_t value, uint8_t bitsToPush)
{
  if (bitsToPush == 0)
  {
      return;
  }
  
  Push((uint16_t) (value & 0xffff), bitsToPush);
  
  if (bitsToPush > 16)
  {
    Push((uint16_t) ((value >> 16) & 0xffff), bitsToPush - 16);
  }
}

bool BitStream::Pull1Bit()
{
  uint32_t byteIndex;
  uint8_t bitIndex;
  uint8_t asByte;
  
  byteIndex = myBitIndex / 8;
  bitIndex = (uint8_t) myBitIndex & 0x07;
  
  asByte = (*myBuffer)[byteIndex];
  
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

uint8_t BitStream::PullU8(uint8_t bitsToPull)
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
    
    byteIndex = myBitIndex / 8;
    bitIndex = (uint8_t) myBitIndex & 0x07;
    result = (*myBuffer)[byteIndex];

    if (bitIndex == 0)
    {
        result = result & ((1 << bitsToPull) - 1);
    }
    else
    {
        uint16_t asU16;
        asU16 = result + (((uint16_t) (*myBuffer)[byteIndex + 1]) << 8);
        asU16 = asU16 >> bitIndex;    
        result = (uint8_t) (asU16 & ((1 << bitsToPull) - 1));
    }
    
    myBitIndex += bitsToPull;
  
    return result;
}

uint16_t BitStream::PullU16(uint8_t bitsToPull)
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

uint32_t BitStream::PullU32(uint8_t bitsToPull)
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

unique_ptr<vector<uint8_t>> BitStream::TakeBuffer()
{
  unique_ptr<vector<uint8_t>> result;
  
  result = move(myBuffer);
  
  // class is pretty much dead after this point.
  myBitIndex = 0;
  myCurrentBitCount = 0;
  myBuffer.reset(new vector<uint8_t>());
  
  return move(result);
}


