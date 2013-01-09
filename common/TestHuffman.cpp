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

#include "Huffman.h"
#include "gtest/gtest.h"

#include <string>

using namespace std;

// Class definition!
class TestHuffman : public ::testing::Test 
{
    virtual void SetUp()
    {
        std::string testPhrase;

        testPhrase = "Silly Sally saw sixty-six thick thistle sticks";

        myTestBuffers.push_back(new vector<uint8_t>(testPhrase.begin(), testPhrase.end()));
    }
    
    std::vector<std::vector<uint8_t>*> myTestBuffers; 
};

TEST_F(TestHuffman, CreateSimpleMap) 
{
    // RAM: TODO!
    EXPECT_FALSE(true);
}
