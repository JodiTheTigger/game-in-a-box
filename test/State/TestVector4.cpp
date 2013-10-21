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
    auto toTest = (Vector4(0, Vector4::tagReplicate{})).ToVector();

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, X(toTest));
    EXPECT_FLOAT_EQ(0.0f, Y(toTest));
    EXPECT_FLOAT_EQ(0.0f, Z(toTest));
    EXPECT_FLOAT_EQ(0.0f, W(toTest));
}

TEST_F(TestVector4, Equal)
{
    auto a = (Vector4(-1.0f, 0.0f, 1.0f)).ToVector();
    auto b = (Vector4(-1.0f, 0.0f, 1.0f)).ToVector();

    EXPECT_EQ(a, b);
    EXPECT_FLOAT_EQ(X(a), X(b));
    EXPECT_FLOAT_EQ(Y(a), Y(b));
    EXPECT_FLOAT_EQ(Z(a), Z(b));
    EXPECT_FLOAT_EQ(W(a), W(b));
}

TEST_F(TestVector4, NotEqual)
{
    auto a = (Vector4(-1.0f, 0.0f, 1.0f)).ToVector();
    auto b = std::vector<Vector>{};

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

TEST_F(TestVector4, Add)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto b = Vector4(groupB[i]);
        auto c = Vector4(groupC[i]);

        auto result = (a + b).ToVector();
        c += a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(groupA[i]) + X(groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(groupA[i]) + Y(groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(groupA[i]) + Z(groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(groupA[i]) + W(groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(groupC[i]) + X(groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(groupC[i]) + Y(groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(groupC[i]) + Z(groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(groupC[i]) + W(groupA[i]));
    }
}

TEST_F(TestVector4, Subtract)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto b = Vector4(groupB[i]);
        auto c = Vector4(groupC[i]);

        auto result = (a - b).ToVector();
        c -= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(groupA[i]) - X(groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(groupA[i]) - Y(groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(groupA[i]) - Z(groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(groupA[i]) - W(groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(groupC[i]) - X(groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(groupC[i]) - Y(groupA[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(groupA[i]) - Z(groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(groupA[i]) - W(groupB[i]));
    }
}

TEST_F(TestVector4, Multiply)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto b = Vector4(groupB[i]);
        auto c = Vector4(groupC[i]);

        auto result = (a * b).ToVector();
        c *= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(groupA[i]) * X(groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(groupA[i]) * Y(groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(groupA[i]) * Z(groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(groupA[i]) * W(groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(groupC[i]) * X(groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(groupC[i]) * Y(groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(groupC[i]) * Z(groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(groupC[i]) * W(groupA[i]));
    }
}

TEST_F(TestVector4, Divide)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto b = Vector4(groupB[i]);
        auto c = Vector4(groupC[i]);

        auto result = (a / b).ToVector();
        c /= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(groupA[i]) / X(groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(groupA[i]) / Y(groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(groupA[i]) / Z(groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(groupA[i]) / W(groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(groupC[i]) / X(groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(groupC[i]) / Y(groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(groupC[i]) / Z(groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(groupC[i]) / W(groupA[i]));
    }
}

TEST_F(TestVector4, Mad)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto b = Vector4(groupB[i]);
        auto c = Vector4(groupC[i]);

        auto result = ((a*b) + c).ToVector();
        auto d = c;
        d += a * b;
        auto result2 = (d).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(groupA[i]) * X(groupB[i]) + X(groupC[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(groupA[i]) * Y(groupB[i]) + Y(groupC[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(groupA[i]) * Z(groupB[i]) + Z(groupC[i]));
        ASSERT_FLOAT_EQ(W(result), W(groupA[i]) * W(groupB[i]) + W(groupC[i]));

        ASSERT_FLOAT_EQ(X(result2), X(groupA[i]) * X(groupB[i]) + X(groupC[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result2), Y(groupA[i]) * Y(groupB[i]) + Y(groupC[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(groupA[i]) * Z(groupB[i]) + Z(groupC[i]));
        ASSERT_FLOAT_EQ(W(result2), W(groupA[i]) * W(groupB[i]) + W(groupC[i]));
    }
}

TEST_F(TestVector4, FnSqrt)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        // Don't bother if anything is negative.
        if  (
                (X(groupA[i]) >= 0) &&
                (Y(groupA[i]) >= 0) &&
                (Z(groupA[i]) >= 0) &&
                (W(groupA[i]) >= 0)
             )
        {
            auto a = Vector4(groupA[i]);
            auto result = (Sqrt(a)).ToVector();

            ASSERT_FLOAT_EQ(X(result), sqrt(X(groupA[i]))) << " i: " << i;
            ASSERT_FLOAT_EQ(Y(result), sqrt(Y(groupA[i])));
            ASSERT_FLOAT_EQ(Z(result), sqrt(Z(groupA[i])));
            ASSERT_FLOAT_EQ(W(result), sqrt(W(groupA[i])));
        }
    }
}

TEST_F(TestVector4, FnDot)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto b = Vector4(groupB[i]);
        auto result = (Dot(a, b)).ToVector();
        auto dot =
                (X(groupA[i]) * X(groupB[i])) +
                (Y(groupA[i]) * Y(groupB[i])) +
                (Z(groupA[i]) * Z(groupB[i])) +
                (W(groupA[i]) * W(groupB[i]));

        ASSERT_FLOAT_EQ(X(result), dot) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), dot);
        ASSERT_FLOAT_EQ(Z(result), dot);
        ASSERT_FLOAT_EQ(W(result), dot);
    }
}

TEST_F(TestVector4, FnLength)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto result = (Length(a)).ToVector();
        auto l =
                sqrt((X(groupA[i]) * X(groupA[i])) +
                     (Y(groupA[i]) * Y(groupA[i])) +
                     (Z(groupA[i]) * Z(groupA[i])) +
                     (W(groupA[i]) * W(groupA[i])));

        ASSERT_FLOAT_EQ(X(result), l) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), l);
        ASSERT_FLOAT_EQ(Z(result), l);
        ASSERT_FLOAT_EQ(W(result), l);
    }
}

TEST_F(TestVector4, FnLengthSquared)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto result = (Length(a)).ToVector();
        auto l =
                sqrt((X(groupA[i]) * X(groupA[i])) +
                     (Y(groupA[i]) * Y(groupA[i])) +
                     (Z(groupA[i]) * Z(groupA[i])) +
                     (W(groupA[i]) * W(groupA[i])));



        ASSERT_FLOAT_EQ(X(result), l) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), l);
        ASSERT_FLOAT_EQ(Z(result), l);
        ASSERT_FLOAT_EQ(W(result), l);
    }
}

TEST_F(TestVector4, FnNormaliseFast)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto result = (Normalise(a)).ToVector();
        auto l = sqrt(
            (X(groupA[i]) * X(groupA[i])) +
            (Y(groupA[i]) * Y(groupA[i])) +
            (Z(groupA[i]) * Z(groupA[i])) +
            (W(groupA[i]) * W(groupA[i])));

        auto answer = Vector{{{
                X(groupA[i]) / l,
                Y(groupA[i]) / l,
                Z(groupA[i]) / l,
                W(groupA[i]) / l}}};

        ASSERT_FLOAT_EQ(X(result), X(answer)) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(answer));
        ASSERT_FLOAT_EQ(Z(result), Z(answer));
        ASSERT_FLOAT_EQ(W(result), W(answer));
    }
}

TEST_F(TestVector4, FnNormaliseAccurate)
{
    auto asize = groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = Vector4(groupA[i]);
        auto result = (NormaliseStable(a)).ToVector();
        auto l = sqrt(
            (X(groupA[i]) * X(groupA[i])) +
            (Y(groupA[i]) * Y(groupA[i])) +
            (Z(groupA[i]) * Z(groupA[i])) +
            (W(groupA[i]) * W(groupA[i])));

        auto answer = Vector{{{
                X(groupA[i]) / l,
                Y(groupA[i]) / l,
                Z(groupA[i]) / l,
                W(groupA[i]) / l}}};

        ASSERT_FLOAT_EQ(X(result), X(answer)) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(answer));
        ASSERT_FLOAT_EQ(Z(result), Z(answer));
        ASSERT_FLOAT_EQ(W(result), W(answer));
    }
}

}}} // namespace

