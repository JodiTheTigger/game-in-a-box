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

#include <Implementation/DeltaStatePlayerClient.hpp>
#include <Implementation/Types.hpp>

#include <Common/UnitOperators.hpp>
#include <Common/MathConstants.hpp>

#include <gtest/gtest.h>

#include <vector>
#include <random>
#include <functional>

using namespace std;
using namespace GameInABox::Common;

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Test Class
// /////////////////////
class TestDeltaStatePlayerClient : public ::testing::Test
{
public:
    TestDeltaStatePlayerClient()
        : ::testing::Test()
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

    void TestN(std::function<StatePlayerClient()> baseGetter, unsigned count);
};


// /////////////////////
// Helpers
// /////////////////////
void TestDeltaStatePlayerClient::TestN(std::function<StatePlayerClient()> baseGetter, unsigned count)
{
    minstd_rand generator;
    uniform_real_distribution<float> angles(-2*Pi(), 2*Pi());
    uniform_int_distribution<uint32_t> flags(0, static_cast<uint>(FlagsPlayer::MaxValue));

    generator.seed(1);

    for (auto setting : myResearch)
    {
        DeltaStatePlayerClient toTest(setting);

        for (unsigned i = 0; i < count; ++i)
        {
            auto target = StatePlayerClient
            {
                {
                    angles(generator),
                    angles(generator),
                    angles(generator)
                },
                static_cast<FlagsPlayer>(flags(generator))
            };

            auto base = baseGetter();

            auto coded = toTest(base, target);
            auto decoded = toTest(base, coded);

            ASSERT_EQ(decoded.flags, target.flags) << " i: " << i;

            // One bit worth is what the error is allowed to be.
            // One bit has a delta range of the inverse m value.
            // But since we don't know that info, lets just assume 3dp
            ASSERT_NEAR(decoded.orientation.x, target.orientation.x, 0.001);
            ASSERT_NEAR(decoded.orientation.y, target.orientation.y, 0.001);
            ASSERT_NEAR(decoded.orientation.z, target.orientation.z, 0.001);
        }
    }
}

// /////////////////////
// Tests
// /////////////////////
TEST_F(TestDeltaStatePlayerClient, Random1000)
{
    minstd_rand generator;
    uniform_real_distribution<float> angles(-2*Pi(), 2*Pi());
    uniform_int_distribution<uint32_t> flags(0, static_cast<uint>(FlagsPlayer::MaxValue));

    generator.seed(1);

    TestN([&]()
    {
        return StatePlayerClient
        {
            {
                angles(generator),
                angles(generator),
                angles(generator)
            },
            static_cast<FlagsPlayer>(flags(generator))
        };
    },
    1000);
}

TEST_F(TestDeltaStatePlayerClient, Random1000FromZeroIdentity)
{
    auto base = StatePlayerClient
    {
        {
            0.0,
            0.0,
            0.0,
        },
        FlagsPlayer::NoFlags
    };

    TestN([&base]()
    {
        return base;
    },
    1000);
}

}}} // namespace
