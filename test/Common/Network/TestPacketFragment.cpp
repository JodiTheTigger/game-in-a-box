/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012-2013 Richard Maxwell <jodi.the.tigger@gmail.com>
    
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
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <Common/Network/PacketFragment.hpp>
#include <Common/Network/PacketDelta.hpp>
#include <gmock/gmock.h>

using namespace std;
using GameInABox::Common::WrappingCounter;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestPacketFragment : public ::testing::Test
{
public:
    PacketFragment testFragment{Sequence(44), {1,2,3,4}, 1024, 0 };
};

TEST_F(TestPacketFragment, Empty)
{
    PacketFragment toTest;

    EXPECT_FALSE(toTest.IsValid());

    EXPECT_FALSE(toTest.IsLastFragment());
    EXPECT_EQ(0, toTest.FragmentId());
    EXPECT_EQ(0, toTest.data.size());

    EXPECT_EQ(0, toTest.GetSequence());
    EXPECT_EQ(0, GetPayloadBuffer(toTest).size());
    EXPECT_EQ(0, toTest.data.size());
}

TEST_F(TestPacketFragment, Valid)
{
    PacketFragment toTest(testFragment);

    EXPECT_TRUE(toTest.IsValid());

    EXPECT_TRUE(toTest.IsLastFragment());
    EXPECT_EQ(0, toTest.FragmentId());

    EXPECT_EQ(44, toTest.GetSequence());
    EXPECT_EQ(4, GetPayloadBuffer(toTest).size());
    EXPECT_EQ(7, toTest.data.size());
}

TEST_F(TestPacketFragment, TestCopy)
{
    PacketFragment toTest(testFragment);
    EXPECT_EQ(toTest, testFragment);
}

TEST_F(TestPacketFragment, TestAssignCopy)
{
    PacketFragment toTest = testFragment;
    EXPECT_EQ(toTest, testFragment);
}

TEST_F(TestPacketFragment, TestMoveAssign)
{
    PacketFragment toTest = std::move(testFragment);
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(testFragment.IsValid());
}

TEST_F(TestPacketFragment, TestMoveCopy)
{
    PacketFragment toTest(std::move(testFragment));
    EXPECT_TRUE(toTest.IsValid());
    EXPECT_FALSE(testFragment.IsValid());
}

TEST_F(TestPacketFragment, TestLast)
{
    PacketFragment toTest{Sequence(44), {1,2,3,4}, 10, 0};

    EXPECT_TRUE(toTest.IsLastFragment());
}

TEST_F(TestPacketFragment, FragmentEmpty)
{
    auto toTest = PacketFragment{Sequence(0), {}, 1000, 0};

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketFragment, FragmentIdPastEnd)
{
    auto toTest = PacketFragment{Sequence(0), {1,2,3,4,5,6,7}, 1000, 123};

    EXPECT_FALSE(toTest.IsValid());
}

TEST_F(TestPacketFragment, DefragmentSingleNotFragmented)
{
    auto toTest = PacketFragment{PacketDelta(Sequence{0}, Sequence{0}, 0, {1,2,3,4}).data};

    EXPECT_FALSE(toTest.IsValid());
}


TEST_F(TestPacketFragment, Fragments)
{
    auto toTest1 = PacketFragment{Sequence(0), {0,1,2,3,4,5,6,7}, 7, 0};
    auto toTest2 = PacketFragment{Sequence(0), {0,1,2,3,4,5,6,7}, 7, 1};
    std::vector<uint8_t> payload1(GetPayloadBuffer(toTest1));
    std::vector<uint8_t> payload2(GetPayloadBuffer(toTest2));

    // first
    EXPECT_TRUE(toTest1.IsValid());
    EXPECT_EQ(0, toTest1.FragmentId());
    EXPECT_FALSE(toTest1.IsLastFragment());

    // second
    EXPECT_TRUE(toTest2.IsValid());
    EXPECT_EQ(1, toTest2.FragmentId());
    EXPECT_TRUE(toTest2.IsLastFragment());
}

}}} // namespace
