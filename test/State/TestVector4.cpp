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
#include <Implementation/Vector4.hpp>

#include <gmock/gmock.h>

#include <vector>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

class TestVector4 : public ::testing::Test
{
public:
    const static uint ArraySize = 100;

    TestVector4()
        : randomEngine(4)
        , randomNegMilltoMill(-1000000, 1000000)
    {
        groupA.reserve(ArraySize);
        groupB.reserve(ArraySize);
        groupC.reserve(ArraySize);

        for (uint i = 0; i < ArraySize; ++i)
        {
            groupA.push_back({{{
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine)}}});


            groupB.push_back({{{
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine)}}});


            groupC.push_back({{{
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine)}}});
        }
    }

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> randomNegMilltoMill;

    std::vector<Vector> groupA;
    std::vector<Vector> groupB;
    std::vector<Vector> groupC;
};

TEST_F(TestVector4, Empty)
{
    auto toTest = (Vector4{0.0f}).ToVector();

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[3]);
}

TEST_F(TestVector4, XYZW)
{
    auto toTest = Vector4{groupA[0].values};

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(toTest.X(), groupA[0].values[0]);
    EXPECT_FLOAT_EQ(toTest.Y(), groupA[0].values[1]);
    EXPECT_FLOAT_EQ(toTest.Z(), groupA[0].values[2]);
    EXPECT_FLOAT_EQ(toTest.W(), groupA[0].values[3]);
}

TEST_F(TestVector4, Equal)
{
    auto a = (Vector4{-1.0f, 0.0f, 1.0f}).ToVector();
    auto b = (Vector4{-1.0f, 0.0f, 1.0f}).ToVector();

    EXPECT_EQ(a, b);
    EXPECT_FLOAT_EQ(a.values[0], b.values[0]);
    EXPECT_FLOAT_EQ(a.values[1], b.values[1]);
    EXPECT_FLOAT_EQ(a.values[2], b.values[2]);
    EXPECT_FLOAT_EQ(a.values[3], b.values[3]);
}

TEST_F(TestVector4, NotEqual)
{
    auto a = (Vector4{-1.0f, 0.0f, 1.0f, 99999.0f}).ToVector();
    auto b = std::vector<Vector>{};

    b.push_back({1.0f, 0.0f, 1.0f, 99999.0f});
    b.push_back({0.0f, 0.0f, 1.0f, 99999.0f});
    b.push_back({-1.0f, 1.0f, 1.0f, 99999.0f});
    b.push_back({-1.0f, 0.0f, -1.0f, 99999.0f});
    b.push_back({0.0f, 0.0f, 0.0f, 99999.0f});
    b.push_back({-1.0f, 0.0f, 1.0f, 99998.0f});

    for (auto& toTest : b)
    {
        EXPECT_NE(toTest, a);
    }
}

TEST_F(TestVector4, Add)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4{groupA[i].values};
        auto b = Vector4{groupB[i].values};
        auto c = Vector4{groupC[i].values};

        auto result = (a + b).ToVector();
        c += a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] + groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] + groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] + groupB[i].values[2]);
        ASSERT_FLOAT_EQ(result.values[3], groupA[i].values[3] + groupB[i].values[3]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] + groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] + groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupC[i].values[2] + groupA[i].values[2]);
        ASSERT_FLOAT_EQ(result2.values[3], groupC[i].values[3] + groupA[i].values[3]);
    }
}

TEST_F(TestVector4, Subtract)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4{groupA[i].values};
        auto b = Vector4{groupB[i].values};
        auto c = Vector4{groupC[i].values};

        auto result = (a - b).ToVector();
        c -= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] - groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] - groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] - groupB[i].values[2]);
        ASSERT_FLOAT_EQ(result.values[3], groupA[i].values[3] - groupB[i].values[3]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] - groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] - groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] - groupB[i].values[2]);
        ASSERT_FLOAT_EQ(result.values[3], groupA[i].values[3] - groupB[i].values[3]);
    }
}

TEST_F(TestVector4, Multiply)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {

        auto a = Vector4{groupA[i].values};
        auto b = Vector4{groupB[i].values};
        auto c = Vector4{groupC[i].values};

        auto result = (a * b).ToVector();
        c *= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] * groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] * groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] * groupB[i].values[2]);
        ASSERT_FLOAT_EQ(result.values[3], groupA[i].values[3] * groupB[i].values[3]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] * groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] * groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupC[i].values[2] * groupA[i].values[2]);
        ASSERT_FLOAT_EQ(result2.values[3], groupC[i].values[3] * groupA[i].values[3]);
    }
}

TEST_F(TestVector4, Divide)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {

        auto a = Vector4{groupA[i].values};
        auto b = Vector4{groupB[i].values};
        auto c = Vector4{groupC[i].values};

        auto result = (a / b).ToVector();
        c /= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] / groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] / groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] / groupB[i].values[2]);
        ASSERT_FLOAT_EQ(result.values[3], groupA[i].values[3] / groupB[i].values[3]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] / groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] / groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupC[i].values[2] / groupA[i].values[2]);
        ASSERT_FLOAT_EQ(result2.values[3], groupC[i].values[3] / groupA[i].values[3]);
    }
}

