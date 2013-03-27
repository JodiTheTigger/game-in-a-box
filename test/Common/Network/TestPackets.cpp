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

#include <Common/Network/PacketSimple.h>
#include <Common/Network/PacketChallenge.h>
#include <Common/Network/PacketChallengeResponse.h>
#include <gtest/gtest.h>

using namespace std;

// Class definition!
class TestPacketCommand : public ::testing::Test
{
};

TEST_F(TestPacketCommand, ChallengeFromEmptyData)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge = PacketChallenge(vector<uint8_t>());

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Invalid, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPacketCommand, Challenge)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge;

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Challenge, typeResult);
    EXPECT_TRUE(challenge.IsValid());
}

TEST_F(TestPacketCommand, ChallengeFromNotACommand)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge = PacketChallenge({0xFF, 0x01, 0x02, 0x03});

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Invalid, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}


TEST_F(TestPacketCommand, ChallengeFromValidDataInvalidChallengeBadLength)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge = PacketChallenge({0xFF, 0xFF, uint8_t(PacketCommand::Command::Challenge), 0x03});

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Challenge, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPacketCommand, ChallengeFromValidDataInvalidChallengeBadData)
{
    PacketCommand::Command typeResult;
    PacketChallenge source;

    // make it invalid.
    source.myBuffer[source.myBuffer.size() - 2] = 0;

    PacketChallenge challenge = PacketChallenge(source.myBuffer);

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Challenge, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPacketCommand, ChallengeFromValidData)
{
    PacketCommand::Command typeResult;
    PacketChallenge source;

    PacketChallenge challenge = PacketChallenge(source.myBuffer);

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Challenge, typeResult);
    EXPECT_TRUE(challenge.IsValid());
}

TEST_F(TestPacketCommand, ChallengeResponseCreation)
{
    PacketChallengeResponse source(13, 0x12345678);

    EXPECT_EQ(13, source.Version());
    EXPECT_EQ(0x12345678, source.Key());
    EXPECT_TRUE(source.IsValid());
}

TEST_F(TestPacketCommand, ChallengeResponseCreationZeroKeyInvalid)
{
    PacketChallengeResponse source(0, 0x12345678);

    EXPECT_FALSE(source.IsValid());
}

TEST_F(TestPacketCommand, ChallengeResponseFromValidDataInvalidChallengeBadLength)
{
    PacketCommand::Command typeResult;

    PacketChallengeResponse challengeResponse =
            PacketChallengeResponse({0xFF, 0xFF, uint8_t(PacketCommand::Command::ChallengeResponse), 0x03});

    typeResult = challengeResponse.GetCommand();

    EXPECT_EQ(PacketCommand::Command::ChallengeResponse, typeResult);
    EXPECT_FALSE(challengeResponse.IsValid());
}

TEST_F(TestPacketCommand, SimplePacketsKey)
{
    PacketInfo      info(12345678);
    PacketConnect   connect(87654321);

    ASSERT_TRUE(info.IsValid());
    EXPECT_EQ(12345678, info.Key());

    ASSERT_TRUE(connect.IsValid());
    EXPECT_EQ(87654321, connect.Key());
}

TEST_F(TestPacketCommand, SimplePacketsKeyInvalid)
{
    PacketInfo      info(0);
    PacketConnect   connect(0);

    EXPECT_FALSE(info.IsValid());
    EXPECT_FALSE(connect.IsValid());
}

TEST_F(TestPacketCommand, SimplePacketsBuffer)
{
    PacketInfoResponse info;
    PacketConnectResponse connect;

    PacketInfoResponse infoBuffer({0xFF, 0xFF, uint8_t(PacketCommand::Command::InfoResponse), 0x03});
    PacketConnectResponse connectBuffer({0xFF, 0xFF, uint8_t(PacketCommand::Command::ConnectResponse), 0x03, 0x20});

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

TEST_F(TestPacketCommand, SimplePacketsBufferInvalid)
{
    PacketInfoResponse infoBuffer({0xFF, 0xFF, 42, 0x03});
    PacketConnectResponse connectBuffer({0xFF, 0xFF, 77, 0x03, 0x20});

    EXPECT_FALSE(infoBuffer.IsValid());
    EXPECT_FALSE(connectBuffer.IsValid());
}