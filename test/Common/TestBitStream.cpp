/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>
    
    This file is part of Game-in-a-box

    Game-in-a-box is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <BitStream.hpp>

#include <gtest/gtest.h>

using namespace std;

namespace GameInABox { namespace Common {

// Class definition!
class TestBitStream : public ::testing::Test 
{
};

TEST_F(TestBitStream, FromPointer) 
{
  vector<uint8_t> dude{};
  
  dude.push_back(1);
  
  BitStream result(move(dude));
  
  EXPECT_EQ(8, result.SizeInBits());
  EXPECT_EQ(0, result.PullU8(4));
  EXPECT_EQ(4, result.PositionReadBits());
}

TEST_F(TestBitStream, ZeroSize) 
{
  BitStream testStream(8);
  vector<uint8_t> dude{};
  
  EXPECT_EQ(0, testStream.SizeInBits());
  EXPECT_EQ(0, testStream.PositionReadBits());
  
  dude = testStream.TakeBuffer();
  
  // haven't added any data - so expect the size of the array to be 0.
  EXPECT_EQ(0, dude.size());
}

TEST_F(TestBitStream, AddOneBit)
{
  BitStream source(22);
  BitStream source2(22);
  
  source.Push(true);
  EXPECT_EQ(1, source.SizeInBits());
  
  BitStream result(move(source.TakeBuffer())); 
  
  EXPECT_TRUE(result.Pull1Bit());   
  
  source2.Push(false);
  source2.Push(true);
  source2.Push(true);
  EXPECT_EQ(3, source2.SizeInBits());
  
  BitStream result2(source2.TakeBuffer()); 
  
  EXPECT_FALSE(result2.Pull1Bit());
  EXPECT_TRUE(result2.Pull1Bit());
  EXPECT_TRUE(result2.Pull1Bit());
}

TEST_F(TestBitStream, AddU8)
{
  BitStream source(22);
  
  source.Push((uint8_t) 69, 8);
  EXPECT_EQ(8, source.SizeInBits());
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(69, result.PullU8(8)); 
  
  BitStream source2(22);
  
  source2.Push((uint8_t) 69, 8);
  EXPECT_EQ(8, source2.SizeInBits());
  
  BitStream result2(move(source2.TakeBuffer()));  
  
  EXPECT_EQ(4, result2.PullU8(4));
}

TEST_F(TestBitStream, AddMoreBitsThanNeeded)
{
  BitStream source(22);
  
  source.Push((uint8_t) 0xFF, 4);
  source.Push(false);
  source.Push(true);
  source.Push(false);
  EXPECT_EQ(7, source.SizeInBits());
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(0xF4, result.PullU8(8));
}

TEST_F(TestBitStream, AddU16)
{
  BitStream source(69);
  
  source.Push((uint16_t) 0x1234, 16);
  EXPECT_EQ(16, source.SizeInBits());
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(0x1234, result.PullU16(16));
}

TEST_F(TestBitStream, AddU32)
{
  BitStream source(69);
  
  source.Push((uint32_t) 0x12345678, 32);
  EXPECT_EQ(32, source.SizeInBits());
  
  BitStream result(move(source.TakeBuffer()));  
  
  EXPECT_EQ(0x12345678, result.PullU32(32));
}

TEST_F(TestBitStream, AddUnalignedU8)
{
  BitStream source(69);
  
  source.Push(true);
  source.Push((uint8_t) 133, 8);
  EXPECT_EQ(9, source.SizeInBits());
  
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
    EXPECT_EQ(60, source.SizeInBits());
    
    BitStream result(move(source.TakeBuffer()));  
  
    EXPECT_TRUE(result.Pull1Bit());
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_EQ(0x12, result.PullU8(8));
    EXPECT_EQ(0x3456, result.PullU16(16));
    EXPECT_TRUE(result.Pull1Bit());
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_EQ(0x789abcde, result.PullU32(32));
}

TEST_F(TestBitStream, TakeBuffer)
{
  BitStream source(69);
  
  source.Push((uint16_t) 0x1234, 16);
  EXPECT_EQ(16, source.SizeInBits());
  
  auto result = source.TakeBuffer();
  
  EXPECT_EQ(0x12, result[0]);
  EXPECT_EQ(0x34, result[1]);
}

TEST_F(TestBitStream, HuffmanZeroBug)
{
  BitStream source(69);
  
  source.Push((uint16_t) 0, 2);
  source.Push((uint16_t) 3, 3);
  source.Push((uint16_t) 4, 3);
  source.Push((uint16_t) 12, 4);
  EXPECT_EQ(12, source.SizeInBits());
  
  auto result = source.TakeBuffer();
  
  EXPECT_EQ(28, result[0]);
  EXPECT_EQ(0xC0, result[1]);
}

TEST_F(TestBitStream, HuffmanMissingTopBitBug)
{
  BitStream source(69);
  
  source.Push((uint16_t) 3, 3);
  source.Push((uint16_t) 1, 3);
  source.Push((uint16_t) 2, 3);
  
  source.Push((uint16_t) 2, 3);
  source.Push((uint16_t) 13, 4);
  
  source.Push((uint16_t) 0, 3);
  source.Push((uint16_t) 3, 3);
  source.Push((uint16_t) 10, 4);  
  
  source.Push((uint16_t) 2, 3);
  source.Push((uint16_t) 2, 3);
  
  EXPECT_EQ(32, source.SizeInBits());
  
  auto result = source.TakeBuffer();
  
  EXPECT_EQ(0x65, result[0]);
  EXPECT_EQ(0x2D, result[1]);
  EXPECT_EQ(0x0E, result[2]);
  EXPECT_EQ(0x92, result[3]);
}

TEST_F(TestBitStream, Huffman12BitsBug)
{
  BitStream source(69);
  
  source.Push((uint16_t) 0xFFE, 12);
  source.Push((uint16_t) 0x7FE, 11);
  source.Push((uint16_t) 0x7FE, 11);
    
  EXPECT_EQ(34, source.SizeInBits());
  
  auto result = source.TakeBuffer();
  
  EXPECT_EQ(0xFF, result[0]);
  EXPECT_EQ(0xEF, result[1]);
  EXPECT_EQ(0xFD, result[2]);
  EXPECT_EQ(0xFF, result[3]);
  EXPECT_EQ(0x80, result[4]);

}

}} // namespace
