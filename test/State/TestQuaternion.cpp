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

#include <Implementation/Quaternion.hpp>
#include <Implementation/VectorCommon.hpp>

#include <gmock/gmock.h>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

// Class definition!
class TestQuaternion : public ::testing::Test
{
};

TEST_F(TestQuaternion, Empty)
{
    auto toTest = (Quaternion{}).ToVector();

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(1.0f, toTest.values[3]);
}

TEST_F(TestQuaternion, FromFloats)
{
    auto toTest = (Quaternion{1.0f, 2.0f, 3.0f, 4.0f}).ToVector();

    EXPECT_FLOAT_EQ(1.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(2.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(3.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(4.0f, toTest.values[3]);
}

TEST_F(TestQuaternion, FromVector)
{
    auto toTest = (Quaternion{Vector{{{1.0f, 2.0f, 3.0f, 4.0f}}}}).ToVector();

    EXPECT_FLOAT_EQ(1.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(2.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(3.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(4.0f, toTest.values[3]);
}

TEST_F(TestQuaternion, FromArray)
{
    auto array = std::array<float,4> {{1.0f, 2.0f, 3.0f, 4.0f}};
    auto toTest = (Quaternion{array}).ToVector();

    EXPECT_FLOAT_EQ(1.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(2.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(3.0f, toTest.values[2]);
    EXPECT_FLOAT_EQ(4.0f, toTest.values[3]);
}

TEST_F(TestQuaternion, AxisAndAngle)
{
    auto angle = Radians{2};
    auto vector = Normalise(Vector3{1,2,3});
    auto toTest = (Quaternion{vector, angle}).ToVector();

    // do Quaternion maths here to verify
    auto sin_a = std::sin( angle.value / 2.0f );
    auto cos_a = std::cos( angle.value / 2.0f );

    auto expected = Vector
    {{{
        vector.ToVector().values[0] * sin_a,
        vector.ToVector().values[1] * sin_a,
        vector.ToVector().values[2] * sin_a,
        cos_a
    }}};

    EXPECT_FLOAT_EQ(expected.values[0], toTest.values[0]);
    EXPECT_FLOAT_EQ(expected.values[1], toTest.values[1]);
    EXPECT_FLOAT_EQ(expected.values[2], toTest.values[2]);
    EXPECT_FLOAT_EQ(expected.values[3], toTest.values[3]);
}

TEST_F(TestQuaternion, BetweenVectors)
{
    auto vector1 = Vector3{1.0f,0.0f,0.0f};
    auto vector2 = Vector3{0.0f,1.0f,0.0f};

    auto toTest = (Quaternion{vector1, vector2}).ToVector();

    // Rotate about the z axis (0,0,1) by 90 degrees. Dunno why it normalises
    // to (0,0,inverseRoot2,inverseRoot2) but it does.
    auto inverseRoot2 = 1.0f / std::sqrt(2.0f);

    EXPECT_FLOAT_EQ(0.0f, toTest.values[0]);
    EXPECT_FLOAT_EQ(0.0f, toTest.values[1]);
    EXPECT_FLOAT_EQ(inverseRoot2, toTest.values[2]);
    EXPECT_FLOAT_EQ(inverseRoot2, toTest.values[3]);
}

TEST_F(TestQuaternion, Equal)
{
    auto toTest1 = Quaternion{1.0f, 2.0f, 3.0f, 4.0f};
    auto toTest2 = Quaternion{1.0f, 2.0f, 3.0f, 4.0f};

    EXPECT_EQ(toTest1, toTest2);
}

TEST_F(TestQuaternion, NotEqual)
{
    auto toTest1 = Quaternion{1.0f, 2.0f, 3.0f, 4.0f};
    auto toTest2 = Quaternion{1.0f, 2.0f, 2.0f, 4.0f};
    auto toTest3 = Quaternion{1.0f, 2.0f, 3.0f, 4.1f};

    EXPECT_NE(toTest1, toTest2);
    EXPECT_NE(toTest1, toTest3);
}

TEST_F(TestQuaternion, Multiply)
{
    auto angle  = Radians{2.0f};
    auto angleN = Radians{-2.0f};
    auto vector = Normalise(Vector3{1,2,3});
    auto original = Quaternion{vector, angle};
    auto otherWay = Quaternion{vector, angleN};

    auto toTest = (original * otherWay).ToVector();

    // do Quaternion maths here to verify
    auto a = original.ToVector();
    auto b = otherWay.ToVector();

    auto expected = Vector
    {{{
        (a.values[0] * b.values[0]) - (a.values[0] * b.values[1]) - (a.values[0] * b.values[2]) - (a.values[0] * b.values[3]),
        (a.values[1] * b.values[1]) + (a.values[1] * b.values[0]) + (a.values[1] * b.values[3]) - (a.values[1] * b.values[2]),
        (a.values[2] * b.values[2]) - (a.values[2] * b.values[3]) + (a.values[2] * b.values[0]) + (a.values[2] * b.values[1]),
        (a.values[3] * b.values[3]) + (a.values[3] * b.values[2]) - (a.values[3] * b.values[1]) + (a.values[3] * b.values[0])
    }}};

    EXPECT_FLOAT_EQ(expected.values[0], toTest.values[0]);
    EXPECT_FLOAT_EQ(expected.values[1], toTest.values[1]);
    EXPECT_FLOAT_EQ(expected.values[2], toTest.values[2]);
    EXPECT_FLOAT_EQ(expected.values[3], toTest.values[3]);
}

}}} // namespace

