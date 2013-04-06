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
#include <Common/Network/PacketDelta.h>
#include <gmock/gmock.h>

using namespace std;

// Class definition!
class TestPackets : public ::testing::Test
{
};

TEST_F(TestPackets, ChallengeFromEmptyData)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge = PacketChallenge(vector<uint8_t>());

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Invalid, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPackets, Challenge)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge;

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Challenge, typeResult);
    EXPECT_TRUE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeFromNotACommand)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge = PacketChallenge({0xFF, 0x01, 0x02, 0x03});

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Invalid, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}


TEST_F(TestPackets, ChallengeFromValidDataInvalidChallengeBadLength)
{
    PacketCommand::Command typeResult;

    PacketChallenge challenge = PacketChallenge({0xFF, 0xFF, uint8_t(PacketCommand::Command::Challenge), 0x03});

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Challenge, typeResult);
    EXPECT_FALSE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeFromValidDataInvalidChallengeBadData)
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

TEST_F(TestPackets, ChallengeFromValidData)
{
    PacketCommand::Command typeResult;
    PacketChallenge source;

    PacketChallenge challenge = PacketChallenge(source.myBuffer);

    typeResult = challenge.GetCommand();

    EXPECT_EQ(PacketCommand::Command::Challenge, typeResult);
    EXPECT_TRUE(challenge.IsValid());
}

TEST_F(TestPackets, ChallengeResponseCreation)
{
    PacketChallengeResponse source(13, 0x12345678);

    EXPECT_EQ(13, source.Version());
    EXPECT_EQ(0x12345678, source.Key());
    EXPECT_TRUE(source.IsValid());
}

TEST_F(TestPackets, ChallengeResponseCreationZeroKeyInvalid)
{
    PacketChallengeResponse source(0, 0x12345678);

    EXPECT_FALSE(source.IsValid());
}

TEST_F(TestPackets, ChallengeResponseFromValidDataInvalidChallengeBadLength)
{
    PacketCommand::Command typeResult;

    PacketChallengeResponse challengeResponse =
            PacketChallengeResponse({0xFF, 0xFF, uint8_t(PacketCommand::Command::ChallengeResponse), 0x03});

    typeResult = challengeResponse.GetCommand();

    EXPECT_EQ(PacketCommand::Command::ChallengeResponse, typeResult);
    EXPECT_FALSE(challengeResponse.IsValid());
}

TEST_F(TestPackets, SimplePacketsKey)
{
    PacketInfo      info(12345678);
    PacketConnect   connect(87654321);

    ASSERT_TRUE(info.IsValid());
    EXPECT_EQ(12345678, info.Key());

    ASSERT_TRUE(connect.IsValid());
    EXPECT_EQ(87654321, connect.Key());
}

TEST_F(TestPackets, SimplePacketsKeyInvalid)
{
    PacketInfo      info(0);
    PacketConnect   connect(0);

    EXPECT_FALSE(info.IsValid());
    EXPECT_FALSE(connect.IsValid());
}

TEST_F(TestPackets, SimplePacketsBuffer)
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

TEST_F(TestPackets, SimplePacketsBufferInvalid)
{
    PacketInfoResponse infoBuffer({0xFF, 0xFF, 42, 0x03});
    PacketConnectResponse connectBuffer({0xFF, 0xFF, 77, 0x03, 0x20});

    EXPECT_FALSE(infoBuffer.IsValid());
    EXPECT_FALSE(connectBuffer.IsValid());
}

TEST_F(TestPackets, DeltaEmpty)
{
    PacketDelta empty;

    EXPECT_FALSE(empty.IsValid());
    EXPECT_FALSE(empty.HasClientId());
    EXPECT_EQ(0, empty.GetSequence());
    EXPECT_EQ(0, empty.GetSequenceAck());
    EXPECT_EQ(0, empty.GetSequenceAckBase());
    EXPECT_EQ(0, empty.TakeBuffer().size());
    EXPECT_EQ(0, empty.GetPayload().size());
}

