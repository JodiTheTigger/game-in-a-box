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

#include <Common/UnitOperators.hpp>

#include <gtest/gtest.h>

#include <vector>
#include <random>

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
};

// /////////////////////
// Tests
// /////////////////////
TEST_F(TestDeltaStateGameSnapshot, Todo)
{
    ASSERT_TRUE(false);
}

}}} // namespace
