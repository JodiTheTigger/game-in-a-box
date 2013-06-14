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

#include <Common/Network/PacketDeltaFragmentManager.hpp>
#include <gmock/gmock.h>

using namespace std;
using GameInABox::Common::WrappingCounter;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestPacketDeltaFragmentManager : public ::testing::Test
{
public:
    TestPacketDeltaFragmentManager()
        : delta4kBytePayloadServerSequence22(
              Sequence(22),
              Sequence(2),
              3,
              std::vector<uint8_t>(4096,33))
        , delta4kBytePayloadServerSequence44(
              Sequence(44),
              Sequence(22),
              22,
              std::vector<uint8_t>(4096,35))
        , delta8BytePayloadServer(
              Sequence(1),
              Sequence(2),
              3,
              {1,2,3,4,5,6,7,8})
    {
    }

    PacketDelta delta4kBytePayloadServerSequence22;
    PacketDelta delta4kBytePayloadServerSequence44;
    PacketDelta delta8BytePayloadServer;
};

TEST_F(TestPacketDeltaFragmentManager, FragmentNothingReturnsNothing)
{
    auto testResult = PacketDeltaFragmentManager::FragmentPacket(PacketDelta());

    EXPECT_EQ(testResult.size(), 0);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentSmallReturnsNotFragmented)
{
    auto testResult = PacketDeltaFragmentManager::FragmentPacket(delta8BytePayloadServer);

    ASSERT_EQ(testResult.size(), 1);
    EXPECT_EQ(PacketDelta(testResult[0]), delta8BytePayloadServer);
}

TEST_F(TestPacketDeltaFragmentManager, LargeGetsFragmentedCorrectlyAndInOrder)
{
    auto testResult = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);

    ASSERT_GT(testResult.size(), 1);

    // make sure they are all valid and the fragment id is as expected.
    // Also tests the returned values are sorted by fragmentId asending.
    PacketFragment last;
    uint8_t count(0);

    for (auto& fragment : testResult)
    {
        PacketFragment toTest(fragment);

        EXPECT_TRUE(toTest.IsValid());
        EXPECT_EQ(toTest.FragmentId(), count);
        count++;
        last = toTest;
    }

    // expect the last packet is recognised as such
    ASSERT_TRUE(last.IsValid());
    EXPECT_TRUE(last.IsLastFragment());
}

TEST_F(TestPacketDeltaFragmentManager, EmptyReturnsInvalid)
{
    PacketDeltaFragmentManager toTest;
    auto testResult = toTest.GetDefragmentedPacket();

    EXPECT_FALSE(testResult.IsValid());
}

TEST_F(TestPacketDeltaFragmentManager, AddNothing)
{
    PacketDeltaFragmentManager toTest;

    // shouldn't crash
    toTest.AddPacket({});
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragment)
{
    auto halfWay = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);

    PacketDeltaFragmentManager toTest;

    for (auto fragment : halfWay)
    {
        toTest.AddPacket(PacketFragment(fragment));
    }

    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta4kBytePayloadServerSequence22);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragmentOldThenNew)
{
    auto halfWayOld = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);
    auto halfWayNew = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44);

    PacketDeltaFragmentManager toTest;

    for (auto fragment : halfWayOld)
    {
        toTest.AddPacket(PacketFragment(fragment));
    }

    for (auto fragment : halfWayNew)
    {
        toTest.AddPacket(PacketFragment(fragment));
    }

    // always expect the older one.
    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta4kBytePayloadServerSequence44);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragmentNewThenOld)
{
    auto halfWayOld = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);
    auto halfWayNew = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44);

    PacketDeltaFragmentManager toTest;

    for (auto fragment : halfWayNew)
    {
        toTest.AddPacket(PacketFragment(fragment));
    }

    for (auto fragment : halfWayOld)
    {
        toTest.AddPacket(PacketFragment(fragment));
    }

    // always expect the highest sequence.
    auto testResult = toTest.GetDefragmentedPacket();
    EXPECT_EQ(testResult, delta4kBytePayloadServerSequence44);
}

TEST_F(TestPacketDeltaFragmentManager, DefragmentTooMany)
{
    PacketDeltaFragmentManager toTest;

    auto fragment = PacketFragment{Sequence(0), delta8BytePayloadServer.data, 7, 0};
    auto buffer = std::vector<PacketFragment>(128, fragment);

    for (auto fragment : buffer)
    {
        toTest.AddPacket(PacketFragment(fragment));
    }

    EXPECT_FALSE(toTest.GetDefragmentedPacket().IsValid());
}

TEST_F(TestPacketDeltaFragmentManager, DefragmentTooBig)
{
    auto big = PacketDelta{Sequence{2},Sequence{4},6,vector<uint8_t>(1024*256, 44)};

    auto result = PacketDeltaFragmentManager::FragmentPacket(big);

    EXPECT_EQ(0, result.size());
}

TEST_F(TestPacketDeltaFragmentManager, DefragmentOutOfOrder)
{
    PacketDeltaFragmentManager toTest;

    auto big = PacketDelta{Sequence{2},Sequence{4},6,vector<uint8_t>(1024*20, 44)};

    auto halfway = PacketDeltaFragmentManager::FragmentPacket(big);

    for (std::size_t i = 0; i < halfway.size(); ++i)
    {
        if (i == 0)
        {
            toTest.AddPacket(PacketFragment{halfway[2]});
        }

        if (i == 1)
        {
            toTest.AddPacket(PacketFragment{halfway[0]});
        }

        if (i == 2)
        {
            toTest.AddPacket(PacketFragment{halfway[1]});
        }

        if (i > 2)
        {
            toTest.AddPacket(PacketFragment{halfway[i]});
        }
    }

    EXPECT_EQ(big, toTest.GetDefragmentedPacket());
}

TEST_F(TestPacketDeltaFragmentManager, DefragmentMissingOne)
{
    PacketDeltaFragmentManager toTest;

    auto big = PacketDelta{Sequence{2},Sequence{4},6,vector<uint8_t>(1024*20, 44)};

    auto halfway = PacketDeltaFragmentManager::FragmentPacket(big);

    for (std::size_t i = 0; i < halfway.size(); ++i)
    {
        if (i == 2)
        {
            continue;
        }

        toTest.AddPacket(PacketFragment{halfway[i]});
    }

    auto result = toTest.GetDefragmentedPacket();

    EXPECT_FALSE(result.IsValid());
}

TEST_F(TestPacketDeltaFragmentManager, DefragmentSequenceInterleavedNotComplete)
{
    auto halfWayOld = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);
    auto halfWayNew = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44);

    PacketDeltaFragmentManager toTest;

    for (auto fragment : halfWayOld)
    {
        toTest.AddPacket(PacketFragment(fragment));
    }

    bool first = true;
    for (auto fragment : halfWayNew)
    {
        if (first)
        {
            first = false;
            continue;
        }

        toTest.AddPacket(PacketFragment(fragment));
    }

    auto result = toTest.GetDefragmentedPacket();

    EXPECT_FALSE(result.IsValid());
}

}}} // namespace
