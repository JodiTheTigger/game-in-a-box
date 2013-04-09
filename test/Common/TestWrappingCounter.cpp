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

#include <Common/WrappingCounter.h>
#include <gtest/gtest.h>

#include <limits>

using namespace std;

template <typename T>
class TestWrappingCounter : public ::testing::Test
{
public:
    typedef WrappingCounter<T> WrapCount;

    enum
    {
        maxValue = (uint64_t(std::numeric_limits<T>::max()) + 1),
        maxValueHalf = ((uint64_t(std::numeric_limits<T>::max()) + 1) >> 1)
    };
};

// TYPES I'M TESTING!
// Enabling uint32_t means the test will take about 30 min.
// typedef ::testing::Types<uint8_t, uint16_t, uint32_t> TestSequenceTypes;
typedef ::testing::Types<uint8_t, uint16_t> TestSequenceTypes;
TYPED_TEST_CASE(TestWrappingCounter, TestSequenceTypes);

// THE TESTS!
TYPED_TEST(TestWrappingCounter, TestSimpleEqual)
{    
    for (uint64_t i(0); i < uint64_t(TestFixture::maxValue); i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i);

        ASSERT_EQ(a, b);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleNotEqual)
{
    for (uint64_t i(0); i < uint64_t(TestFixture::maxValue); i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+42);

        ASSERT_NE(a, b);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleEqualAgainstUint64)
{
    for (uint64_t i(0); i < uint64_t(TestFixture::maxValue); i++)
    {
        typename TestFixture::WrapCount a(i);

        ASSERT_EQ(a, i);
    }
}


TYPED_TEST(TestWrappingCounter, TestSimpleLessThan)
{
    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+1);

        ASSERT_LT(a, b);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleGreaterThan)
{
    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+1);

        ASSERT_GT(b, a);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleGreaterThanWraparound)
{
    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+100);

        ASSERT_GT(b, a);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleLessThanWraparound)
{
    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+100);

        ASSERT_LT(a, b);
    }
}

TYPED_TEST(TestWrappingCounter, TestLessThanEdgeCase)
{
    typename TestFixture::WrapCount a(0);

    for (uint64_t i(1); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount b(i);

        if (i <= TestFixture::maxValueHalf)
        {
            ASSERT_LT(a, b);
        }
        else
        {
            ASSERT_LT(b, a);
        }
    }
}


TYPED_TEST(TestWrappingCounter, TestGreaterThanEdgeCase)
{
    typename TestFixture::WrapCount a(0);

    for (uint64_t i(1); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount b(i);

        if (i <= TestFixture::maxValueHalf)
        {
            ASSERT_GT(b, a);
        }
        else
        {
            ASSERT_GT(a, b);
        }
    }
}
