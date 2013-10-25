/*
    Game-in-a-box. Simple uint0 Person Shooter Network Game.
    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>

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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <Implementation/DeltaStateGameSnapshot.hpp>
#include <Implementation/Types.hpp>

#include <Common/UnitOperators.hpp>
#include <Common/MathConstants.hpp>

#include <gtest/gtest.h>

#include <vector>
#include <random>
#include <functional>
#include <limits>

using namespace std;
using namespace GameInABox::Common;
using namespace GameInABox::Network;

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Test Class
// /////////////////////
class TestDeltaStateGameSnapshot : public ::testing::Test
{
public:
    TestDeltaStateGameSnapshot()
        : ::testing::Test()
        , myGenerator()
        , myAngles(-Tau(), Tau())
        , myFlagsPlayer(0, static_cast<uint>(FlagsPlayer::MaxValue))
        , myFlagsMissle(0, static_cast<uint>(FlagsMissle::MaxValue))
        , myFlagsMode(0, static_cast<uint>(StateMode::MaxValue))
        , myRandomUint32(0, std::numeric_limits<uint32_t>::max())
    {
    }

    virtual void SetUp()
    {
    }

protected:
    static const int N = 8;
    static constexpr float Tolerance{0.004f};

    std::array<Research, 4> myResearch =
    {{
        {false, false},
        {false, true},
        {true, false},
        {true, true}
    }};

    minstd_rand myGenerator;
    uniform_real_distribution<float> myAngles;
    uniform_int_distribution<uint32_t> myFlagsPlayer;
    uniform_int_distribution<uint32_t> myFlagsMissle;
    uniform_int_distribution<uint32_t> myFlagsMode;
    uniform_int_distribution<uint32_t> myRandomUint32;

    void TestN(
            std::function<StateGameSnapshot()> baseGetter,
            std::function<StateGameSnapshot()> targetGetter,
            unsigned count,
            float floatTolerance);

    StateGameSnapshot GameStateRandom();
};


// /////////////////////
// Helpers
// /////////////////////
void TestDeltaStateGameSnapshot::TestN(
        std::function<StateGameSnapshot()> baseGetter,
        std::function<StateGameSnapshot()> targetGetter,
        unsigned count,
        float floatTolerance)
{
    for (auto setting : myResearch)
    {
        DeltaStateGameSnapshot toTest(setting);

        for (unsigned i = 0; i < count; ++i)
        {
            auto target = targetGetter();
            auto base = baseGetter();

            auto coded = toTest(base, target);
            auto decoded = toTest(base, coded);

            // now test!

            // first the flags
            ASSERT_EQ(decoded.mode, target.mode) << " i: " << i;

            for (uint i = 0; i < decoded.players.size(); ++i)
            {
                auto& playerTarget = target.players[i];
                auto& playerDecoded = decoded.players[i];

                ASSERT_NEAR(playerDecoded.position.values[0], playerTarget.position.values[0], floatTolerance) << " i: " << i;
                ASSERT_NEAR(playerDecoded.position.values[1], playerTarget.position.values[1], floatTolerance);
                ASSERT_NEAR(playerDecoded.position.values[2], playerTarget.position.values[2], floatTolerance);

                ASSERT_EQ(playerDecoded.lookAndDo.flags, playerTarget.lookAndDo.flags);

                ASSERT_NEAR(playerDecoded.lookAndDo.orientation.values[0], playerTarget.lookAndDo.orientation.values[0], floatTolerance) << " i: " << i;
                ASSERT_NEAR(playerDecoded.lookAndDo.orientation.values[1], playerTarget.lookAndDo.orientation.values[1], floatTolerance);
                ASSERT_NEAR(playerDecoded.lookAndDo.orientation.values[2], playerTarget.lookAndDo.orientation.values[2], floatTolerance);

                ASSERT_NEAR(playerDecoded.jetDirection.values[0], playerTarget.jetDirection.values[0], floatTolerance) << " i: " << i;
                ASSERT_NEAR(playerDecoded.jetDirection.values[1], playerTarget.jetDirection.values[1], floatTolerance);
                ASSERT_NEAR(playerDecoded.jetDirection.values[2], playerTarget.jetDirection.values[2], floatTolerance);

                ASSERT_EQ(playerDecoded.health, playerTarget.health);
                ASSERT_EQ(playerDecoded.energy, playerTarget.energy);
            }

            for (uint i = 0; i < decoded.missles.size(); ++i)
            {
                auto& missleTarget = target.missles[i];
                auto& missleDecoded = decoded.missles[i];

                ASSERT_NEAR(missleDecoded.source.values[0], missleTarget.source.values[0], floatTolerance);
                ASSERT_NEAR(missleDecoded.source.values[1], missleTarget.source.values[1], floatTolerance);
                ASSERT_NEAR(missleDecoded.source.values[2], missleTarget.source.values[2], floatTolerance);

                ASSERT_NEAR(missleDecoded.orientation.values[0], missleTarget.orientation.values[0], floatTolerance);
                ASSERT_NEAR(missleDecoded.orientation.values[1], missleTarget.orientation.values[1], floatTolerance);
                ASSERT_NEAR(missleDecoded.orientation.values[2], missleTarget.orientation.values[2], floatTolerance);

                ASSERT_EQ(missleDecoded.lastAction, missleTarget.lastAction);
                ASSERT_EQ(missleDecoded.flags, missleTarget.flags);
            }

            for (uint i = 0; i < decoded.playerNames.size(); ++i)
            {
                ASSERT_EQ(decoded.playerNames[i], target.playerNames[i]);
            }
        }
    }
}

StateGameSnapshot TestDeltaStateGameSnapshot::GameStateRandom()
{
    StateGameSnapshot result;

    for (auto& player : result.players)
    {
        player =
        {
            0,
            {{{
                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator),
                0.0f
            }}},
            {
                {{{
                    myAngles(myGenerator),
                    myAngles(myGenerator),
                    myAngles(myGenerator),
                    0.0f
                }}},
                static_cast<FlagsPlayer>(myFlagsPlayer(myGenerator))
            },            
            {{{
                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator),
                0.0f
            }}},
            myRandomUint32(myGenerator) % 1024,
            myRandomUint32(myGenerator) % 1024,
            0,
            0,
            0
        };
    }

    for (auto& missle : result.missles)
    {
        missle =
        {
            0,
            0,
            {{{
                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator),
                0.0f
            }}},
            {{{
                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator),
                0.0f
            }}},
            myRandomUint32(myGenerator),
            static_cast<FlagsMissle>(myFlagsMissle(myGenerator))
        };
    }

    result.mode = static_cast<StateMode>(myFlagsMode(myGenerator));

    for (auto& name : result.playerNames)
    {
        for (auto& namePoint: name)
        {
            namePoint = static_cast<uint8_t>(myRandomUint32(myGenerator));
        }
    }

    return result;
}


StateGameSnapshot GameStateZero()
{
    auto vectorZero = Vector{{{0.0f}}};

    auto basePlayer = StatePlayer
    {
        0,
        vectorZero,
        {vectorZero, FlagsPlayer::Default},
        vectorZero,
        0,
        0,
        0,
        0,
        0
    };

    auto baseMissle = StateMissle
    {
        0,
        0,
        vectorZero,
        vectorZero,
        0,
        FlagsMissle::Default
    };

    return StateGameSnapshot
    {
        {{basePlayer}},
        {{baseMissle}},
        StateMode::Default,
        {{{" "}}}
    };
}

// /////////////////////
// Tests
// /////////////////////
TEST_F(TestDeltaStateGameSnapshot, RandomN)
{
    myGenerator.seed(1);

    // 0.001 ==
    // One bit worth is what the error is allowed to be.
    // One bit has a delta range of the inverse m value.
    // But since we don't know that info, lets just assume 3dp

    TestN([&]()
    {
        return GameStateRandom();
    },
    [&]()
    {
        return GameStateRandom();
    },
    N,
    Tolerance);
}

TEST_F(TestDeltaStateGameSnapshot, RandomNFromZeroIdentity)
{
    TestN(
        GameStateZero,
        [&]()
        {
            return GameStateRandom();
        },
        N,
        Tolerance);
}


TEST_F(TestDeltaStateGameSnapshot, RandomOnceFromZeroIdentity)
{
    TestN(
        GameStateZero,
        [&]()
        {
            return GameStateRandom();
        },
        1,
        Tolerance);
}

TEST_F(TestDeltaStateGameSnapshot, ZeroToZero)
{
    TestN(
        GameStateZero,
        GameStateZero,
        1,
        Tolerance);
}

TEST_F(TestDeltaStateGameSnapshot, Mode)
{
    auto target = GameStateZero();

    // Test only the game mode.
    target.mode = StateMode::Playing;

    TestN(
        GameStateZero,
        [&]()
        {
            return target;
        },
        1,
        Tolerance);
}

}}} // namespace
