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

    void TestN(std::function<StateGameSnapshot()> baseGetter, unsigned count);
    StateGameSnapshot RandomGameState();
};


// /////////////////////
// Helpers
// /////////////////////
void TestDeltaStateGameSnapshot::TestN(std::function<StateGameSnapshot()> baseGetter, unsigned count)
{
    for (auto setting : myResearch)
    {
        DeltaStateGameSnapshot toTest(setting);

        for (unsigned i = 0; i < count; ++i)
        {
            auto target = RandomGameState();
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

                // One bit worth is what the error is allowed to be.
                // One bit has a delta range of the inverse m value.
                // But since we don't know that info, lets just assume 3dp
                ASSERT_NEAR(playerDecoded.position.x, playerTarget.position.x, 0.001) << " i: " << i;
                ASSERT_NEAR(playerDecoded.position.y, playerTarget.position.y, 0.001);
                ASSERT_NEAR(playerDecoded.position.z, playerTarget.position.z, 0.001);

                ASSERT_EQ(playerDecoded.lookAndDo.flags, playerTarget.lookAndDo.flags);

                ASSERT_NEAR(playerDecoded.lookAndDo.orientation.x, playerTarget.lookAndDo.orientation.x, 0.001) << " i: " << i;
                ASSERT_NEAR(playerDecoded.lookAndDo.orientation.y, playerTarget.lookAndDo.orientation.y, 0.001);
                ASSERT_NEAR(playerDecoded.lookAndDo.orientation.z, playerTarget.lookAndDo.orientation.z, 0.001);

                ASSERT_NEAR(playerDecoded.jetDirection.x, playerTarget.jetDirection.x, 0.001) << " i: " << i;
                ASSERT_NEAR(playerDecoded.jetDirection.y, playerTarget.jetDirection.y, 0.001);
                ASSERT_NEAR(playerDecoded.jetDirection.z, playerTarget.jetDirection.z, 0.001);

                ASSERT_EQ(playerDecoded.health, playerTarget.health);
                ASSERT_EQ(playerDecoded.energy, playerTarget.energy);
            }

            for (uint i = 0; i < decoded.missles.size(); ++i)
            {
                auto& missleTarget = target.missles[i];
                auto& missleDecoded = decoded.missles[i];

                ASSERT_NEAR(missleDecoded.source.x, missleTarget.source.x, 0.001);
                ASSERT_NEAR(missleDecoded.source.y, missleTarget.source.y, 0.001);
                ASSERT_NEAR(missleDecoded.source.z, missleTarget.source.z, 0.001);

                ASSERT_NEAR(missleDecoded.orientation.x, missleTarget.orientation.x, 0.001);
                ASSERT_NEAR(missleDecoded.orientation.y, missleTarget.orientation.y, 0.001);
                ASSERT_NEAR(missleDecoded.orientation.z, missleTarget.orientation.z, 0.001);

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

StateGameSnapshot TestDeltaStateGameSnapshot::RandomGameState()
{
    StateGameSnapshot result;

    for (auto& player : result.players)
    {
        player =
        {
            0,
            {
                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator)
            },
            {
                {
                    myAngles(myGenerator),
                    myAngles(myGenerator),
                    myAngles(myGenerator)
                },
                static_cast<FlagsPlayer>(myFlagsPlayer(myGenerator))
            },
            {

                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator)
            },
            myRandomUint32(myGenerator),
            myRandomUint32(myGenerator),
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
            {
                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator)
            },
            {
                myAngles(myGenerator),
                myAngles(myGenerator),
                myAngles(myGenerator)
            },
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

// /////////////////////
// Tests
// /////////////////////
TEST_F(TestDeltaStateGameSnapshot, Random1000)
{
    myGenerator.seed(1);

    TestN([&]()
    {
        return RandomGameState();
    },
    1000);
}

TEST_F(TestDeltaStateGameSnapshot, Random1000FromZeroIdentity)
{
    auto basePlayer = StatePlayer
    {
        0,
        {0.0, 0.0, 0.0},
        {{0.0, 0.0, 0.0}, FlagsPlayer::Default},
        {0.0, 0.0, 0.0},
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
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        0,
        FlagsMissle::Default
    };

    auto base = StateGameSnapshot
    {
        {{basePlayer}},
        {{baseMissle}},
        StateMode::Default,
        {{" "}}
    };

    TestN([&base]()
    {
        return base;
    },
    1000);
}

}}} // namespace
