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

    bool Push(uint8_t value, uint8_t bitsToPush);
    bool Push(uint16_t value, uint8_t bitsToPush);
    bool Push(uint32_t value, uint8_t bitsToPush);

    uint8_t PullU8(uint8_t bitsToPull);
    uint16_t PullU16(uint8_t bitsToPull);
    uint32_t PullU32(uint8_t bitsToPull);

    unique_ptr<vector<uint8_t>> TakeBuffer();

private:
    unique_ptr<vector<uint8_t>> myBuffer;
    uint64_t myBitIndex;
    uint64_t myMaxBitCount;
    uint64_t myCurrentBitCount;

};

#endif // BITSTREAM_H
