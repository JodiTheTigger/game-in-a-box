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
    EXPECT_EQ(testResult[0], delta8BytePayloadServer);
}

TEST_F(TestPacketDeltaFragmentManager, LargeGetsFragmentedCorrectlyAndInOrder)
{
    /* RAM: TODO: FIX!
    auto testResult = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);

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
    */
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
    // RAM: TODO: FIX! toTest.AddPacket(PacketDelta());
}

TEST_F(TestPacketDeltaFragmentManager, AddNotFragmentedReturnsValid)
{
    PacketDeltaFragmentManager toTest;

    // RAM: TODO: FIX! toTest.AddPacket(delta8BytePayloadServer);

    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_TRUE(testResult.IsValid());
}

TEST_F(TestPacketDeltaFragmentManager, FragmentDefragment)
{
    auto halfWay = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);

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
    auto halfWayOld = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);
    auto halfWayNew = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44);

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
    auto halfWayOld = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence22);
    auto halfWayNew = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44);

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
    auto halfWayNew = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44);

    PacketDeltaFragmentManager toTest;

    // RAM: TODO: FIX! toTest.AddPacket(delta8BytePayloadServer);

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
    auto halfWayNew = PacketDeltaFragmentManager::FragmentPacket(delta4kBytePayloadServerSequence44);

    PacketDelta delta8BytePayloadServer66(
                  Sequence(66),
                  Sequence(2),
                  3,
                  {1,2,3,4,5,6,7,8});

    PacketDeltaFragmentManager toTest;

    // RAM: TODO: FIX! toTest.AddPacket(delta8BytePayloadServer66);

    for (auto fragment : halfWayNew)
    {
        toTest.AddPacket(fragment);
    }

    // always expect the highest sequence.
    PacketDelta testResult(toTest.GetDefragmentedPacket());
    EXPECT_EQ(testResult, delta8BytePayloadServer66);
}

