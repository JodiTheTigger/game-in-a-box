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

#include <Common/Network/Packets.hpp>
#include <gmock/gmock.h>

using namespace std;
using Bytes = std::vector<uint8_t>;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestPackets : public ::testing::Test
{
};

TEST_F(TestPackets, ChallengeFromEmptyData)
{
    Command typeResult;

    PacketChallenge challenge = PacketChallenge(vector<uint8_t>());

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Unrecognised, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPackets, Challenge)
{
    Command typeResult;

    PacketChallenge challenge;

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Challenge, typeResult);
    EXPECT_TRUE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeFromNotACommand)
{
    Command typeResult;

    auto challenge = PacketChallenge{PacketInfoResponse().data};

    typeResult = challenge.GetCommand();

    EXPECT_NE(Command::Challenge, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeFromValidDataInvalidChallengeBadLength)
{
    Command typeResult;    

    auto challenge = PacketChallengeResponse{Packet(Command::Challenge).data};
    challenge.data.push_back(0x03);

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Challenge, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeFromValidDataInvalidChallengeBadData)
{
    Command typeResult;

    // make it invalid.
    auto buffer = PacketChallenge().data;
    buffer[buffer.size() - 2] = 0;
    PacketChallenge challenge(buffer);

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Challenge, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeFromValidData)
{
    Command typeResult;
    PacketChallenge source;

    PacketChallenge challenge = PacketChallenge(source.data);

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Challenge, typeResult);
    EXPECT_TRUE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeResponseCreation)
{
    NetworkKey theKey{{0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x69}};
    auto payload = std::vector<uint8_t>{1,2,3,4};
    PacketChallengeResponse source(13, theKey, payload);

    ASSERT_TRUE(source.IsValid());
    EXPECT_EQ(13, source.Version());
    EXPECT_EQ(theKey, source.Key());
    EXPECT_EQ(GetPayloadBuffer(source), payload);
}

TEST_F(TestPackets, ChallengeResponseCreationZeroKeyInvalid)
{
    NetworkKey theKey{{0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x69}};
    PacketChallengeResponse source(0, theKey);

    EXPECT_FALSE(source.IsValid());
}

TEST_F(TestPackets, ChallengeResponsePayloadSize)
{
    NetworkKey theKey{{0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x69}};
    PacketChallengeResponse source(255, theKey);

    EXPECT_EQ(0, GetPayloadBuffer(source).size());
    EXPECT_EQ(0, GetPayloadString(source).size());
}

TEST_F(TestPackets, ChallengeResponseFromValidDataInvalidChallengeBadLength)
{
    Command typeResult;

    auto challengeResponse = PacketChallengeResponse{Packet(Command::ChallengeResponse).data};
    challengeResponse.data.push_back(0x03);

    typeResult = challengeResponse.GetCommand();

    EXPECT_EQ(Command::ChallengeResponse, typeResult);
    EXPECT_FALSE(challengeResponse.IsValid());
}

TEST_F(TestPackets, Disconnect)
{
    auto theKey = NetworkKey{{0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x69}};
    auto message = std::string{"Because!"};
    auto source = PacketDisconnect{theKey, message};

    EXPECT_TRUE(source.IsValid());
    EXPECT_EQ(theKey, source.Key());
    EXPECT_EQ(message, GetPayloadString(source));
}


TEST_F(TestPackets, SimplePacketsKey)
{    
    NetworkKey key1{{0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x69}};
    NetworkKey key2{{0x10,0x29,0x38,0x48,0x57,0x66,0xEF,0x69}};

    PacketInfo      info(key1);
    PacketConnect   connect(key2);

    ASSERT_TRUE(info.IsValid());
    EXPECT_EQ(key1, info.Key());

    ASSERT_TRUE(connect.IsValid());
    EXPECT_EQ(key2, connect.Key());
}

TEST_F(TestPackets, SimplePacketsKeyInvalid)
{
    PacketInfo      info(GetNetworkKeyNil());
    PacketConnect   connect(GetNetworkKeyNil());

    EXPECT_FALSE(info.IsValid());
    EXPECT_FALSE(connect.IsValid());
}

TEST_F(TestPackets, SimplePacketsBuffer)
{
    PacketInfoResponse info;
    PacketConnectResponse connect;

    auto infoBuffer = PacketInfoResponse::WithBuffer({0x03});
    auto connectBuffer = PacketConnectResponse::WithBuffer({0x03, 0x20});

    ASSERT_TRUE(info.IsValid());
    EXPECT_EQ(0, GetPayloadBuffer(info).size());

    ASSERT_TRUE(connect.IsValid());
    EXPECT_EQ(0, GetPayloadBuffer(connect).size());

    ASSERT_TRUE(infoBuffer.IsValid());
    EXPECT_EQ(1, GetPayloadBuffer(infoBuffer).size());
    EXPECT_EQ(3, GetPayloadBuffer(infoBuffer)[0]);

    ASSERT_TRUE(connectBuffer.IsValid());
    EXPECT_EQ(2, GetPayloadBuffer(connectBuffer).size());
    EXPECT_EQ(3, GetPayloadBuffer(connectBuffer)[0]);
    EXPECT_EQ(0x20, GetPayloadBuffer(connectBuffer)[1]);
}

TEST_F(TestPackets, SimplePacketsBufferInvalid)
{
    PacketInfoResponse infoBuffer(PacketConnectResponse().data);
    PacketConnectResponse connectBuffer(PacketInfoResponse().data);

    EXPECT_FALSE(infoBuffer.IsValid());
    EXPECT_FALSE(connectBuffer.IsValid());
}

/* RAM: link error - fix!
TEST_F(TestPackets, ClientEmptyDelta)
{
    auto connectionId = IdConnection(PacketDelta{});
    auto payload = ClientPayload(PacketDelta{});

    EXPECT_FALSE(connectionId);

    EXPECT_EQ(0, payload.size());
    EXPECT_EQ(std::vector<uint8_t>(), payload);
}*/

/* RAM: TODO: FIX!
TEST_F(TestPackets, ClientEmptyNoAck)
{
    auto connectionId = IdConnection(PacketDeltaNoAck{});
    auto payload = ClientPayload(PacketDeltaNoAck{});

    EXPECT_FALSE(connectionId);

    EXPECT_EQ(0, payload.size());
    EXPECT_EQ(std::vector<uint8_t>(), payload);
}*/

/* RAM: link error - fix!
TEST_F(TestPackets, ClientDelta)
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
}*/

/* RAM: TODO: FIX!
TEST_F(TestPackets, ClientDeltaNoAck)
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
}*/


}}} // namespace
