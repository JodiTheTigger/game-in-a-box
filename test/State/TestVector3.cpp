
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

#include <Implementation/VectorPod.hpp>
#include <Implementation/Vector3.hpp>

#include <gmock/gmock.h>

#include <vector>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

class TestVector3 : public ::testing::Test
{
public:
    const static uint ArraySize = 100;

    TestVector3()
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
                                 randomNegMilltoMill(randomEngine)}}});


            groupB.push_back({{{
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine)}}});


            groupC.push_back({{{
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine),
                                 randomNegMilltoMill(randomEngine)}}});
        }
    }

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> randomNegMilltoMill;

    std::vector<VectorPod> groupA;
    std::vector<VectorPod> groupB;
    std::vector<VectorPod> groupC;
};

TEST_F(TestVector3, Empty)
{
    auto toTest = (Vector3{}).ToVectorPod();

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[3]);
}

TEST_F(TestVector3, ZeroReplicate)
{
    auto toTest = (Vector3(0, Vector3::tagReplicate{})).ToVectorPod();

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[3]);
}

TEST_F(TestVector3, Equal)
{
    auto a = (Vector3(-1.0f, 0.0f, 1.0f)).ToVectorPod();
    auto b = (Vector3(-1.0f, 0.0f, 1.0f)).ToVectorPod();

    EXPECT_EQ(a, b);
    EXPECT_FLOAT_EQ(a.values[0], b.values[0]);
    EXPECT_FLOAT_EQ(a.values[1], b.values[1]);
    EXPECT_FLOAT_EQ(a.values[2], b.values[2]);
}

TEST_F(TestVector3, NotEqual)
{
    auto a = (Vector3(-1.0f, 0.0f, 1.0f)).ToVectorPod();
    auto b = std::vector<VectorPod>{};

    b.push_back({{{1.0f, 0.0f, 1.0f}}});
    b.push_back({{{0.0f, 0.0f, 1.0f}}});
    b.push_back({{{-1.0f, 1.0f, 1.0f}}});
    b.push_back({{{-1.0f, 0.0f, -1.0f}}});
    b.push_back({{{0.0f, 0.0f, 0.0f}}});

    for (auto& toTest : b)
    {
        EXPECT_NE(toTest, a);
    }
}

TEST_F(TestVector3, Add)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto b = Vector3(groupB[i]);
        auto c = Vector3(groupC[i]);

        auto result = (a + b).ToVectorPod();
        c += a;
        auto result2 = (c).ToVectorPod();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] + groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] + groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] + groupB[i].values[2]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] + groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] + groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupC[i].values[2] + groupA[i].values[2]);
    }
}

TEST_F(TestVector3, Subtract)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto b = Vector3(groupB[i]);
        auto c = Vector3(groupC[i]);

        auto result = (a - b).ToVectorPod();
        c -= a;
        auto result2 = (c).ToVectorPod();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] - groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] - groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] - groupB[i].values[2]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] - groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] - groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupC[i].values[2] - groupA[i].values[2]);
    }
}

TEST_F(TestVector3, Multiply)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto b = Vector3(groupB[i]);
        auto c = Vector3(groupC[i]);

        auto result = (a * b).ToVectorPod();
        c *= a;
        auto result2 = (c).ToVectorPod();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] * groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] * groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] * groupB[i].values[2]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] * groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] * groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupC[i].values[2] * groupA[i].values[2]);
    }
}

TEST_F(TestVector3, Divide)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto b = Vector3(groupB[i]);
        auto c = Vector3(groupC[i]);

        auto result = (a / b).ToVectorPod();
        c /= a;
        auto result2 = (c).ToVectorPod();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] / groupB[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] / groupB[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] / groupB[i].values[2]);

        ASSERT_FLOAT_EQ(result2.values[0], groupC[i].values[0] / groupA[i].values[0]);
        ASSERT_FLOAT_EQ(result2.values[1], groupC[i].values[1] / groupA[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupC[i].values[2] / groupA[i].values[2]);
    }
}

