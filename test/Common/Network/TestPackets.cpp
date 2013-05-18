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

#include <Common/Network/PacketTypes.hpp>
#include <Common/Network/PacketChallenge.hpp>
#include <Common/Network/PacketChallengeResponse.hpp>
#include <Common/Network/PacketDelta.hpp>
#include <Common/Network/PacketKey.hpp>
#include <gmock/gmock.h>

using namespace std;
using Bytes = std::vector<uint8_t>;

namespace GameInABox { namespace Common { namespace Network {

// RAM: TODO! TEST DISCONNECT PACKETS!

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

    PacketChallenge challenge = PacketChallenge({0xFF, 0x01, 0x02, 0x03});

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Unrecognised, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}


TEST_F(TestPackets, ChallengeFromValidDataInvalidChallengeBadLength)
{
    Command typeResult;

    PacketChallenge challenge = PacketChallenge({0xFF, 0xFF, uint8_t(Command::Challenge), 0x03});

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Challenge, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeFromValidDataInvalidChallengeBadData)
{
    Command typeResult;

    // make it invalid.
    auto buffer = PacketChallenge().TakeBuffer();
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

    PacketChallenge challenge = PacketChallenge(source.TakeBuffer());

    typeResult = challenge.GetCommand();

    EXPECT_EQ(Command::Challenge, typeResult);
    EXPECT_TRUE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeResponseCreation)
{
    NetworkKey theKey{{0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x69}};
    PacketChallengeResponse source(13, theKey);

    EXPECT_EQ(13, source.Version());
    EXPECT_EQ(theKey, source.Key());
    EXPECT_TRUE(source.IsValid());
}

TEST_F(TestPackets, ChallengeResponseCreationZeroKeyInvalid)
{
    NetworkKey theKey{{0x12,0x34,0x56,0x78,0xAB,0xCD,0xEF,0x69}};
    PacketChallengeResponse source(0, theKey);

    EXPECT_FALSE(source.IsValid());
}

TEST_F(TestPackets, ChallengeResponseFromValidDataInvalidChallengeBadLength)
{
    Command typeResult;

    PacketChallengeResponse challengeResponse =
            PacketChallengeResponse({0xFF, 0xFF, uint8_t(Command::ChallengeResponse), 0x03});

    typeResult = challengeResponse.GetCommand();

    EXPECT_EQ(Command::ChallengeResponse, typeResult);
    EXPECT_FALSE(challengeResponse.IsValid());
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

    PacketInfoResponse infoBuffer(Bytes{0xFF, 0xFF, uint8_t(Command::InfoResponse), 0x03});
    PacketConnectResponse connectBuffer(Bytes{0xFF, 0xFF, uint8_t(Command::ConnectResponse), 0x03, 0x20});

    ASSERT_TRUE(info.IsValid());
    EXPECT_EQ(0, info.GetBuffer().size());

    ASSERT_TRUE(connect.IsValid());
    EXPECT_EQ(0, connect.GetBuffer().size());

    ASSERT_TRUE(infoBuffer.IsValid());
    EXPECT_EQ(1, infoBuffer.GetBuffer().size());
    EXPECT_EQ(3, infoBuffer.GetBuffer()[0]);

    ASSERT_TRUE(connectBuffer.IsValid());
    EXPECT_EQ(2, connectBuffer.GetBuffer().size());
    EXPECT_EQ(3, connectBuffer.GetBuffer()[0]);
    EXPECT_EQ(0x20, connectBuffer.GetBuffer()[1]);
}

TEST_F(TestPackets, SimplePacketsBufferInvalid)
{
    PacketInfoResponse infoBuffer(Bytes{0xFF, 0xFF, 42, 0x03});
    PacketConnectResponse connectBuffer(Bytes{0xFF, 0xFF, 77, 0x03, 0x20});

    EXPECT_FALSE(infoBuffer.IsValid());
    EXPECT_FALSE(connectBuffer.IsValid());
}

}}} // namespace
