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

#include <Common/Network/WrappingCounter.h>
#include <gtest/gtest.h>

using namespace std;

// Class definition!
class TestSequence : public ::testing::Test
{
};

TEST_F(TestSequence, TestSimpleEqual)
{
    for (uint32_t i(0); i < (1 << 16); i++)
    {
        WrappingCounter<uint16_t> a(i);
        WrappingCounter<uint16_t> b(i);

        ASSERT_TRUE(a == b);
    }
}


TEST_F(TestSequence, TestSimpleLessThan)
{
    for (uint32_t i(0); i < (1 << 16); i++)
    {
        WrappingCounter<uint16_t> a(i);
        WrappingCounter<uint16_t> b(i+1);

        ASSERT_LT(a, b);
    }
}

TEST_F(TestSequence, TestSimpleGreaterThan)
{
    for (uint32_t i(0); i < (1 << 16); i++)
    {
        WrappingCounter<uint16_t> a(i);
        WrappingCounter<uint16_t> b(i+1);

        ASSERT_GT(b, a);
    }
}

TEST_F(TestSequence, TestSimpleGreaterThanWraparound)
{
    for (uint32_t i(0); i < (1 << 16); i++)
    {
        WrappingCounter<uint16_t> a(i);
        WrappingCounter<uint16_t> b(i+100);

        ASSERT_GT(b, a);
    }
}

TEST_F(TestSequence, TestSimpleLessThanWraparound)
{
    for (uint32_t i(0); i < (1 << 16); i++)
    {
        WrappingCounter<uint16_t> a(i);
        WrappingCounter<uint16_t> b(i+100);

        ASSERT_LT(a, b);
    }
}

TEST_F(TestSequence, TestLessThanEdgeCase)
{
    WrappingCounter<uint16_t> a(0);

    for (uint32_t i(1); i < (1 << 16); i++)
    {
        WrappingCounter<uint16_t> b(i);

        if (i <= (1 << 15))
        {
            ASSERT_LT(a, b);
        }
        else
        {
            ASSERT_LT(b, a);
        }
    }
}


TEST_F(TestSequence, TestGreaterThanEdgeCase)
{
    WrappingCounter<uint16_t> a(0);

    for (uint32_t i(1); i < (1 << 16); i++)
    {
        WrappingCounter<uint16_t> b(i);

        if (i <= (1 << 15))
        {
            ASSERT_GT(b, a);
        }
        else
        {
            ASSERT_GT(a, b);
        }
    }
}