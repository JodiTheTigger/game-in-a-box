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

// RAM: TODO: Add, subtract, multiply, divide, length, length squared, dot, xyzw.

}}} // namespace