// RAM: TODO: Include these tests too.
/*
// =================================================
// Fragment testing
// =================================================
TEST_F(TestPacketDelta, FragmentEmpty)
{
    PacketDelta toTest(PacketDelta(), 0, 0);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, FragmentIdPastEnd)
{
    PacketDelta toTest(delta8BytePayloadServer, 4, 100);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, FragmentNotFragmented)
{
    PacketDelta toTest(delta8BytePayloadServer, 1024, 0);

    EXPECT_FALSE(toTest.IsFragmented());
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_EQ(toTest.data.size(), delta8BytePayloadServer.data.size());
}

TEST_F(TestPacketDelta, Fragments)
{
    PacketDelta toTest1(delta8BytePayloadServer, 10, 0);
    PacketDelta toTest2(delta8BytePayloadServer, 10, 1);
    std::vector<uint8_t> payload1(GetPayloadBuffer(toTest1));
    std::vector<uint8_t> payload2(GetPayloadBuffer(toTest2));

    // first
    EXPECT_TRUE(toTest1.IsValid());
    EXPECT_TRUE(toTest1.IsFragmented());
    EXPECT_EQ(0, toTest1.FragmentId());
    EXPECT_FALSE(toTest1.IsLastFragment());
    EXPECT_EQ(toTest1.data.size(), 10);
    EXPECT_EQ(7, payload1.size());

    // second
    EXPECT_TRUE(toTest2.IsValid());
    EXPECT_TRUE(toTest2.IsFragmented());
    EXPECT_EQ(1, toTest2.FragmentId());
    EXPECT_TRUE(toTest2.IsLastFragment());
    EXPECT_EQ(toTest2.data.size(), 3 + ((delta8BytePayloadServer.data.size() - 2) - 7));
}

TEST_F(TestPacketDelta, DefragmentEmpty)
{
    auto buffer = std::vector<PacketDelta>();
    PacketDelta toTest(buffer);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, DefragmentSingleNotFragmented)
{
    auto buffer = std::vector<PacketDelta>(1, delta8BytePayloadServer);

    PacketDelta toTest(buffer);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, DefragmentMultipleNotFragmented)
{
    auto buffer = std::vector<PacketDelta>(3, delta8BytePayloadServer);

    PacketDelta toTest(buffer);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, DefragmentTooMany)
{
    PacketDelta fragment(delta8BytePayloadServer, 10, 0);

    auto buffer = std::vector<PacketDelta>(128, fragment);

    PacketDelta toTest(buffer);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, DefragmentTooBig)
{
    PacketDelta toTest({Sequence{2},Sequence{4},6,{},vector<uint8_t>(1024*256, 44)}, 1500, 0);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, DefragmentSimple)
{
    std::vector<PacketDelta> fragments;

    // Oh comon, this is c++11, do this using lambdas or iterators or something.
    uint8_t count(0);
    bool go(true);
    while (go)
    {
        PacketDelta result(PacketDelta(delta8BytePayloadServer, 8, count));
        if (result.IsLastFragment())
        {
            go = false;
        }
        fragments.push_back(result);
        count++;
    }

    PacketDelta toTest(fragments);

    EXPECT_EQ(toTest, delta8BytePayloadServer);
}

TEST_F(TestPacketDelta, DefragmentOutOfOrder)
{
    std::vector<PacketDelta> fragments;

    uint8_t count(3);
    while (count < 4)
    {
        PacketDelta result(PacketDelta(delta8BytePayloadServer, 8, count));
        fragments.push_back(result);
        count--;
    }

    PacketDelta toTest(fragments);

    EXPECT_EQ(toTest, delta8BytePayloadServer);
}


TEST_F(TestPacketDelta, DefragmentSimpleMissingOne)
{
    std::vector<PacketDelta> fragments;

    // Oh comon, this is c++11, do this using lambdas or iterators or something.
    uint8_t count(0);
    bool go(true);
    while (go)
    {
        PacketDelta result(PacketDelta(delta8BytePayloadServer, 8, count));
        if (result.IsLastFragment())
        {
            go = false;
        }

        // miss a fragment.
        if (count != 1)
        {
            fragments.push_back(result);
        }

        count++;
    }

    PacketDelta toTest(fragments);

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketDelta, DefragmentFragmentNoFragmentedFragments)
{
    std::vector<PacketDelta> fragments;

    // Oh comon, this is c++11, do this using lambdas or iterators or something.
    uint8_t count(0);
    bool go(true);
    while (go)
    {
        PacketDelta result(PacketDelta(delta8BytePayloadServer, 8, count));
        if (result.IsLastFragment())
        {
            go = false;
        }
        fragments.push_back(result);

        // throw in a normal packet, should be ignored when defragged.
        if (count == 0)
        {
            fragments.push_back({Sequence{2},Sequence{4},6,{},{2,4,6,8,10,12,14,18}});
        }

        count++;
    }

    PacketDelta toTest(fragments);

    EXPECT_EQ(toTest, delta8BytePayloadServer);
}

TEST_F(TestPacketDelta, DefragmentSequenceInterleavedNotComplete)
{
    // send complete delta fragments, but interleaved with one
    // of a newer sequence, should be invalid returned.
    std::vector<PacketDelta> fragments;
    PacketDelta first(PacketDelta(delta8BytePayloadServer, 8, 0));
    PacketDelta second({Sequence{2},Sequence{4},6,{},{2,4,6,8,10,12,14,18}}, 8, 0);

    fragments.push_back(first);
    fragments.push_back(second);

    uint8_t count(1);
    bool go(true);
    while (go)
    {
        PacketDelta result(PacketDelta(delta8BytePayloadServer, 8, count));
        if (result.IsLastFragment())
        {
            go = false;
        }
        fragments.push_back(result);
        count++;
    }

    PacketDelta toTest(fragments);

    EXPECT_FALSE(toTest.IsValid());
}
*/


}}} // namespace
