#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <vector>
#include <memory>

using namespace std;

class BitStream
{
public:
    BitStream(uint32_t maximumSizeOfBitStreamInBytes);
    BitStream(unique_ptr<vector<uint8_t>> sourceBuffer);

    bool Push(bool value);
    bool Push(uint8_t value, uint8_t bitsToPush);
    bool Push(uint16_t value, uint8_t bitsToPush);
    bool Push(uint32_t value, uint8_t bitsToPush);

    bool Pull1Bit();
    uint8_t PullU8(uint8_t bitsToPull);
    uint16_t PullU16(uint8_t bitsToPull);
    uint32_t PullU32(uint8_t bitsToPull);
    
    uint32_t SizeInBits() const { return myCurrentBitCount; }
    uint32_t Position() const { return myBitIndex; }

    unique_ptr<vector<uint8_t>> TakeBuffer();

private:
    // Don't allow copying, I want the compiler to tell me if I'm accidentally doing a copy.
    // If they want to copy, make it explicit by creating a new class using TakeBuffer.
    BitStream(const BitStream&);
    BitStream& operator=(const BitStream&);
    
    unique_ptr<vector<uint8_t>> myBuffer;
    uint64_t myBitIndex;
    uint64_t myMaxBitCount;
    uint64_t myCurrentBitCount;

};

#endif // BITSTREAM_H
