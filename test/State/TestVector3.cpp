
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

// RAM: TODO: Testing which vector I can actually test.
#include <Implementation/Vector3cpp.hpp>
#include <Implementation/Vector3sse2.hpp>
//#include <Implementation/Vector3neon.hpp>

#include <gmock/gmock.h>

#include <vector>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

template <typename T>
class TestVector3 : public ::testing::Test
{
public:
    typedef T Vec;

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

    std::vector<Vector> groupA;
    std::vector<Vector> groupB;
    std::vector<Vector> groupC;
};

typedef ::testing::Types<Vector3cpp, Vector3sse2> TestVector3Types;
TYPED_TEST_CASE(TestVector3, TestVector3Types);

TYPED_TEST(TestVector3, Empty)
{
    auto toTest = (typename TestFixture::Vec(0, typename TestFixture::Vec::tagReplicate{})).ToVector();

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, X(toTest));
    EXPECT_FLOAT_EQ(0.0f, Y(toTest));
    EXPECT_FLOAT_EQ(0.0f, Z(toTest));
    EXPECT_FLOAT_EQ(0.0f, W(toTest));
}

TYPED_TEST(TestVector3, Equal)
{
    auto a = (typename TestFixture::Vec(-1.0f, 0.0f, 1.0f)).ToVector();
    auto b = (typename TestFixture::Vec(-1.0f, 0.0f, 1.0f)).ToVector();

    EXPECT_EQ(a, b);
    EXPECT_FLOAT_EQ(X(a), X(b));
    EXPECT_FLOAT_EQ(Y(a), Y(b));
    EXPECT_FLOAT_EQ(Z(a), Z(b));
}

TYPED_TEST(TestVector3, NotEqual)
{
    auto a = (typename TestFixture::Vec(-1.0f, 0.0f, 1.0f)).ToVector();
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

TYPED_TEST(TestVector3, Increment)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);

        // RAM: TODO: Test saving the values when inc/dec them.
        a++;
        ++b;

        auto result = (a).ToVector();
        auto result2 = (b).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) + 1.0f) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) + 1.0f);
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) + 1.0f);

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupB[i]) + 1.0f);
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupB[i]) + 1.0f);
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupB[i]) + 1.0f);
    }
}


TYPED_TEST(TestVector3, Decrement)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);

        // RAM: TODO: Test saving the values when inc/dec them.
        a--;
        --b;

        auto result = (a).ToVector();
        auto result2 = (b).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) - 1.0f) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) - 1.0f);
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) - 1.0f);

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupB[i]) - 1.0f);
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupB[i]) - 1.0f);
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupB[i]) - 1.0f);
    }
}

TYPED_TEST(TestVector3, Add)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = (a + b).ToVector();
        c += a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) + X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) + Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) + Z(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) + X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) + Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) + Z(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVector3, Subtract)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = (a - b).ToVector();
        c -= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) - X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) - Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) - Z(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) - X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) - Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) - Z(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVector3, Multiply)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = (a * b).ToVector();
        c *= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) * X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) * X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) * Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) * Z(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVector3, Divide)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = (a / b).ToVector();
        c /= a;
        auto result2 = (c).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) / X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) / Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) / Z(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) / X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) / Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) / Z(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVector3, Mad)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = ((a*b) + c).ToVector();
        auto d = c;
        d += a * b;
        auto result2 = (d).ToVector();

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) * X(TestFixture::groupB[i]) + X(TestFixture::groupC[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i]) + Y(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]) + Z(TestFixture::groupC[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupA[i]) * X(TestFixture::groupB[i]) + X(TestFixture::groupC[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i]) + Y(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]) + Z(TestFixture::groupC[i]));
    }
}

TYPED_TEST(TestVector3, FnSqrt)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        // Don't bother if anything is negative.
        if  (
                (X(TestFixture::groupA[i]) >= 0) &&
                (Y(TestFixture::groupA[i]) >= 0) &&
                (Z(TestFixture::groupA[i]) >= 0)
             )
        {
            auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
            auto result = (Sqrt(a)).ToVector();

            ASSERT_FLOAT_EQ(X(result), sqrt(X(TestFixture::groupA[i]))) << " i: " << i;
            ASSERT_FLOAT_EQ(Y(result), sqrt(Y(TestFixture::groupA[i])));
            ASSERT_FLOAT_EQ(Z(result), sqrt(Z(TestFixture::groupA[i])));
        }
    }
}

TYPED_TEST(TestVector3, FnDot)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto result = (Dot(a, b)).ToVector();
        auto dot =
                (X(TestFixture::groupA[i]) * X(TestFixture::groupB[i])) +
                (Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i])) +
                (Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result), dot) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), dot);
        ASSERT_FLOAT_EQ(Z(result), dot);
    }
}

TYPED_TEST(TestVector3, FnLength)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = (Length(a)).ToVector();
        auto l =
                sqrt((X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
                     (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
                     (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])));

        ASSERT_FLOAT_EQ(X(result), l) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), l);
        ASSERT_FLOAT_EQ(Z(result), l);
    }
}

TYPED_TEST(TestVector3, FnLengthSquared)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = (Length(a)).ToVector();
        auto l =
                sqrt((X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
                     (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
                     (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])));



        ASSERT_FLOAT_EQ(X(result), l) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), l);
        ASSERT_FLOAT_EQ(Z(result), l);
    }
}

TYPED_TEST(TestVector3, FnNormaliseFast)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = (Normalise(a)).ToVector();
        auto l = sqrt(
            (X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
            (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
            (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])));

        auto answer = Vector{{{
                X(TestFixture::groupA[i]) / l,
                Y(TestFixture::groupA[i]) / l,
                Z(TestFixture::groupA[i]) / l}}};

        ASSERT_FLOAT_EQ(X(result), X(answer)) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(answer));
        ASSERT_FLOAT_EQ(Z(result), Z(answer));
    }
}

TYPED_TEST(TestVector3, FnNormaliseAccurate)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = (NormaliseStable(a)).ToVector();
        auto l = sqrt(
            (X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
            (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
            (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])));

        auto answer = Vector{{{
                X(TestFixture::groupA[i]) / l,
                Y(TestFixture::groupA[i]) / l,
                Z(TestFixture::groupA[i]) / l}}};

        ASSERT_FLOAT_EQ(X(result), X(answer)) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(answer));
        ASSERT_FLOAT_EQ(Z(result), Z(answer));
        ASSERT_FLOAT_EQ(W(result), W(answer));
    }
}

TYPED_TEST(TestVector3, Benchmark)
{
    using Vec3          = typename TestFixture::Vec;
    using tagReplicate  = typename Vec3::tagReplicate;

    auto asize = TestFixture::groupA.size();

    if (asize > 50)
    {
        asize = 50;
    }

    for (uint k = 0 ; k < 100; ++k)
    {
        for (uint i = 0 ; i < asize; ++i)
        {
            for (uint j = 0 ; j < asize; ++j)
            {
                // Normalise an angle, by speed, add some other velocities, add it to a position
                // via a time step.
                auto a = Vec3(TestFixture::groupA[i]);
                auto b = Vec3(TestFixture::groupB[i]);
                auto c = Vec3(TestFixture::groupC[i]);
                auto finalVel = Normalise(a) * Vec3(5, tagReplicate{}) + b + c;

                TestFixture::groupA[j] = (a + (finalVel /  Vec3(60,60,60))).ToVector();
            }
        }
    }
}

}}} // namespace
