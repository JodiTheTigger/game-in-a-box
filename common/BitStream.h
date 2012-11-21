#ifndef BITSTREAM_H
#define BITSTREAM_H

#include <cstdint>
#include <vector>
#include <memory>

class BitStream
{
public:
    BitStream(uint32_t initialCapacityInBytes);
    BitStream(std::unique_ptr<std::vector<uint8_t>> sourceBuffer);

    void Push(bool value);
    
    // These will only push as many bits as the input data size.
    // e.g. for a uint8_t it will push no more than 8 bits.
    void Push(uint8_t value, uint8_t bitsToPush);
    void Push(uint16_t value, uint8_t bitsToPush);
    void Push(uint32_t value, uint8_t bitsToPush);

    bool Pull1Bit();
    
    // These will only pull as many bits as the output data size.
    // e.g. for a uint8_t it will pull no more than 8 bits.
    uint8_t PullU8(uint8_t bitsToPull);
    uint16_t PullU16(uint8_t bitsToPull);
    uint32_t PullU32(uint8_t bitsToPull);
    
    uint64_t SizeInBits() const { return myCurrentBitCount; }
    uint64_t Position() const { return myBitIndex; }

    std::unique_ptr<std::vector<uint8_t>> TakeBuffer();

private:
    // Don't allow copying, I want the compiler to tell me if I'm accidentally doing a copy.
    // If they want to copy, make it explicit by creating a new class using TakeBuffer.
    BitStream(const BitStream&);
    BitStream& operator=(const BitStream&);
    
    std::unique_ptr<std::vector<uint8_t>> myBuffer;
    uint64_t myBitIndex;
    uint64_t myCurrentBitCount;

};

#endif // BITSTREAM_H
