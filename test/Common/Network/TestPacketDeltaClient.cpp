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
#include <Common/Network/PacketDelta.hpp>
#include <Common/Network/PacketDeltaNoAck.hpp>
#include <gmock/gmock.h>

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestPacketDeltaClient : public ::testing::Test
{

};

TEST_F(TestPacketDeltaClient, EmptyDelta)
{
    auto connectionId = IdConnection(PacketDelta{});
    auto payload = ClientPayload(PacketDelta{});

    EXPECT_FALSE(connectionId);

    EXPECT_EQ(0, payload.size());
    EXPECT_EQ(std::vector<uint8_t>(), payload);
}


TEST_F(TestPacketDeltaClient, EmptyNoAck)
{
    auto connectionId = IdConnection(PacketDeltaNoAck{});
    auto payload = ClientPayload(PacketDeltaNoAck{});

    EXPECT_FALSE(connectionId);

    EXPECT_EQ(0, payload.size());
    EXPECT_EQ(std::vector<uint8_t>(), payload);
}

TEST_F(TestPacketDeltaClient, Delta)
{
    auto delta = PacketDelta{
            Sequence(1),
            Sequence(2),
            3,
            // note, big endian, 1,2 -> 0x0102
    {1,2,3,4,5,6,7,8}};

    auto connectionId = IdConnection(delta);
    auto payload = ClientPayload(delta);

    ASSERT_TRUE(connectionId);
    EXPECT_EQ(0x0102, *connectionId);

    EXPECT_EQ(6, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({3,4,5,6,7,8}), payload);
}

TEST_F(TestPacketDeltaClient, DeltaNoAck)
{
    auto noAck = PacketDeltaNoAck{
            Sequence(1),
            24,
            {1,2,3,4,5,6,7,8}};

    auto connectionId = IdConnection(noAck);
    auto payload = ClientPayload(noAck);

    ASSERT_TRUE(connectionId);
    EXPECT_EQ(0x0102, *connectionId);

    EXPECT_EQ(6, payload.size());
    EXPECT_EQ(std::vector<uint8_t>({3,4,5,6,7,8}), payload);
}

}}} // namespace

