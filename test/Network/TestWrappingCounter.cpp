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

#include <WrappingCounter.hpp>
#include <gtest/gtest.h>

#include <limits>

using namespace std;

namespace GameInABox { namespace Network {

template <typename T>
class TestWrappingCounter : public ::testing::Test
{
public:
    typedef WrappingCounter<T> WrapCount;
    typedef WrappingCounter<T, 7> WrapCount7;
    typedef WrappingCounter<T, 15> WrapCount15;

    typedef T Datum;

    enum
    {
        maxValue = (uint64_t(WrapCount::max()) + 1),
        maxValueHalf = ((uint64_t(WrapCount::max()) + 1) >> 1),

        maxValue7 = (uint64_t(WrapCount7::max()) + 1),
        maxValueHalf7 = ((uint64_t(WrapCount7::max()) + 1) >> 1),

        maxValue15 = (uint64_t(WrapCount15::max()) + 1),
        maxValueHalf15 = ((uint64_t(WrapCount15::max()) + 1) >> 1)
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

        typename TestFixture::WrapCount7 a7(i);
        typename TestFixture::WrapCount7 b7(i);

        typename TestFixture::WrapCount15 a15(i);
        typename TestFixture::WrapCount15 b15(i);

        ASSERT_EQ(a, b);
        ASSERT_EQ(a7, b7);
        ASSERT_EQ(a15, b15);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleNotEqual)
{
    for (uint64_t i(0); i < uint64_t(TestFixture::maxValue); i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+42);

        typename TestFixture::WrapCount7 a7(i);
        typename TestFixture::WrapCount7 b7(i+42);

        typename TestFixture::WrapCount15 a15(i);
        typename TestFixture::WrapCount15 b15(i+42);

        ASSERT_NE(a, b);
        ASSERT_NE(a7, b7);
        ASSERT_NE(a15, b15);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleEqualAgainstUint64)
{
    for (uint64_t i(0); i < uint64_t(TestFixture::maxValue); i++)
    {
        typename TestFixture::WrapCount a(i);

        ASSERT_EQ(a, i);
    }


    for (uint64_t i(0); i < uint64_t(TestFixture::maxValue7); i++)
    {
        typename TestFixture::WrapCount7 a7(i);

        ASSERT_EQ(a7, i);
    }


    for (uint64_t i(0); i < uint64_t(TestFixture::maxValue15); i++)
    {
        typename TestFixture::WrapCount15 a15(i);

        ASSERT_EQ(a15, i);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleAdd)
{
    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+1);
        typename TestFixture::WrapCount c(i+i+1);

        ASSERT_EQ(a + b, c);
    }

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i);
        typename TestFixture::WrapCount7 b(i+1);
        typename TestFixture::WrapCount7 c(i+i+1);

        ASSERT_EQ(a + b, c);
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i);
        typename TestFixture::WrapCount15 b(i+1);
        typename TestFixture::WrapCount15 c(i+i+1);

        ASSERT_EQ(a + b, c);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleSubtract)
{
    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount b(i+1);

        ASSERT_EQ(b - a, typename TestFixture::WrapCount(1));
    }

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i);
        typename TestFixture::WrapCount7 b(i+1);

        ASSERT_EQ(b - a, typename TestFixture::WrapCount7(1));
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i);
        typename TestFixture::WrapCount15 b(i+1);

        ASSERT_EQ(b - a, typename TestFixture::WrapCount15(1));
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleAddAssign)
{
    typename TestFixture::Datum toDelta(100);

    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i);
        typename TestFixture::WrapCount c(i + 100);

        a += toDelta;

        ASSERT_EQ(a, c);
    }

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i);
        typename TestFixture::WrapCount7 c(i + 20);

        a += typename TestFixture::Datum(20);

        ASSERT_EQ(a, c);
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i);
        typename TestFixture::WrapCount15 c(i + 100);

        a += toDelta;

        ASSERT_EQ(a, c);
    }
}

