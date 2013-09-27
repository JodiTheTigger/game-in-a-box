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

#include <Implementation/VectorSimple.hpp>
#include <Implementation/VectorSimpleByValue.hpp>

#include <gmock/gmock.h>

#include <vector>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

template <typename T>
class TestVector : public ::testing::Test
{
public:
    typedef T Vec;

    const static uint ArraySize = 100;

    TestVector()
        : randomEngine(4)
        , randomNegMilltoMill(-1000000, 1000000)
    {
        groupA.reserve(ArraySize);
        groupB.reserve(ArraySize);
        groupC.reserve(ArraySize);

        for (uint i = 0; i < ArraySize; ++i)
        {
            groupA.push_back(
            {{{
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
            }}});

            groupB.push_back(
            {{{
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
            }}});

            groupC.push_back(
            {{{
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
                 randomNegMilltoMill(randomEngine),
            }}});
        }
    }

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> randomNegMilltoMill;

    std::vector<T> groupA;
    std::vector<T> groupB;
    std::vector<T> groupC;
};

typedef ::testing::Types<VectorSimple, VectorSimpleByValue> TestVectorTypes;
TYPED_TEST_CASE(TestVector, TestVectorTypes);

TYPED_TEST(TestVector, Empty)
{
    typename TestFixture::Vec toTest = {0,0,0,0};

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, X(toTest));
    EXPECT_FLOAT_EQ(0.0f, Y(toTest));
    EXPECT_FLOAT_EQ(0.0f, Z(toTest));
    EXPECT_FLOAT_EQ(0.0f, W(toTest));
}

TYPED_TEST(TestVector, Equal)
{
    auto a = typename TestFixture::Vec{-1.0f, 0.0f, 1.0f, 123456789.0f};
    auto b = typename TestFixture::Vec{-1.0f, 0.0f, 1.0f, 123456789.0f};

    EXPECT_EQ(a, b);
    EXPECT_FLOAT_EQ(X(a), X(b));
    EXPECT_FLOAT_EQ(Y(a), Y(b));
    EXPECT_FLOAT_EQ(Z(a), Z(b));
    EXPECT_FLOAT_EQ(W(a), W(b));
}

TYPED_TEST(TestVector, NotEqual)
{
    auto a = typename TestFixture::Vec{-1.0f, 0.0f, 1.0f, 123456789.0f};
    auto b = std::vector<typename TestFixture::Vec>{};

    b.push_back({1.0f, 0.0f, 1.0f, 123456789.0f});
    b.push_back({0.0f, 0.0f, 1.0f, 123456789.0f});
    b.push_back({-1.0f, 1.0f, 1.0f, 123456789.0f});
    b.push_back({-1.0f, 0.0f, -1.0f, 123456789.0f});
    b.push_back({-1.0f, 0.0f, -1.0f, 12345689.0f});
    b.push_back({-1.0f, 0.0f, 1.0f, -123456789.0f});
    b.push_back({0.0f, 0.0f, 0.0f, 0.0f});

    for (auto& toTest : b)
    {
        EXPECT_NE(toTest, a);
    }
}

TYPED_TEST(TestVector, Add)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto result = TestFixture::groupA[i] + TestFixture::groupB[i];
        auto result2 = TestFixture::groupC[i];
        result2 += TestFixture::groupA[i];

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) + X(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) + Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) + Z(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) + W(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) + X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) + Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) + Z(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupC[i]) + W(TestFixture::groupA[i]));
    }
}

// RAM: TODO: Add, subtract, multiply, divide, length, length squared, dot, xyzw.

}}} // namespace
