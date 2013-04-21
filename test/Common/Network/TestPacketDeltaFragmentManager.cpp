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

#include <Common/Network/PacketDeltaFragmentManager.h>
#include <gmock/gmock.h>

using namespace std;

// Class definition!
class TestPacketDeltaFragmentManager : public ::testing::Test
{
public:
    TestPacketDeltaFragmentManager()
        : delta4kBytePayloadServerSequence22(
              WrappingCounter<uint16_t>(22),
              WrappingCounter<uint16_t>(2),
              3,
              nullptr,
              std::vector<uint8_t>(4096,33))
        , delta4kBytePayloadServerSequence44(
              WrappingCounter<uint16_t>(44),
              WrappingCounter<uint16_t>(22),
              22,
              nullptr,
              std::vector<uint8_t>(4096,35))
        , delta8BytePayloadServer(
              WrappingCounter<uint16_t>(1),
              WrappingCounter<uint16_t>(2),
              3,
              nullptr,
              {1,2,3,4,5,6,7,8})
    {
    }

    PacketDelta delta4kBytePayloadServerSequence22;
    PacketDelta delta4kBytePayloadServerSequence44;
    PacketDelta delta8BytePayloadServer;
};

TEST_F(TestPacketDeltaFragmentManager, FragmentNothingReturnsNothing)
{
    std::vector<PacketDelta> testResult(
                PacketDeltaFragmentManager::FragmentPacket(PacketDelta()));

    EXPECT_EQ(testResult.size(), 0);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentSmallReturnsNotFragmented)
{
    std::vector<PacketDelta> testResult(
                PacketDeltaFragmentManager::FragmentPacket(delta8BytePayloadServer));

    ASSERT_EQ(testResult.size(), 1);
    EXPECT_EQ(testResult[0], delta8BytePayloadServer);
}

TEST_F(TestPacketDeltaFragmentManager, LargeGetsFragmentedCorrectlyAndInOrder)
{
    std::vector<PacketDelta> testResult(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22));

    ASSERT_GT(testResult.size(), 1);

    // make sure they are all valid and the fragment id is as expected.
    // Also tests the returned values are sorted by fragmentId asending.
    PacketDelta* last(nullptr);
    uint8_t count(0);
    for (auto& fragment : testResult)
    {
        EXPECT_TRUE(fragment.IsValid());
        EXPECT_TRUE(fragment.IsFragmented());
        EXPECT_EQ(fragment.FragmentId(), count);
        count++;
        last = &fragment;
    }

    // expect the last packet is recognised as such
    ASSERT_NE(last, nullptr);
    EXPECT_TRUE(last->IsLastFragment());
}

TEST_F(TestPacketDeltaFragmentManager, EmptyReturnsInvalid)
{
    PacketDeltaFragmentManager toTest;
    PacketDelta testResult(toTest.GetDefragmentedPacket());

    EXPECT_FALSE(testResult.IsValid());
}

TEST_F(TestPacketDeltaFragmentManager, AddNothing)
{
    PacketDeltaFragmentManager toTest;

    // shouldn't crash
    toTest.AddPacket(PacketDelta());
}

TEST_F(TestPacketDeltaFragmentManager, AddNotFragmentedReturnsValid)
{
    PacketDeltaFragmentManager toTest;

    toTest.AddPacket(delta8BytePayloadServer);

    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_TRUE(testResult.IsValid());
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragment)
{
    std::vector<PacketDelta> halfWay(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22));

    PacketDeltaFragmentManager toTest;

    for (auto fragment : halfWay)
    {
        toTest.AddPacket(fragment);
    }

    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta4kBytePayloadServerSequence22);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragmentOldThenNew)
{
    std::vector<PacketDelta> halfWayOld(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22));
    std::vector<PacketDelta> halfWayNew(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44));

    PacketDeltaFragmentManager toTest;

    for (auto fragment : halfWayOld)
    {
        toTest.AddPacket(fragment);
    }

    for (auto fragment : halfWayNew)
    {
        toTest.AddPacket(fragment);
    }

    // always expect the older one.
    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta4kBytePayloadServerSequence44);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragmentNewThenOld)
{
    std::vector<PacketDelta> halfWayOld(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22));
    std::vector<PacketDelta> halfWayNew(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44));

    PacketDeltaFragmentManager toTest;

    for (auto fragment : halfWayNew)
    {
        toTest.AddPacket(fragment);
    }

    for (auto fragment : halfWayOld)
    {
        toTest.AddPacket(fragment);
    }

    // always expect the highest sequence.
    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta4kBytePayloadServerSequence44);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragmentOldThenNewComplete)
{
    std::vector<PacketDelta> halfWayNew(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44));

    PacketDeltaFragmentManager toTest;

    toTest.AddPacket(delta8BytePayloadServer);

    for (auto fragment : halfWayNew)
    {
        toTest.AddPacket(fragment);
    }

    // always expect the highest sequence.
    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta4kBytePayloadServerSequence44);
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragmentNewThenOldComplete)
{
    std::vector<PacketDelta> halfWayNew(
                PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44));

    PacketDelta delta8BytePayloadServer66(
                  WrappingCounter<uint16_t>(66),
                  WrappingCounter<uint16_t>(2),
                  3,
                  nullptr,
                  {1,2,3,4,5,6,7,8});

    PacketDeltaFragmentManager toTest;

    toTest.AddPacket(delta8BytePayloadServer66);

    for (auto fragment : halfWayNew)
    {
        toTest.AddPacket(fragment);
    }

    // always expect the highest sequence.
    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta8BytePayloadServer66);
}
