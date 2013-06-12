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

#include <Common/Network/PacketDelta2.hpp>
#include <gmock/gmock.h>

using namespace std;
using GameInABox::Common::WrappingCounter;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestPacketDelta2 : public ::testing::Test
{
public:
    TestPacketDelta2()
        : delta8BytePayloadServer(
              Sequence(1),
              Sequence(2),
              3,
              {1,2,3,4,5,6,7,8})
    {
    }

    PacketDelta2 delta8BytePayloadServer;
};

TEST_F(TestPacketDelta2, Empty)
{
    PacketDelta2 toTest;

    EXPECT_FALSE(toTest.IsValid());

    EXPECT_EQ(0, toTest.data.size());

    EXPECT_EQ(0, toTest.GetSequence());
    EXPECT_EQ(0, toTest.GetSequenceAck());
    EXPECT_EQ(0, toTest.GetSequenceBase());
    EXPECT_EQ(0, GetPayloadBuffer(toTest).size());
}

TEST_F(TestPacketDelta2, TestCopy)
{
    PacketDelta2 toTest(delta8BytePayloadServer);
    EXPECT_EQ(toTest, delta8BytePayloadServer);
}


TEST_F(TestPacketDelta2, TestAssignCopy)
{
    PacketDelta2 toTest = delta8BytePayloadServer;
    EXPECT_EQ(toTest, delta8BytePayloadServer);
}


TEST_F(TestPacketDelta2, TestMoveAssign)
{
    PacketDelta2 toTest = std::move(delta8BytePayloadServer);
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(delta8BytePayloadServer.IsValid());
}

TEST_F(TestPacketDelta2, TestMoveCopy)
{
    PacketDelta2 toTest(std::move(delta8BytePayloadServer));
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(delta8BytePayloadServer.IsValid());
}

TEST_F(TestPacketDelta2, NoData)
{
    PacketDelta2 toTest(Sequence{2},Sequence{4},6,{});

    EXPECT_TRUE(toTest.IsValid());

    EXPECT_NE(0, toTest.data.size());

    EXPECT_EQ(2, toTest.GetSequence());
    EXPECT_EQ(4, toTest.GetSequenceAck());
    EXPECT_EQ(0x7FFE, toTest.GetSequenceBase());
    EXPECT_EQ(0, GetPayloadBuffer(toTest).size());
}

TEST_F(TestPacketDelta2, Simple)
{
    PacketDelta2& toTest = delta8BytePayloadServer;
    std::vector<uint8_t> payload(GetPayloadBuffer(toTest));

    EXPECT_TRUE(toTest.IsValid());

    EXPECT_NE(0, toTest.data.size());

    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(0x7FFF, toTest.GetSequenceBase());
    EXPECT_EQ(8, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4,5,6,7,8}), payload);
}

TEST_F(TestPacketDelta2, EncodeDecode)
{
    PacketDelta2& source = delta8BytePayloadServer;
    PacketDelta2 toTest(std::move(source.data));

    std::vector<uint8_t> payload(GetPayloadBuffer(toTest));

    EXPECT_FALSE(source.IsValid());
    EXPECT_NE(0, toTest.data.size());

    EXPECT_TRUE(toTest.IsValid());
    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(0x7FFF, toTest.GetSequenceBase());
    EXPECT_EQ(8, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4,5,6,7,8}), payload);
}

}}} // namespace
