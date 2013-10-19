
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

#include <Implementation/FastVectorGeneral.hpp>
#include <Implementation/Vector.hpp>

// RAM: DEBUG
#include <Implementation/Vector4.hpp>
#include <Implementation/QuaternionCpp.hpp>

#include <gmock/gmock.h>

#include <vector>

using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

template <typename T>
class TestVectorFast : public ::testing::Test
{
public:
    typedef T Vec;

    const static uint ArraySize = 100;

    TestVectorFast()
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

typedef ::testing::Types<FastVectorGeneral> TestVectorFastTypes;
TYPED_TEST_CASE(TestVectorFast, TestVectorFastTypes);

TYPED_TEST(TestVectorFast, Empty)
{
    auto toTest = Save(typename TestFixture::Vec(0, typename TestFixture::Vec::tagReplicate{}));

    // Shouldn't crash.
    EXPECT_FLOAT_EQ(0.0f, X(toTest));
    EXPECT_FLOAT_EQ(0.0f, Y(toTest));
    EXPECT_FLOAT_EQ(0.0f, Z(toTest));
    EXPECT_FLOAT_EQ(0.0f, W(toTest));
}

TYPED_TEST(TestVectorFast, Equal)
{
    auto a = Save(typename TestFixture::Vec(-1.0f, 0.0f, 1.0f, 123456789.0f));
    auto b = Save(typename TestFixture::Vec(-1.0f, 0.0f, 1.0f, 123456789.0f));

    EXPECT_EQ(a, b);
    EXPECT_FLOAT_EQ(X(a), X(b));
    EXPECT_FLOAT_EQ(Y(a), Y(b));
    EXPECT_FLOAT_EQ(Z(a), Z(b));
    EXPECT_FLOAT_EQ(W(a), W(b));
}

TYPED_TEST(TestVectorFast, NotEqual)
{
    auto a = Save(typename TestFixture::Vec(-1.0f, 0.0f, 1.0f, 123456789.0f));
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

TYPED_TEST(TestVectorFast, Increment)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);

        // RAM: TODO: Test saving the values when inc/dec them.
        a++;
        ++b;

        auto result = Save(a);
        auto result2 = Save(b);

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) + 1.0f) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) + 1.0f);
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) + 1.0f);
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) + 1.0f);

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupB[i]) + 1.0f);
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupB[i]) + 1.0f);
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupB[i]) + 1.0f);
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupB[i]) + 1.0f);
    }
}


TYPED_TEST(TestVectorFast, Decrement)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);

        // RAM: TODO: Test saving the values when inc/dec them.
        a--;
        --b;

        auto result = Save(a);
        auto result2 = Save(b);

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) - 1.0f) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) - 1.0f);
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) - 1.0f);
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) - 1.0f);

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupB[i]) - 1.0f);
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupB[i]) - 1.0f);
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupB[i]) - 1.0f);
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupB[i]) - 1.0f);
    }
}

TYPED_TEST(TestVectorFast, Add)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = Save(a + b);
        c += a;
        auto result2 = Save(c);

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) + X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) + Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) + Z(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) + W(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) + X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) + Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) + Z(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupC[i]) + W(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVectorFast, Subtract)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = Save(a - b);
        c -= a;
        auto result2 = Save(c);

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) - X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) - Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) - Z(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) - W(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) - X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) - Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) - Z(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupC[i]) - W(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVectorFast, Multiply)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = Save(a * b);
        c *= a;
        auto result2 = Save(c);

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) * X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) * W(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) * X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) * Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) * Z(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupC[i]) * W(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVectorFast, Divide)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = Save(a / b);
        c /= a;
        auto result2 = Save(c);

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) / X(TestFixture::groupB[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) / Y(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) / Z(TestFixture::groupB[i]));
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) / W(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupC[i]) / X(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupC[i]) / Y(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupC[i]) / Z(TestFixture::groupA[i]));
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupC[i]) / W(TestFixture::groupA[i]));
    }
}

TYPED_TEST(TestVectorFast, Mad)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);

        auto result = Save((a*b) + c);
        auto d = c;
        d += a * b;
        auto result2 = Save(d);

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) * X(TestFixture::groupB[i]) + X(TestFixture::groupC[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i]) + Y(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]) + Z(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) * W(TestFixture::groupB[i]) + W(TestFixture::groupC[i]));

        ASSERT_FLOAT_EQ(X(result2), X(TestFixture::groupA[i]) * X(TestFixture::groupB[i]) + X(TestFixture::groupC[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result2), Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i]) + Y(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(Z(result2), Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]) + Z(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(W(result2), W(TestFixture::groupA[i]) * W(TestFixture::groupB[i]) + W(TestFixture::groupC[i]));
    }
}

TYPED_TEST(TestVectorFast, FnMad)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto c = typename TestFixture::Vec(TestFixture::groupC[i]);
        auto result = Save(Mad(a, b, c));

        ASSERT_FLOAT_EQ(X(result), X(TestFixture::groupA[i]) * X(TestFixture::groupB[i]) + X(TestFixture::groupC[i])) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i]) + Y(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(Z(result), Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i]) + Z(TestFixture::groupC[i]));
        ASSERT_FLOAT_EQ(W(result), W(TestFixture::groupA[i]) * W(TestFixture::groupB[i]) + W(TestFixture::groupC[i]));
    }
}

