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


#include <Common/Network/BufferSerialisation.h>
#include <gmock/gmock.h>

using namespace std;

// Class definition!
class TestBufferSerialisation : public ::testing::Test
{

};

TEST_F(TestBufferSerialisation, EmptyBufferEmptyKey)
{
    std::vector<uint8_t> buffer;

    // will it compile without problem (No)
    //Push(uint8_t(8), buffer);
    //Push(uint16_t(16), buffer);
    //Push(uint32_t(32), buffer);

    // no execptions expected;
    EXPECT_TRUE(false);
}
