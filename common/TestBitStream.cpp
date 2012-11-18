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
  
  // havent added any data - so expect the size of the array to be 0.
  EXPECT_EQ(0, dude->size());
}