TEST_F(TestVector3, Mad)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto b = Vector3(groupB[i]);
        auto c = Vector3(groupC[i]);

        auto result = ((a*b) + c).ToVectorPod();
        auto d = c;
        d += a * b;
        auto result2 = (d).ToVectorPod();

        ASSERT_FLOAT_EQ(result.values[0], groupA[i].values[0] * groupB[i].values[0] + groupC[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], groupA[i].values[1] * groupB[i].values[1] + groupC[i].values[1]);
        ASSERT_FLOAT_EQ(result.values[2], groupA[i].values[2] * groupB[i].values[2] + groupC[i].values[2]);

        ASSERT_FLOAT_EQ(result2.values[0], groupA[i].values[0] * groupB[i].values[0] + groupC[i].values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result2.values[1], groupA[i].values[1] * groupB[i].values[1] + groupC[i].values[1]);
        ASSERT_FLOAT_EQ(result2.values[2], groupA[i].values[2] * groupB[i].values[2] + groupC[i].values[2]);
    }
}

TEST_F(TestVector3, FnSqrt)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        // Don't bother if anything is negative.
        if  (
                (groupA[i].values[0] >= 0) &&
                (groupA[i].values[1] >= 0) &&
                (groupA[i].values[2] >= 0)
             )
        {
            auto a = Vector3(groupA[i]);
            auto result = (Sqrt(a)).ToVectorPod();

            ASSERT_FLOAT_EQ(result.values[0], sqrt(groupA[i].values[0])) << " i: " << i;
            ASSERT_FLOAT_EQ(result.values[1], sqrt(groupA[i].values[1]));
            ASSERT_FLOAT_EQ(result.values[2], sqrt(groupA[i].values[2]));
        }
    }
}

TEST_F(TestVector3, FnDot)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto b = Vector3(groupB[i]);
        auto result = (Dot(a, b)).ToVectorPod();
        auto dot =
                (groupA[i].values[0] * groupB[i].values[0]) +
                (groupA[i].values[1] * groupB[i].values[1]) +
                (groupA[i].values[2] * groupB[i].values[2]);

        ASSERT_FLOAT_EQ(result.values[0], dot) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], dot);
        ASSERT_FLOAT_EQ(result.values[2], dot);
    }
}

TEST_F(TestVector3, FnLength)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto result = (Length(a)).ToVectorPod();
        auto l =
                sqrt((groupA[i].values[0] * groupA[i].values[0]) +
                     (groupA[i].values[1] * groupA[i].values[1]) +
                     (groupA[i].values[2] * groupA[i].values[2]));

        ASSERT_FLOAT_EQ(result.values[0], l) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], l);
        ASSERT_FLOAT_EQ(result.values[2], l);
    }
}

TEST_F(TestVector3, FnLengthSquared)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto result = (Length(a)).ToVectorPod();
        auto l =
                sqrt((groupA[i].values[0] * groupA[i].values[0]) +
                     (groupA[i].values[1] * groupA[i].values[1]) +
                     (groupA[i].values[2] * groupA[i].values[2]));



        ASSERT_FLOAT_EQ(result.values[0], l) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], l);
        ASSERT_FLOAT_EQ(result.values[2], l);
    }
}

TEST_F(TestVector3, FnNormaliseFast)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto result = (Normalise(a)).ToVectorPod();
        auto l = sqrt(
            (groupA[i].values[0] * groupA[i].values[0]) +
            (groupA[i].values[1] * groupA[i].values[1]) +
            (groupA[i].values[2] * groupA[i].values[2]));

        auto answer = VectorPod{{{
                groupA[i].values[0] / l,
                groupA[i].values[1] / l,
                groupA[i].values[2] / l}}};

        ASSERT_FLOAT_EQ(result.values[0], answer.values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], answer.values[1]);
        ASSERT_FLOAT_EQ(result.values[2], answer.values[2]);
    }
}

TEST_F(TestVector3, FnNormaliseAccurate)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector3(groupA[i]);
        auto result = (NormaliseStable(a)).ToVectorPod();
        auto l = sqrt(
            (groupA[i].values[0] * groupA[i].values[0]) +
            (groupA[i].values[1] * groupA[i].values[1]) +
            (groupA[i].values[2] * groupA[i].values[2]));

        auto answer = VectorPod{{{
                groupA[i].values[0] / l,
                groupA[i].values[1] / l,
                groupA[i].values[2] / l}}};

        ASSERT_FLOAT_EQ(result.values[0], answer.values[0]) << " i: " << i;
        ASSERT_FLOAT_EQ(result.values[1], answer.values[1]);
        ASSERT_FLOAT_EQ(result.values[2], answer.values[2]);
        ASSERT_FLOAT_EQ(result.values[3], answer.values[3]);
    }
}

}}} // namespace
