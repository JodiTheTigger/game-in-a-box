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
    , myCurrentBitCount(myBuffer->size())
{
}

void BitStream::Push(bool value)
{
  uint32_t byteIndex;
  uint8_t bitIndex;
  
  // new byte needed?
  if (myBitIndex & 0x07)
  {
    myBuffer->push_back(0);
  }
    
  byteIndex = myBitIndex / 8;
  bitIndex = (uint8_t) myBitIndex & 0x07;
  (*myBuffer)[byteIndex] |= 1 << bitIndex;
  ++myBitIndex;
}

void BitStream::Push(uint8_t value, uint8_t bitsToPush)
{
  if (bitsToPush > 8)
  {
    bitsToPush = 8;
  }
  
  if (myBitIndex & 0x07)
  {   
    uint32_t byteIndex;
    
    byteIndex = myBitIndex / 8;
    
    myBuffer->push_back(0);
    (*myBuffer)[byteIndex] = value;
    
    myBitIndex += bitsToPush;    
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
  Push((uint8_t) (value & 0xff), bitsToPush);
  
  if (bitsToPush > 8)
  {
    Push((uint8_t) ((value >> 8) & 0xff), bitsToPush - 8);
  }
}

void BitStream::Push(uint32_t value, uint8_t bitsToPush)
{
  Push((uint16_t) (value & 0xffff), bitsToPush);
  
  if (bitsToPush > 8)
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
    // stub TODO
    return 0;
}

uint16_t BitStream::PullU16(uint8_t bitsToPull)
{
    // stub TODO
    return 0;
}

uint32_t BitStream::PullU32(uint8_t bitsToPull)
{
    // stub TODO
    return 0;
}

unique_ptr<vector<uint8_t>> BitStream::TakeBuffer()
{
  // class is pretty much dead after this point.
  myBitIndex = 0;
  myCurrentBitCount = 0;
  
  return move(myBuffer);
}


