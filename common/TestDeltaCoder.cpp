/*
    Game-in-a-box. Simple First Person Shooter Network Game.
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

#include "DeltaCoder.h"
#include "DeltaMapItem.h"
#include "gtest/gtest.h"

#include <string>
#include <vector>

using namespace std;

// Class definition!
class TestDeltaCoder : public ::testing::Test 
{
    virtual void SetUp()
    {
        // please remove if not used.
    }
    
protected:
    
    struct DeltaTester
    {
        uint32_t first;
        uint32_t second;
        float waitWhat;
    };
};

TEST_F(TestDeltaCoder, Stub) 
{
    vector<DeltaMapItem> map = {
        {DELTAMAP(TestDeltaCoder::DeltaTester, first, 8)},
        {DELTAMAP(TestDeltaCoder::DeltaTester, second, 18)},
        {DELTAMAP(TestDeltaCoder::DeltaTester, waitWhat, 32)}};
    
    EXPECT_TRUE(false);
}