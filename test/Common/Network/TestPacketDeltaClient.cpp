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

#include <Common/Network/PacketDeltaClient.hpp>
#include <gmock/gmock.h>
/* RAM: TODO: Fix the entire lot!

using namespace std;
using GameInABox::Common::WrappingCounter;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestPacketDeltaClient : public ::testing::Test
{
public:
    TestPacketDeltaClient()
        : delta8BytePayload(
              Sequence(1),
              Sequence(2),
              3,
              7337,
              {1,2,3,4,5,6,7,8})
    {
    }

    PacketDeltaClient delta8BytePayload;
};

TEST_F(TestPacketDeltaClient, Empty)
{
    PacketDeltaClient toTest;

    EXPECT_FALSE(toTest.IsValid());

    EXPECT_EQ(0, toTest.data.size());

    EXPECT_EQ(0, toTest.IdConnection());
    EXPECT_EQ(0, toTest.GetSequence());
    EXPECT_EQ(0, toTest.GetSequenceAck());
    EXPECT_EQ(0, toTest.GetSequenceBase());
    EXPECT_EQ(0, GetPayloadBuffer(toTest).size());
    EXPECT_EQ(0, toTest.data.size());
}

TEST_F(TestPacketDeltaClient, TestCopy)
{
    PacketDeltaClient toTest(delta8BytePayload);
    EXPECT_EQ(toTest, delta8BytePayload);
}


TEST_F(TestPacketDeltaClient, TestAssignCopy)
{
    PacketDeltaClient toTest = delta8BytePayload;
    EXPECT_EQ(toTest, delta8BytePayload);
}


TEST_F(TestPacketDeltaClient, TestMoveAssign)
{
    PacketDeltaClient toTest = std::move(delta8BytePayload);
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(delta8BytePayload.IsValid());
}

TEST_F(TestPacketDeltaClient, TestMoveCopy)
{
    PacketDeltaClient toTest(std::move(delta8BytePayload));
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(delta8BytePayload.IsValid());
}

TEST_F(TestPacketDeltaClient, Simple)
{
    PacketDeltaClient toTest = delta8BytePayload;

    EXPECT_TRUE(toTest.IsValid());

    EXPECT_NE(0, toTest.data.size());

    Sequence base = Sequence{Sequence(1) - Sequence(3)};

    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(base, toTest.GetSequenceBase());
    EXPECT_EQ(7337, toTest.IdConnection());
    EXPECT_EQ(8, GetPayloadBuffer(toTest).size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4,5,6,7,8}), GetPayloadBuffer(toTest));
}

TEST_F(TestPacketDeltaClient, NoDataClient)
{
    PacketDeltaClient toTest(
                Sequence(1),
                Sequence(2),
                3,
                4,
                std::vector<uint8_t>());

    EXPECT_TRUE(toTest.IsValid());
    EXPECT_NE(0, toTest.data.size());

    Sequence base = Sequence{Sequence(1) - Sequence(3)};

    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(base, toTest.GetSequenceBase());
    EXPECT_EQ(4, toTest.IdConnection());
    EXPECT_EQ(0, GetPayloadBuffer(toTest).size());
}

TEST_F(TestPacketDeltaClient, EncodeDecodeClient)
{
    PacketDeltaClient source(
                Sequence(1),
                Sequence(2),
                3,
                4,
                {1,2,3,4});

    PacketDeltaClient toTest(std::move(source.data));
    std::vector<uint8_t> payload(GetPayloadBuffer(toTest));

    EXPECT_FALSE(source.IsValid());

    EXPECT_NE(0, toTest.data.size());

    Sequence base = Sequence{Sequence(1) - Sequence(3)};

    EXPECT_TRUE(toTest.IsValid());
    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(base, toTest.GetSequenceBase());
    EXPECT_EQ(4, toTest.IdConnection());
    EXPECT_EQ(4, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4}), payload);
}

}}} // namespace
*/
