#include "BitStream.h"
#include "gtest/gtest.h"

// Class definition!
class TestBitStream : public ::testing::Test 
{
};

TEST_F(TestBitStream, ZeroSize) 
{
  BitStream testStream(8);
  unique_ptr<vector<uint8_t>> dude;
  
  EXPECT_EQ(0, testStream.SizeInBits());
  EXPECT_EQ(0, testStream.Position());
  
  dude = testStream.TakeBuffer();
  
  // haven't added any data - so expect the size of the array to be 0.
  EXPECT_EQ(0, dude->size());
}

TEST_F(TestBitStream, AddOneBit)
{
  BitStream source(22);
  
  // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestBitStream, AddU8)
{
  // TODO!
  EXPECT_EQ(0, 1);
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