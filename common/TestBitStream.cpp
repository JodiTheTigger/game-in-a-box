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
}

TEST_F(TestBitStream, AddU16)
{
  // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestBitStream, AddU32)
{
  // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestBitStream, AddLotsOfStuff)
{
  // TODO!
  EXPECT_EQ(0, 1);
}