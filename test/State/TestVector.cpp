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

#include <Implementation/Vector.hpp>

#include <gmock/gmock.h>

#include <vector>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

class TestVector : public ::testing::Test
{
public:
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
                randomNegMilltoMill(randomEngine)
            }}});

            groupB.push_back(
            {{{
                randomNegMilltoMill(randomEngine),
                randomNegMilltoMill(randomEngine),
                randomNegMilltoMill(randomEngine),
                randomNegMilltoMill(randomEngine)
            }}});

            groupC.push_back(
            {{{
                randomNegMilltoMill(randomEngine),
                randomNegMilltoMill(randomEngine),
                randomNegMilltoMill(randomEngine),
                randomNegMilltoMill(randomEngine)
            }}});
        }
    }

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> randomNegMilltoMill;

    std::vector<Vector> groupA;
    std::vector<Vector> groupB;
    std::vector<Vector> groupC;
};

TEST_F(TestVector, Empty)
{
    auto toTest = Vector{{{0.0f}}};

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[3]);
}

TEST_F(TestVector, Equal)
{
    auto a = Vector{{{-1.0f, 0.0f, 1.0f, 123456789.0f}}};
    auto b = Vector{{{-1.0f, 0.0f, 1.0f, 123456789.0f}}};

    EXPECT_EQ(a, b);
    EXPECT_FLOAT_EQ(a.values[0], b.values[0]);
    EXPECT_FLOAT_EQ(a.values[1], b.values[1]);
    EXPECT_FLOAT_EQ(a.values[2], b.values[2]);
    EXPECT_FLOAT_EQ(a.values[3], b.values[3]);
}

TEST_F(TestVector, NotEqual)
{
    auto a = Vector{{{-1.0f, 0.0f, 1.0f, 123456789.0f}}};
    auto b = std::vector<Vector>{};

    b.push_back({{{1.0f, 0.0f, 1.0f, 123456789.0f}}});
    b.push_back({{{0.0f, 0.0f, 1.0f, 123456789.0f}}});
    b.push_back({{{-1.0f, 1.0f, 1.0f, 123456789.0f}}});
    b.push_back({{{-1.0f, 0.0f, -1.0f, 123456789.0f}}});
    b.push_back({{{-1.0f, 0.0f, -1.0f, 12345689.0f}}});
    b.push_back({{{-1.0f, 0.0f, 1.0f, -123456789.0f}}});
    b.push_back({{{0.0f, 0.0f, 0.0f, 0.0f}}});

    for (auto& toTest : b)
    {
        EXPECT_NE(toTest, a);
    }
}

TEST_F(TestVector, FnXYZW)
{
    auto toTest = Vector{{{-1.0f, 0.0f, 2.0f, 123456789.0f}}};

    EXPECT_FLOAT_EQ(toTest.values[0], -1.0f);
    EXPECT_FLOAT_EQ(toTest.values[1], 0.0f);
    EXPECT_FLOAT_EQ(toTest.values[2], 2.0f);
    EXPECT_FLOAT_EQ(toTest.values[3], 123456789.0f);
}

}}} // namespace
