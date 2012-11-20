#include "BitStream.h"
#include "gtest/gtest.h"

// Class definition!
class TestBitStream : public ::testing::Test 
{
};

TEST_F(TestBitStream, FromPointer) 
{
  unique_ptr<vector<uint8_t>> dude(new vector<uint8_t>());
  
  dude->push_back(1);
  
  BitStream testStream(move(dude));
  
  EXPECT_EQ(8, testStream.SizeInBits());
  EXPECT_EQ(1, testStream.PullU8(4));
  EXPECT_EQ(4, testStream.Position());  
}

TEST_F(TestBitStream, ZeroSize) 
{
  BitStream testStream(8);
  unique_ptr<vector<uint8_t>> dude(new vector<uint8_t>());
  
  EXPECT_EQ(0, testStream.SizeInBits());
  EXPECT_EQ(0, testStream.Position());
  
  dude = testStream.TakeBuffer();
  
  // haven't added any data - so expect the size of the array to be 0.
  EXPECT_EQ(0, dude->size());
}

TEST_F(TestBitStream, AddOneBit)
{
  BitStream source(22);
  BitStream source2(22);
  
  source.Push(true);
  
  source2.Push(false);
  source2.Push(true);
  source2.Push(true);
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_TRUE(result.Pull1Bit());   
  
  BitStream result2(source2.TakeBuffer()); 
  
  EXPECT_FALSE(result2.Pull1Bit());
  EXPECT_TRUE(result2.Pull1Bit());
  EXPECT_TRUE(result2.Pull1Bit());
}

TEST_F(TestBitStream, AddU8)
{
  BitStream source(22);
  
  source.Push((uint8_t) 69, 8);
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(69, result.PullU8(8)); 
  
  BitStream source2(22);
  
  source2.Push((uint8_t) 69, 8);
  
  BitStream result2(move(source2.TakeBuffer()));  
  
  EXPECT_EQ(5, result2.PullU8(4));
}

TEST_F(TestBitStream, AddMoreBitsThanNeeded)
{
  BitStream source(22);
  
  source.Push((uint8_t) 0xFF, 4);
  source.Push(false);
  source.Push(true);
  source.Push(false);
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(47, result.PullU8(8));
}

TEST_F(TestBitStream, AddU16)
{
  BitStream source(69);
  
  source.Push((uint16_t) 0x1234, 16);
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(0x1234, result.PullU16(16));
}

TEST_F(TestBitStream, AddU32)
{
  BitStream source(69);
  
  source.Push((uint32_t) 0x12345678, 32);
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(0x12345678, result.PullU32(32));
}

TEST_F(TestBitStream, AddUnalignedU8)
{
  BitStream source(69);
  
  source.Push(true);
  source.Push((uint8_t) 133, 8);
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_TRUE(result.Pull1Bit());
  EXPECT_EQ(133, result.PullU8(8));
}

TEST_F(TestBitStream, AddLotsOfStuff)
{
    BitStream source(16);

    source.Push(true);  
    source.Push(false);
    source.Push((uint8_t) 0x12, 8);
    source.Push((uint16_t) 0x3456, 16);
    source.Push(true);  
    source.Push(false);
    source.Push((uint32_t) 0x789abcde, 32);
    
    BitStream result(move(source.TakeBuffer()));  
  
    EXPECT_TRUE(result.Pull1Bit());
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_EQ(0x12, result.PullU8(8));
    EXPECT_EQ(0x3456, result.PullU16(16));
    EXPECT_TRUE(result.Pull1Bit());
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_EQ(0x789abcde, result.PullU32(32));
}