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

#include <Common/Network/PacketDelta.h>
#include <gmock/gmock.h>

using namespace std;

// Class definition!
class TestPacketDeltaDefragmenter : public ::testing::Test
{
public:
    TestPacketDeltaDefragmenter()
        : delta8BytePayloadServer(
              WrappingCounter<uint16_t>(1),
              WrappingCounter<uint16_t>(2),
              3,
              nullptr,
              {1,2,3,4,5,6,7,8})
    {
    }

    void TestEmpty(const PacketDelta& toTest)
    {
        EXPECT_FALSE(toTest.IsValid());

        EXPECT_FALSE(toTest.IsFragmented());
        EXPECT_FALSE(toTest.IsLastFragment());
        EXPECT_EQ(0, toTest.FragmentId());
        EXPECT_EQ(0, toTest.Size());

        EXPECT_FALSE(toTest.HasClientId());
        EXPECT_EQ(0, toTest.GetSequence());
        EXPECT_EQ(0, toTest.GetSequenceAck());
        EXPECT_EQ(0, toTest.GetSequenceBase());
        EXPECT_EQ(0, toTest.GetPayload().size());
    }

    PacketDelta delta8BytePayloadServer;
};

TEST_F(TestPacketDeltaDefragmenter, Empty)
{
    // RAM: TODO!
    EXPECT_TRUE(false);
}
