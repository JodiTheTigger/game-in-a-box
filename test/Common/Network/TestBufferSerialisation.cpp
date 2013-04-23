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


#include <Common/Network/BufferSerialisation.h>
#include <gmock/gmock.h>

#include <limits>
#include <vector>
#include <array>
#include <list>

using namespace std;

namespace GameInABox { namespace Common { namespace Network {

template <typename T>
class TestBufferSerialisation : public ::testing::Test
{
public:
    typedef T DataType;

    enum
    {
        maxValue = (uint64_t(std::numeric_limits<T>::max()) + 1),
        maxValueHalf = ((uint64_t(std::numeric_limits<T>::max()) + 1) >> 1)
    };
};

// TYPES I'M TESTING!
typedef ::testing::Types<uint8_t, uint16_t, uint32_t> TestSequenceTypes;
TYPED_TEST_CASE(TestBufferSerialisation, TestSequenceTypes);

// TESTS
TYPED_TEST(TestBufferSerialisation, PushPullRandomArray)
{
    std::array<uint8_t, 64> buffer;
    typename TestFixture::DataType result(0);

    Push(buffer.data(), typename TestFixture::DataType(TestFixture::maxValue - 24));
    Pull(buffer.data(), result);

    EXPECT_EQ(result, TestFixture::maxValue - 24);
}

TYPED_TEST(TestBufferSerialisation, PushPullRandomCArray)
{
    uint8_t buffer[64];
    typename TestFixture::DataType result(0);

    Push(buffer, typename TestFixture::DataType(TestFixture::maxValue - 24));
    Pull(buffer, result);

    EXPECT_EQ(result, TestFixture::maxValue - 24);
}

TYPED_TEST(TestBufferSerialisation, PushPullRandomVector)
{
    std::vector<uint8_t> buffer;
    typename TestFixture::DataType result(0);
    std::back_insert_iterator<std::vector<uint8_t>> writer(buffer);

    Push(writer, typename TestFixture::DataType(TestFixture::maxValue - 24));
    Pull(buffer.begin(), result);

    EXPECT_EQ(result, TestFixture::maxValue - 24);
}

}}} // namespace