TEST_F(TestVector4, Mad)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {

        auto a = Vector4{groupA[i].values};
        auto b = Vector4{groupB[i].values};
        auto c = Vector4{groupC[i].values};

        auto result = ((a*b) + c).ToVector();
        auto d = c;
        d += a * b;
        auto result2 = (d).ToVector();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] * groupB[i].values[0] + groupC[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] * groupB[i].values[1] + groupC[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] * groupB[i].values[2] + groupC[i].values[2]);
        ASSERT_FLOAT_EQ(result.values[3], groupA[i].values[3] * groupB[i].values[3] + groupC[i].values[3]);

        ASSERT_FLOAT_EQ(result2.values[0], groupA[i].values[0] * groupB[i].values[0] + groupC[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result2.values[1], groupA[i].values[1] * groupB[i].values[1] + groupC[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupA[i].values[2] * groupB[i].values[2] + groupC[i].values[2]);
        ASSERT_FLOAT_EQ(result2.values[3], groupA[i].values[3] * groupB[i].values[3] + groupC[i].values[3]);
    }
}

TEST_F(TestVector4, FnSqrt)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        // Don't bother if anything is negative.
        if  (
                (groupA[i].values[0] >= 0) &&
                (groupA[i].values[1] >= 0) &&
                (groupA[i].values[2] >= 0) &&
                (groupA[i].values[3] >= 0)
             )
        {
            auto a = Vector4{groupA[i].values};
            auto result = (Sqrt(a)).ToVector();

            ASSERT_FLOAT_EQ(result.values[0], sqrt(groupA[i].values[0])) << " i: " << i;
            ASSERT_FLOAT_EQ(result.values[1], sqrt(groupA[i].values[1]));
            ASSERT_FLOAT_EQ(result.values[2], sqrt(groupA[i].values[2]));
            ASSERT_FLOAT_EQ(result.values[3], sqrt(groupA[i].values[3]));
        }
    }
}

TEST_F(TestVector4, FnDot)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4{groupA[i].values};
        auto b = Vector4{groupB[i].values};
        auto result = (Dot(a, b)).ToVector();
        auto dot =
                (groupA[i].values[0] * groupB[i].values[0]) +
                (groupA[i].values[1] * groupB[i].values[1]) +
                (groupA[i].values[2] * groupB[i].values[2]) +
                (groupA[i].values[3] * groupB[i].values[3]);

        ASSERT_FLOAT_EQ(result.values[0], dot) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], dot);
        ASSERT_FLOAT_EQ(result.values[2], dot);
        ASSERT_FLOAT_EQ(result.values[3], dot);
    }
}

TEST_F(TestVector4, FnLength)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4{groupA[i].values};
        auto result = (Length(a)).ToVector();
        auto l =
                sqrt((groupA[i].values[0] * groupA[i].values[0]) +
                     (groupA[i].values[1] * groupA[i].values[1]) +
                     (groupA[i].values[2] * groupA[i].values[2]) +
                     (groupA[i].values[3] * groupA[i].values[3]));

        ASSERT_FLOAT_EQ(result.values[0], l) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], l);
        ASSERT_FLOAT_EQ(result.values[2], l);
        ASSERT_FLOAT_EQ(result.values[3], l);
    }
}

TEST_F(TestVector4, FnLengthSquared)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4{groupA[i].values};
        auto result = (Length(a)).ToVector();
        auto l =
                sqrt((groupA[i].values[0] * groupA[i].values[0]) +
                     (groupA[i].values[1] * groupA[i].values[1]) +
                     (groupA[i].values[2] * groupA[i].values[2]) +
                     (groupA[i].values[3] * groupA[i].values[3]));



        ASSERT_FLOAT_EQ(result.values[0], l) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], l);
        ASSERT_FLOAT_EQ(result.values[2], l);
        ASSERT_FLOAT_EQ(result.values[3], l);
    }
}

TEST_F(TestVector4, FnNormalise)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4{groupA[i].values};
        auto result = (Normalise(a)).ToVector();
        auto l = sqrt(
            (groupA[i].values[0] * groupA[i].values[0]) +
            (groupA[i].values[1] * groupA[i].values[1]) +
            (groupA[i].values[2] * groupA[i].values[2]) +
            (groupA[i].values[3] * groupA[i].values[3]));

        auto answer = Vector{{{
                groupA[i].values[0] / l,
                groupA[i].values[1] / l,
                groupA[i].values[2] / l,
                groupA[i].values[3] / l}}};

        ASSERT_FLOAT_EQ(result.values[0], answer.values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], answer.values[1]);
        ASSERT_FLOAT_EQ(result.values[2], answer.values[2]);
        ASSERT_FLOAT_EQ(result.values[3], answer.values[3]);
    }
}

}}} // namespace

