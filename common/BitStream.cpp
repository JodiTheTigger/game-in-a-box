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

using namespace std;

BitStream::BitStream(uint32_t initialCapacityInBytes)
    : BitStreamReadOnly(vector<uint8_t>())
    , myBuffer(new vector<uint8_t>())
    , myBitIndexWrite(0)
    , myCurrentBitCount(0)
{
  Reset(*myBuffer);
  myBuffer->reserve(initialCapacityInBytes);
}

BitStream::BitStream(unique_ptr<vector<uint8_t>> sourceBuffer)
    : BitStreamReadOnly(*sourceBuffer)
    , myBuffer(move(sourceBuffer))
    , myBitIndexWrite(0)
    , myCurrentBitCount(myBuffer->size() * 8)
{
}

void BitStream::Push(bool value)
{
  uint32_t byteIndex;
  uint8_t bitIndex;
  
  // new byte needed?
  if (0 == (myBitIndexWrite & 0x07))
  {
    myBuffer->push_back(0);
  }
  
  if (value)
  {    
    byteIndex = (uint32_t) (myBitIndexWrite / 8);
    bitIndex = (uint8_t) myBitIndexWrite & 0x07;
    (*myBuffer)[byteIndex] |= 1 << bitIndex;
  }
  
  ++myBitIndexWrite;
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
  
  if ((myBitIndexWrite & 0x07) == 0)
  {   
    uint32_t byteIndex;
    
    byteIndex = (uint32_t) (myBitIndexWrite / 8);
    
    myBuffer->push_back(0);
    (*myBuffer)[byteIndex] = value & ((1 << bitsToPush) - 1);
    
    myBitIndexWrite += bitsToPush;    
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

unique_ptr<vector<uint8_t>> BitStream::TakeBuffer()
{
  unique_ptr<vector<uint8_t>> result;
  
  result = move(myBuffer);
  
  // class is pretty much dead after this point.
  myBitIndexWrite = 0;
  myCurrentBitCount = 0;
  myBuffer.reset(new vector<uint8_t>());
  
  return move(result);
}


