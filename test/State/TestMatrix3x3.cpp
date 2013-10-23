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

#include <Implementation/Matrix3x3.hpp>

#include <gmock/gmock.h>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

// Class definition!
class TestMatrix3x3 : public ::testing::Test
{
public:
    const static uint ArraySize = 100;

    TestMatrix3x3()
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

    std::vector<Vector> groupA;
    std::vector<Vector> groupB;
    std::vector<Vector> groupC;
};

TEST_F(TestMatrix3x3, CreateEmpty)
{
    Matrix3x3 toTest;

    EXPECT_EQ(Vector3{}, toTest.values[0]);
    EXPECT_EQ(Vector3{}, toTest.values[1]);
    EXPECT_EQ(Vector3{}, toTest.values[2]);
}

TEST_F(TestMatrix3x3, CreateVector3)
{
    Matrix3x3 toTest
    {
        groupA[0],
        groupB[0],
        groupC[0]
    };

    EXPECT_EQ(groupA[0], toTest.values[0]);
    EXPECT_EQ(groupB[0], toTest.values[1]);
    EXPECT_EQ(groupC[0], toTest.values[2]);
}

TEST_F(TestMatrix3x3, CreateFloats)
{
    auto a = groupA[0];
    auto b = groupB[0];
    auto c = groupC[0];

    Matrix3x3 toTest
    {
        a.values[0], a.values[1], a.values[2],
        b.values[0], b.values[1], b.values[2],
        c.values[0], c.values[1], c.values[2],
    };

    EXPECT_EQ(a, toTest.values[0]);
    EXPECT_EQ(b, toTest.values[1]);
    EXPECT_EQ(c, toTest.values[2]);
}

TEST_F(TestMatrix3x3, CreateQuaternionIdentity)
{
    auto x = Vector3{1.0f};
    auto y = Vector3{0.0f, 1.0f};
    auto z = Vector3{0.0f, 0.0f, 1.0f};

    Matrix3x3 toTest
    {
        Quaternion{}
    };

    EXPECT_EQ(x, toTest.values[0]);
    EXPECT_EQ(y, toTest.values[1]);
    EXPECT_EQ(z, toTest.values[2]);
}

TEST_F(TestMatrix3x3, Add)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto va = Vector3(groupA[i]);
        auto vb = Vector3(groupB[i]);
        auto vc = Vector3(groupC[i]);

        auto a = Matrix3x3
        {
            va,
            vb,
            vc
        };

        auto b = Matrix3x3
        {
            vb,
            vc,
            va
        };

        auto c = a;

        auto ra = va + vb;
        auto rb = vb + vc;
        auto rc = vc + va;

        auto result = a + b;
        c += b;

        ASSERT_EQ(result, c)                << " i: " << i;
        ASSERT_EQ(ra, result.values[0]);
        ASSERT_EQ(rb, result.values[1]);
        ASSERT_EQ(rc, result.values[2]);
    }
}

TEST_F(TestMatrix3x3, Subtract)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto va = Vector3(groupA[i]);
        auto vb = Vector3(groupB[i]);
        auto vc = Vector3(groupC[i]);

        auto a = Matrix3x3
        {
            va,
            vb,
            vc
        };

        auto b = Matrix3x3
        {
            vb,
            vc,
            va
        };

        auto c = a;

        auto ra = va - vb;
        auto rb = vb - vc;
        auto rc = vc - va;

        auto result = a - b;
        c -= b;

        ASSERT_EQ(result, c)                << " i: " << i;
        ASSERT_EQ(ra, result.values[0]);
        ASSERT_EQ(rb, result.values[1]);
        ASSERT_EQ(rc, result.values[2]);
    }
}

TEST_F(TestMatrix3x3, Multiply)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto va = Vector3(groupA[i]);
        auto vb = Vector3(groupB[i]);
        auto vc = Vector3(groupC[i]);

        auto a = Matrix3x3
        {
            va,
            vb,
            vc
        };

        auto b = Matrix3x3
        {
            vb,
            vc,
            va
        };

        auto c = a;
        auto result = a * b;
        c *= b;

        auto expected = Matrix3x3
        {
                (va.values[0] * vb.values[0]) + (va.values[1] * vc.values[0]) + (va.values[2] * va.values[0]),
                (va.values[0] * vb.values[1]) + (va.values[1] * vc.values[1]) + (va.values[2] * va.values[1]),
                (va.values[0] * vb.values[2]) + (va.values[1] * vc.values[2]) + (va.values[2] * va.values[2]),

                (vb.values[0] * vb.values[0]) + (vb.values[1] * vc.values[0]) + (vb.values[2] * va.values[0]),
                (vb.values[0] * vb.values[1]) + (vb.values[1] * vc.values[1]) + (vb.values[2] * va.values[1]),
                (vb.values[0] * vb.values[2]) + (vb.values[1] * vc.values[2]) + (vb.values[2] * va.values[2]),

                (vc.values[0] * vb.values[0]) + (vc.values[1] * vc.values[0]) + (vc.values[2] * va.values[0]),
                (vc.values[0] * vb.values[1]) + (vc.values[1] * vc.values[1]) + (vc.values[2] * va.values[1]),
                (vc.values[0] * vb.values[2]) + (vc.values[1] * vc.values[2]) + (vc.values[2] * va.values[2]),
        };

        //ASSERT_EQ(result, c) << " i: " << i; // RAM: Failing at i ==2 - WHY?
        ASSERT_EQ(expected, result);
    }
}

}}} // namespace


