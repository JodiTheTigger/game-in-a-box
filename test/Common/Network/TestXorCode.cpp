/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012-2013 Richard Maxwell <jodi.the.tigger@gmail.com>
    
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
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <Common/Network/XorCode.h>
#include <gmock/gmock.h>

using namespace std;
using Bytes = std::vector<uint8_t>;

// Class definition!
class TestXorCode : public ::testing::Test
{

};

TEST_F(TestXorCode, EmptyBufferEmptyKey)
{
    Bytes empty;

    XorCode(empty.begin(), empty.end(), empty);

    // no execptions expected;
    EXPECT_TRUE(empty.empty());
}

TEST_F(TestXorCode, EmptyBuffer)
{
    Bytes empty;
    Bytes codes(4,42);

    XorCode(empty.begin(), empty.end(), codes);

    // no execptions expected;
    EXPECT_TRUE(empty.empty());
}

TEST_F(TestXorCode, FullCode)
{
    Bytes toTest(8,11);
    Bytes codes(4,42);

    XorCode(begin(toTest), end(toTest), codes);

    EXPECT_NE(toTest, Bytes(8,11));
}

TEST_F(TestXorCode, FullCodeDecode)
{
    Bytes toTest(8,11);
    Bytes codes(4,42);

    XorCode(begin(toTest), end(toTest), codes);
    XorCode(begin(toTest), end(toTest), codes);

    EXPECT_EQ(toTest, Bytes(8,11));
}

TEST_F(TestXorCode, PartialCode)
{
    Bytes toTest(8,11);
    Bytes codes(4,42);

    XorCode(begin(toTest) + 2, end(toTest)-2, codes);

    EXPECT_NE(toTest, Bytes(8,11));
}

TEST_F(TestXorCode, PartialCodeDecode)
{
    Bytes toTest(8,11);
    Bytes codes(4,42);

    XorCode(begin(toTest)+2, end(toTest)-2, codes);
    XorCode(begin(toTest)+2, end(toTest)-2, codes);

    EXPECT_EQ(toTest, Bytes(8,11));
}

TEST_F(TestXorCode, PartialCodeDecode32000)
{
    Bytes toTest(32000,11);
    Bytes codes(16,42);

    XorCode(begin(toTest)+222, end(toTest)-222, codes);
    XorCode(begin(toTest)+222, end(toTest)-222, codes);

    EXPECT_EQ(toTest, Bytes(32000,11));
}