TYPED_TEST(TestWrappingCounter, TestSimpleSubtractAssign)
{
    typename TestFixture::Datum toDelta(100);

    for (uint64_t i(0); i < TestFixture::maxValue; i++)
    {
        typename TestFixture::WrapCount a(i + 100);
        typename TestFixture::WrapCount b(i);

        a -= toDelta;

        ASSERT_EQ(a, b);
    }

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i + 20);
        typename TestFixture::WrapCount7 b(i);

        a -= typename TestFixture::Datum(20);

        ASSERT_EQ(a, b);
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i + 100);
        typename TestFixture::WrapCount15 b(i);

        a -= toDelta;

        ASSERT_EQ(a, b);
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

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i);
        typename TestFixture::WrapCount7 b(i+1);

        ASSERT_LT(a, b);
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i);
        typename TestFixture::WrapCount15 b(i+1);

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

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i);
        typename TestFixture::WrapCount7 b(i+1);

        ASSERT_GT(b, a);
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i);
        typename TestFixture::WrapCount15 b(i+1);

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

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i);
        typename TestFixture::WrapCount7 b(i+20);

        ASSERT_GT(b, a);
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i);
        typename TestFixture::WrapCount15 b(i+100);

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

    for (uint64_t i(0); i < TestFixture::maxValue7; i++)
    {
        typename TestFixture::WrapCount7 a(i);
        typename TestFixture::WrapCount7 b(i+20);

        ASSERT_LT(a, b);
    }

    for (uint64_t i(0); i < TestFixture::maxValue15; i++)
    {
        typename TestFixture::WrapCount15 a(i);
        typename TestFixture::WrapCount15 b(i+100);

        ASSERT_LT(a, b);
    }
}

TYPED_TEST(TestWrappingCounter, TestLessThanEdgeCase)
{
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

    {
        typename TestFixture::WrapCount7 a(0);

        for (uint64_t i(1); i < TestFixture::maxValue7; i++)
        {
            typename TestFixture::WrapCount7 b(i);

            if (i <= TestFixture::maxValueHalf7)
            {
                ASSERT_LT(a, b);
            }
            else
            {
                ASSERT_LT(b, a);
            }
        }
    }


    {
        typename TestFixture::WrapCount15 a(0);

        for (uint64_t i(1); i < TestFixture::maxValue15; i++)
        {
            typename TestFixture::WrapCount15 b(i);

            if (i <= TestFixture::maxValueHalf15)
            {
                ASSERT_LT(a, b);
            }
            else
            {
                ASSERT_LT(b, a);
            }
        }
    }
}


TYPED_TEST(TestWrappingCounter, TestGreaterThanEdgeCase)
{
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

    {
        typename TestFixture::WrapCount7 a(0);

        for (uint64_t i(1); i < TestFixture::maxValue7; i++)
        {
            typename TestFixture::WrapCount7 b(i);

            if (i <= TestFixture::maxValueHalf7)
            {
                ASSERT_GT(b, a);
            }
            else
            {
                ASSERT_GT(a, b);
            }
        }
    }

    {
        typename TestFixture::WrapCount15 a(0);

        for (uint64_t i(1); i < TestFixture::maxValue15; i++)
        {
            typename TestFixture::WrapCount15 b(i);

            if (i <= TestFixture::maxValueHalf15)
            {
                ASSERT_GT(b, a);
            }
            else
            {
                ASSERT_GT(a, b);
            }
        }
    }
}

TYPED_TEST(TestWrappingCounter, TestCreatedOutOfBounds)
{
    typename TestFixture::WrapCount7 a{TestFixture::maxValue - 1};
    typename TestFixture::WrapCount15 b{TestFixture::maxValue - 1};

    ASSERT_EQ(a.Value(), (TestFixture::maxValue - 1) & (TestFixture::maxValue7 - 1));
    ASSERT_EQ(b.Value(), (TestFixture::maxValue - 1) & (TestFixture::maxValue15 - 1));
}

TYPED_TEST(TestWrappingCounter, MaxMinusMax)
{
    auto a1 = typename TestFixture::WrapCount{TestFixture::WrapCount::max()};
    auto a2 = a1;

    auto b1 = typename TestFixture::WrapCount7{TestFixture::WrapCount7::max()};
    auto b2 = b1;

    auto c1 = typename TestFixture::WrapCount15{TestFixture::WrapCount15::max()};
    auto c2 = c1;

    //typename TestFixture::WrapCount a{TestFixture::maxValue - 1}
    //typename TestFixture::WrapCount7 b{TestFixture::maxValue - 1};
    //typename TestFixture::WrapCount15 c{TestFixture::maxValue - 1};
    auto toTestA = a1 - a2;
    auto toTestB = b1 - b2;
    auto toTestC = c1 - c2;

    ASSERT_EQ(typename TestFixture::Datum{0}, toTestA);
    ASSERT_EQ(typename TestFixture::Datum{0}, toTestB);
    ASSERT_EQ(typename TestFixture::Datum{0}, toTestC);
}

}} // namespace
