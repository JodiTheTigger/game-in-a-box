#include "BitStream.h"

BitStream::BitStream(uint32_t maximumSizeOfBitStreamInBytes)
    : myBuffer(new vector<uint8_t>())
    , myBitIndex(0)
    , myMaxBitCount(maximumSizeOfBitStreamInBytes * 8)
    , myCurrentBitCount(0)
{
  myBuffer->reserve(maximumSizeOfBitStreamInBytes);
}

BitStream::BitStream(unique_ptr<vector<uint8_t>> sourceBuffer)
    : myBuffer(move(sourceBuffer))
    , myBitIndex(0)
    , myMaxBitCount(myBuffer->size())
    , myCurrentBitCount(myBuffer->size())
{
}

bool BitStream::Push(uint8_t value, uint8_t bitsToPush)
{
    // stub
    // TODO!
    return false;
}


bool BitStream::Push(uint16_t value, uint8_t bitsToPush)
{
    // stub
    // TODO!
    return false;
}

bool BitStream::Push(uint32_t value, uint8_t bitsToPush)
{
    // stub
    // TODO!
    return false;
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
  myMaxBitCount = 0;
  myCurrentBitCount = 0;
  
  return move(myBuffer);
}


