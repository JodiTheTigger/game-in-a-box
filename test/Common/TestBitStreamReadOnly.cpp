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

#include <gtest/gtest.h>
#include <Common/BitStreamReadOnly.h>

using namespace std;

// Class definition!
class TestBitStreamReadOnly : public ::testing::Test 
{
};

TEST_F(TestBitStreamReadOnly, Simple) 
{
  vector<uint8_t> dude;
  
  dude.push_back(1);
  
  BitStreamReadOnly result(dude);
  
  EXPECT_EQ(1, result.SizeInBytes());
  EXPECT_EQ(0, result.PullU8(4));
  EXPECT_EQ(4, result.PositionReadBits());  
  EXPECT_EQ(1, result.PullU8(4));
  EXPECT_EQ(8, result.PositionReadBits());  
}

TEST_F(TestBitStreamReadOnly, ZeroSize) 
{
  vector<uint8_t> dude;
  
  BitStreamReadOnly testStream(dude);
  
  EXPECT_EQ(0, testStream.SizeInBytes());
  EXPECT_EQ(0, testStream.PositionReadBits());
}

TEST_F(TestBitStreamReadOnly, ReadPastEnd)
{
  vector<uint8_t> dude;
  
  dude.push_back(1);
  
  BitStreamReadOnly result(dude);

  EXPECT_EQ(0, result.PullU8(4));
  EXPECT_EQ(4, result.PositionReadBits());  
  
  EXPECT_EQ(16, result.PullU8(8));
  EXPECT_EQ(8, result.PositionReadBits());  

  EXPECT_EQ(0, result.PullU8(8));
  EXPECT_EQ(8, result.PositionReadBits());  
}

TEST_F(TestBitStreamReadOnly, Reset)
{ 
    vector<uint8_t> dude1;
    vector<uint8_t> dude2;
  
  dude1.push_back(1);
  dude2.push_back(2);
  
  BitStreamReadOnly result(dude1);

  EXPECT_EQ(0, result.PullU8(4));
  EXPECT_EQ(4, result.PositionReadBits());  
  
  EXPECT_EQ(16, result.PullU8(8));
  EXPECT_EQ(8, result.PositionReadBits());  

  result.Reset(dude2);
  
  EXPECT_EQ(0, result.PullU8(4));
  EXPECT_EQ(4, result.PositionReadBits());  
  
  EXPECT_EQ(32, result.PullU8(8));
  EXPECT_EQ(8, result.PositionReadBits()); 
}

TEST_F(TestBitStreamReadOnly, ArrayUpdatedBehindBack)
{
  vector<uint8_t> dude;
  
  dude.push_back(1);
  
  BitStreamReadOnly result(dude);

  EXPECT_EQ(1, result.PullU8(8));
  EXPECT_EQ(0, result.PullU8(8));
  EXPECT_EQ(8, result.PositionReadBits());  
  
  dude.push_back(1);

  EXPECT_EQ(1, result.PullU8(8));
  EXPECT_EQ(0, result.PullU8(8));
  EXPECT_EQ(16, result.PositionReadBits());  

  dude.clear();
  
  EXPECT_EQ(0, result.PullU8(8));
  EXPECT_EQ(0, result.PullU8(8));
  EXPECT_EQ(0, result.PositionReadBits());  

  result.Rewind(100);
  
  EXPECT_EQ(0, result.PullU8(8));
  EXPECT_EQ(0, result.PullU8(8));
  EXPECT_EQ(0, result.PositionReadBits());  
}

TEST_F(TestBitStreamReadOnly, Rewind)
{
  vector<uint8_t> dude;
  
  dude.push_back(1);
  dude.push_back(1);
  
  BitStreamReadOnly result(dude);

  EXPECT_EQ(0, result.PullU8(4));
  EXPECT_EQ(4, result.PositionReadBits());  
  
  result.Rewind(4);
    
  EXPECT_EQ(1, result.PullU8(8));
  EXPECT_EQ(8, result.PositionReadBits());  
  
  result.Rewind(0);
  
  EXPECT_EQ(1, result.PullU8(8));
  EXPECT_EQ(16, result.PositionReadBits());  
  
  result.Rewind(15);
  
  EXPECT_EQ(2, result.PullU8(8));
  EXPECT_EQ(9, result.PositionReadBits());  
  
  result.Rewind(100);
  
  EXPECT_EQ(2, result.PullU16(9));
  EXPECT_EQ(9, result.PositionReadBits());  
}

TEST_F(TestBitStreamReadOnly, TestLotsOfStuff)
{
    vector<uint8_t> dude;
    
    BitStreamReadOnly result(dude);
    
    dude.push_back(1);
    dude.push_back(2);
    dude.push_back(4);
    dude.push_back(8);
    dude.push_back(16);
    
    EXPECT_EQ(5, result.SizeInBytes());
    
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_EQ(4, result.PullU8(8));
    EXPECT_EQ(0x810, result.PullU16(16));
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_FALSE(result.Pull1Bit());
    EXPECT_EQ(0x81000000, result.PullU32(32));
    
    result.Rewind(100);
    EXPECT_EQ(0x01020408, result.PullU32(32));
    
    result.Rewind(31);
    EXPECT_EQ(0x02040810, result.PullU32(32));    
}

TEST_F(TestBitStreamReadOnly, TestHuffmanRead7Bug)
{    
    vector<uint8_t> dude;
    
    BitStreamReadOnly result(dude);
    
    dude.push_back(0xFF);
    dude.push_back(0xEF);
    dude.push_back(0xFD);
    dude.push_back(0xFF);
    dude.push_back(0x80);
    
    EXPECT_EQ(0x01FF, result.PullU16(9));
    EXPECT_EQ(0x6F, result.PullU8(7));    
}

TEST_F(TestBitStreamReadOnly, TestReadLessThanAllBits)
{    
    vector<uint8_t> dude;
    
    BitStreamReadOnly result(dude);
    
    dude.push_back(0x1);
    dude.push_back(0x2);
    dude.push_back(0x3);
    dude.push_back(0x4);
    
    EXPECT_EQ(0x01, result.PullU16(8));
    EXPECT_EQ(0x0203, result.PullU32(16));
    EXPECT_EQ(0x04, result.PullU32(8));    
}