TEST_F(TestPackets, DeltaNoDataClient)
{
    uint16_t clientId(4);

    PacketDelta toTest(
                WrappingCounter<uint16_t>(1),
                WrappingCounter<uint16_t>(2),
                3,
                &clientId,
                std::vector<uint8_t>());

    EXPECT_TRUE(toTest.IsValid());
    EXPECT_TRUE(toTest.HasClientId());
    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(0xFFFF, toTest.GetSequenceAckBase());
    EXPECT_EQ(4, toTest.ClientId());
    EXPECT_EQ(7, toTest.TakeBuffer().size());
    EXPECT_EQ(0, toTest.GetPayload().size());
}


TEST_F(TestPackets, DeltaNoDataServer)
{
    PacketDelta toTest(2,4,6,nullptr,{});

    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(toTest.HasClientId());
    EXPECT_EQ(2, toTest.GetSequence());
    EXPECT_EQ(4, toTest.GetSequenceAck());
    EXPECT_EQ(0xFFFE, toTest.GetSequenceAckBase());
    EXPECT_EQ(0, toTest.ClientId());
    EXPECT_EQ(5, toTest.TakeBuffer().size());
    EXPECT_EQ(0, toTest.GetPayload().size());
}

TEST_F(TestPackets, DeltaSimpleServer)
{
    PacketDelta toTest(2,4,6,nullptr,{1,2,3,4});
    std::vector<uint8_t> payload(toTest.GetPayload());


    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(toTest.HasClientId());
    EXPECT_EQ(2, toTest.GetSequence());
    EXPECT_EQ(4, toTest.GetSequenceAck());
    EXPECT_EQ(0xFFFE, toTest.GetSequenceAckBase());
    EXPECT_EQ(0, toTest.ClientId());
    EXPECT_EQ(9, toTest.TakeBuffer().size());
    EXPECT_FALSE(toTest.IsValid());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4}), payload);
}

TEST_F(TestPackets, DeltaSimpleClient)
{
    uint16_t clientId(4);

    PacketDelta toTest(
                WrappingCounter<uint16_t>(1),
                WrappingCounter<uint16_t>(2),
                3,
                &clientId,
                {1,2,3,4});

    std::vector<uint8_t> payload(toTest.GetPayload());

    EXPECT_TRUE(toTest.IsValid());
    EXPECT_TRUE(toTest.HasClientId());
    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(0xFFFF, toTest.GetSequenceAckBase());
    EXPECT_EQ(4, toTest.ClientId());
    EXPECT_EQ(11, toTest.TakeBuffer().size());
    EXPECT_FALSE(toTest.IsValid());
    EXPECT_EQ(0, toTest.GetPayload().size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4}), payload);
}

TEST_F(TestPackets, DeltaEncodeDecodeServer)
{
    PacketDelta source(2,4,6,nullptr,{1,2,3,4});
    PacketDelta toTest(source.TakeBuffer());

    std::vector<uint8_t> payload(toTest.GetPayload());

    EXPECT_FALSE(source.IsValid());
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(toTest.HasClientId());
    EXPECT_EQ(2, toTest.GetSequence());
    EXPECT_EQ(4, toTest.GetSequenceAck());
    EXPECT_EQ(0xFFFE, toTest.GetSequenceAckBase());
    EXPECT_EQ(0, toTest.ClientId());
    EXPECT_EQ(9, toTest.TakeBuffer().size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4}), payload);
}

TEST_F(TestPackets, DeltaEncodeDecodeClient)
{
    uint16_t clientId(4);

    PacketDelta source(
                WrappingCounter<uint16_t>(1),
                WrappingCounter<uint16_t>(2),
                3,
                &clientId,
                {1,2,3,4});

    PacketDelta toTest(source.TakeBuffer());
    std::vector<uint8_t> payload(toTest.GetPayload());

    EXPECT_FALSE(source.IsValid());
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_TRUE(toTest.HasClientId());
    EXPECT_EQ(1, toTest.GetSequence());
    EXPECT_EQ(2, toTest.GetSequenceAck());
    EXPECT_EQ(0xFFFF, toTest.GetSequenceAckBase());
    EXPECT_EQ(4, toTest.ClientId());
    EXPECT_EQ(11, toTest.TakeBuffer().size());
    EXPECT_FALSE(toTest.IsValid());
    EXPECT_EQ(0, toTest.GetPayload().size());
    EXPECT_EQ(std::vector<uint8_t>({1,2,3,4}), payload);
}
