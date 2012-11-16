#include "BitStream.h"
#include "gtest/gtest.h"

// Class definition!
class TestBitStream : public ::testing::Test 
{
};

// Tests that the Foo::Bar() method does Abc.
TEST_F(TestBitStream, ZeroSize) 
{
  BitStream f(8);
  unique_ptr<vector<uint8_t>> dude;
  
  dude = f.TakeBuffer();
  
  // havent added any data - so expect the size of the array to be 0.
  EXPECT_EQ(0, dude->size());
}