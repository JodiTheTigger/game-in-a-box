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

#include <Common/Network/Sequence.h>
#include <gtest/gtest.h>

using namespace std;

// Class definition!
class TestSequence : public ::testing::Test
{
};

TEST_F(TestSequence, TestSimpleEqual)
{
    for (uint32_t i(0); i < ((2^16)-1); i++)
    {
        Sequence a(i);
        Sequence b(i);

        ASSERT_TRUE(a == b);
    }
}


TEST_F(TestSequence, TestSimpleGreaterThan)
{
    for (uint32_t i(0); i < ((2^16)-1); i++)
    {
        Sequence a(i);
        Sequence b(i+1);

        ASSERT_TRUE(a < b);
    }
}

TEST_F(TestSequence, TestSimpleLessThan)
{
    for (uint32_t i(0); i < ((2^16)-1); i++)
    {
        Sequence a(i);
        Sequence b(i+1);

        ASSERT_TRUE(b < a);
    }
}

TEST_F(TestSequence, TestSimpleGreaterThanWraparound)
{
    for (uint32_t i(0); i < ((2^16)-1); i++)
    {
        Sequence a(i);
        Sequence b((i+100) % (2^16));

        ASSERT_TRUE(a < b);
    }
}

TEST_F(TestSequence, TestSimpleLessThanWraparound)
{
    for (uint32_t i(0); i < ((2^16)-1); i++)
    {
        Sequence a(i);
        Sequence b((i+100) % (2^16));

        ASSERT_TRUE(b < a);
    }
}

TEST_F(TestSequence, TestLessThanEdgeCase)
{
    Sequence a(0);

    for (uint32_t i(1); i < ((2^16)-1); i++)
    {
        Sequence b(i % (2^16));

        if (i < (2^15))
        {
            ASSERT_TRUE(a < b);
        }
        else
        {
            ASSERT_TRUE(b < a);
        }
    }
}


TEST_F(TestSequence, TestGreaterThanEdgeCase)
{
    Sequence a(0);

    for (uint32_t i(1); i < ((2^16)-1); i++)
    {
        Sequence b(i % (2^16));

        if (i < (2^15))
        {
            ASSERT_TRUE(b > a);
        }
        else
        {
            ASSERT_TRUE(a > b);
        }
    }
}