TYPED_TEST(TestVectorFast, FnSqrt)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        // Don't bother if anything is negative.
        if  (
                (X(TestFixture::groupA[i]) >= 0) &&
                (Y(TestFixture::groupA[i]) >= 0) &&
                (Z(TestFixture::groupA[i]) >= 0) &&
                (W(TestFixture::groupA[i]) >= 0)
             )
        {
            auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
            auto result = Save(Sqrt(a));

            ASSERT_FLOAT_EQ(X(result), sqrt(X(TestFixture::groupA[i]))) << " i: " << i;
            ASSERT_FLOAT_EQ(Y(result), sqrt(Y(TestFixture::groupA[i])));
            ASSERT_FLOAT_EQ(Z(result), sqrt(Z(TestFixture::groupA[i])));
            ASSERT_FLOAT_EQ(W(result), sqrt(W(TestFixture::groupA[i])));
        }
    }
}

TYPED_TEST(TestVectorFast, FnDot)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto b = typename TestFixture::Vec(TestFixture::groupB[i]);
        auto result = Save(Dot(a, b));
        auto dot =
                (X(TestFixture::groupA[i]) * X(TestFixture::groupB[i])) +
                (Y(TestFixture::groupA[i]) * Y(TestFixture::groupB[i])) +
                (Z(TestFixture::groupA[i]) * Z(TestFixture::groupB[i])) +
                (W(TestFixture::groupA[i]) * W(TestFixture::groupB[i]));

        ASSERT_FLOAT_EQ(X(result), dot) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), dot);
        ASSERT_FLOAT_EQ(Z(result), dot);
        ASSERT_FLOAT_EQ(W(result), dot);
    }
}

TYPED_TEST(TestVectorFast, FnLength)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = Save(Length(a));
        auto l =
                sqrt((X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
                     (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
                     (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])) +
                     (W(TestFixture::groupA[i]) * W(TestFixture::groupA[i])));

        ASSERT_FLOAT_EQ(X(result), l) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), l);
        ASSERT_FLOAT_EQ(Z(result), l);
        ASSERT_FLOAT_EQ(W(result), l);
    }
}

TYPED_TEST(TestVectorFast, FnLengthSquared)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = Save(Length(a));
        auto l =
                sqrt((X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
                     (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
                     (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])) +
                     (W(TestFixture::groupA[i]) * W(TestFixture::groupA[i])));



        ASSERT_FLOAT_EQ(X(result), l) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), l);
        ASSERT_FLOAT_EQ(Z(result), l);
        ASSERT_FLOAT_EQ(W(result), l);
    }
}

TYPED_TEST(TestVectorFast, FnNormaliseFast)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = Save(Normalise(a));
        auto l = sqrt(
            (X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
            (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
            (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])) +
            (W(TestFixture::groupA[i]) * W(TestFixture::groupA[i])));

        auto answer = Vector{{{
                X(TestFixture::groupA[i]) / l,
                Y(TestFixture::groupA[i]) / l,
                Z(TestFixture::groupA[i]) / l,
                W(TestFixture::groupA[i]) / l}}};

        ASSERT_FLOAT_EQ(X(result), X(answer)) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(answer));
        ASSERT_FLOAT_EQ(Z(result), Z(answer));
        ASSERT_FLOAT_EQ(W(result), W(answer));
    }
}

TYPED_TEST(TestVectorFast, FnNormaliseAccurate)
{
    auto asize = TestFixture::groupA.size();

    for (uint i = 0 ; i < asize; ++i)
    {
        auto a = typename TestFixture::Vec(TestFixture::groupA[i]);
        auto result = Save(Normalise(a, typename TestFixture::Vec::tagAccurate{}));
        auto l = sqrt(
            (X(TestFixture::groupA[i]) * X(TestFixture::groupA[i])) +
            (Y(TestFixture::groupA[i]) * Y(TestFixture::groupA[i])) +
            (Z(TestFixture::groupA[i]) * Z(TestFixture::groupA[i])) +
            (W(TestFixture::groupA[i]) * W(TestFixture::groupA[i])));

        auto answer = Vector{{{
                X(TestFixture::groupA[i]) / l,
                Y(TestFixture::groupA[i]) / l,
                Z(TestFixture::groupA[i]) / l,
                W(TestFixture::groupA[i]) / l}}};

        ASSERT_FLOAT_EQ(X(result), X(answer)) << " i: " << i;
        ASSERT_FLOAT_EQ(Y(result), Y(answer));
        ASSERT_FLOAT_EQ(Z(result), Z(answer));
        ASSERT_FLOAT_EQ(W(result), W(answer));
    }
}

TYPED_TEST(TestVectorFast, Benchmark)
{
    using Vec4          = typename TestFixture::Vec;
    using tagReplicate  = typename Vec4::tagReplicate;

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
                auto a = Vec4(TestFixture::groupA[i]);
                auto b = Vec4(TestFixture::groupB[i]);
                auto c = Vec4(TestFixture::groupC[i]);
                auto finalVel = Mad(
                            Normalise(a),
                            Vec4(5, tagReplicate{}),
                            b + c);

                TestFixture::groupA[j] = Save(a + (finalVel /  Vec4(60,60,60,60)));
            }
        }
    }
}

}}} // namespace
