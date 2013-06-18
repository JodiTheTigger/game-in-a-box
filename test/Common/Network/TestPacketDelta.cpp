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

#include <Common/Network/PacketDelta.hpp>
#include <gmock/gmock.h>

using namespace std;
using GameInABox::Common::WrappingCounter;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestPacketDelta : public ::testing::Test
{
public:
    TestPacketDelta()
        : delta8BytePayloadServer(
              Sequence(1),
              Sequence(2),
              3,
              {1,2,3,4,5,6,7,8})
    {
    }

    PacketDelta delta8BytePayloadServer;
};

TEST_F(TestPacketDelta, Empty)
{
    PacketDelta toTest;

    EXPECT_FALSE(toTest.IsValid());

    EXPECT_EQ(0, toTest.data.size());

    EXPECT_EQ(0, toTest.GetSequence());
    EXPECT_FALSE(toTest.GetSequenceAck());
    EXPECT_EQ(0, toTest.GetSequenceBase());
    EXPECT_EQ(0, GetPayloadBuffer(toTest).size());
}

TEST_F(TestPacketDelta, TestCopy)
{
    PacketDelta toTest(delta8BytePayloadServer);
    EXPECT_EQ(toTest, delta8BytePayloadServer);
}


TEST_F(TestPacketDelta, TestAssignCopy)
{
    PacketDelta toTest = delta8BytePayloadServer;
    EXPECT_EQ(toTest, delta8BytePayloadServer);
}


TEST_F(TestPacketDelta, TestMoveAssign)
{
    PacketDelta toTest = std::move(delta8BytePayloadServer);
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(delta8BytePayloadServer.IsValid());
}

TEST_F(TestPacketDelta, TestMoveCopy)
{
    PacketDelta toTest(std::move(delta8BytePayloadServer));
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(delta8BytePayloadServer.IsValid());
}

TEST_F(TestPacketDelta, NoData)
{
    PacketDelta toTest(Sequence{2},Sequence{4},6,{});

    EXPECT_TRUE(toTest.IsValid());

    EXPECT_NE(0, toTest.data.size());

    Sequence base = Sequence{Sequence(2) - Sequence(6)};

    EXPECT_EQ(2, toTest.GetSequence());
    EXPECT_EQ(base, toTest.GetSequenceBase());
    EXPECT_EQ(0, GetPayloadBuffer(toTest).size());
    ASSERT_TRUE(toTest.GetSequenceAck());
    EXPECT_EQ(4, toTest.GetSequenceAck().get());
}

TEST_F(TestPacketDelta, NoAck)
{
    PacketDelta toTest(Sequence{2},{},6,{});

    EXPECT_TRUE(toTest.IsValid());
    ASSERT_FALSE(toTest.GetSequenceAck());
}

TEST_F(TestPacketDelta, Simple)
{
    PacketDelta& toTest = delta8BytePayloadServer;
    std::vector<uint8_t> payload(GetPayloadBuffer(toTest));

    EXPECT_TRUE(toTest.IsValid());

    EXPECT_NE(0, toTest.data.size());

    Sequence base = Sequence{Sequence(1) - Sequence(3)};

    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(base, toTest.GetSequenceBase());
    EXPECT_EQ(8, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4,5,6,7,8}), payload);
    ASSERT_TRUE(toTest.GetSequenceAck());
    EXPECT_EQ(2, toTest.GetSequenceAck().get());
}

TEST_F(TestPacketDelta, LastSequence)
{
    PacketDelta toTest = {Sequence{Sequence::max()}, Sequence{Sequence::max()}, 0, {1,2,3,4}};
    std::vector<uint8_t> payload(GetPayloadBuffer(toTest));

    EXPECT_TRUE(toTest.IsValid());

    EXPECT_NE(0, toTest.data.size());

    EXPECT_EQ(Sequence::max(), toTest.GetSequence().Value());
    EXPECT_EQ(Sequence::max(), toTest.GetSequenceBase().Value());
    EXPECT_EQ(4, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4}), payload);
    ASSERT_TRUE(toTest.GetSequenceAck());
    EXPECT_EQ(Sequence::max(), toTest.GetSequenceAck().get());
}

TEST_F(TestPacketDelta, EncodeDecode)
{
    PacketDelta& source = delta8BytePayloadServer;
    PacketDelta toTest(std::move(source.data));

    std::vector<uint8_t> payload(GetPayloadBuffer(toTest));

    EXPECT_FALSE(source.IsValid());
    EXPECT_NE(0, toTest.data.size());

    Sequence base = Sequence{Sequence(1) - Sequence(3)};

    EXPECT_TRUE(toTest.IsValid());
    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(base, toTest.GetSequenceBase());
    EXPECT_EQ(8, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4,5,6,7,8}), payload);
    ASSERT_TRUE(toTest.GetSequenceAck());
    EXPECT_EQ(2, toTest.GetSequenceAck().get());
}

}}} // namespace
