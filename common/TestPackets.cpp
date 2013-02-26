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

#include "PacketSimple.h"
#include "PacketChallenge.h"
#include "PacketChallengeResponse.h"
#include "gtest/gtest.h"

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

TEST_F(TestPacketCommand, NotAllTestsWritten)
{
    EXPECT_FALSE(true);
}
