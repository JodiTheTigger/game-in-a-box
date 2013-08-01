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

#include <Implementation/DeltaCoder.hpp>
#include <Implementation/DeltaMapItem.hpp>

#include <Common/BitStream.hpp>
#include <Common/BitStreamReadOnly.hpp>

#include <gtest/gtest.h>

#include <string>
#include <vector>
#include <memory>
#include <random>

using namespace std;
using namespace GameInABox::Common;

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Test all delta coder types, and their invalid values.


// /////////////////////
// Test Class
// /////////////////////
class TestDeltaCoder : public ::testing::Test 
{
public:
    TestDeltaCoder()
        : ::testing::Test()
        , myMap()
        , myIdentity()
        , myFirst()
        , mySecond()
    {
    }

    virtual void SetUp()
    {
        myMap = std::vector<DeltaMapItem>
        {
            {MAKE_OFFSET(TestDeltaCoder::DeltaTester, first), MapUnsigned{8_bits}},
            {MAKE_OFFSET(TestDeltaCoder::DeltaTester, second), MapUnsigned{18_bits}},
            {MAKE_OFFSET(TestDeltaCoder::DeltaTester, waitWhat), MapFloatFull{}}
        };

        myIdentity = DeltaTester{{0,0,0}, 0, 0, 0, false, 0, 0};
        myFirst = DeltaTester{{0,0,0},1,2,3.141f,false,0,0};
        mySecond = DeltaTester{{1.0,0,-42.3423},128,0xFFFFFFFF,-1024.0f,true,-44,88.88};
    }

protected:
    
    struct DeltaTester
    {
        float vector[3];
        uint32_t first;
        uint32_t second;
        float waitWhat;
        bool ignored;
        int32_t third;
        double ignored2;
    };
    
    vector<DeltaMapItem> myMap;
    DeltaTester myIdentity;
    DeltaTester myFirst;
    DeltaTester mySecond;

    DeltaTester TestMapFirstToSecond(std::vector<DeltaMapItem> map);
};

// /////////////////////
// Helpers
// /////////////////////
TestDeltaCoder::DeltaTester TestDeltaCoder::TestMapFirstToSecond(std::vector<DeltaMapItem> map)
{
    DeltaTester result;
    BitStream stream(32);

    DeltaCoder<DeltaTester> coder(map, {true, true});

    coder.DeltaEncode(myFirst, mySecond, stream);
    coder.DeltaDecode(myFirst, result, stream);

    return result;
}

// /////////////////////
// Tests
// /////////////////////
TEST_F(TestDeltaCoder, EncodeDecodeAgainstIdentity) 
{
    DeltaTester result;
    BitStream data(32);
        
    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myIdentity, myFirst, data);
    myCoder.DeltaDecode(myIdentity, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(myFirst.first, result.first);
    EXPECT_EQ(myFirst.second, result.second);
    EXPECT_EQ(myFirst.waitWhat, result.waitWhat);
}

TEST_F(TestDeltaCoder, EncodeDecodeFullDiff) 
{
    DeltaTester second;
    DeltaTester result;
    BitStream data(32);
        
    second.first = 4;
    second.second = 8;
    second.waitWhat = 1.7f;
        
    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myFirst, second, data);
    myCoder.DeltaDecode(myFirst, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(second.first, result.first);
    EXPECT_EQ(second.second, result.second);
    EXPECT_EQ(second.waitWhat, result.waitWhat);
}

TEST_F(TestDeltaCoder, EncodeDecodePartialDiff) 
{
    DeltaTester second;
    DeltaTester result;
    BitStream data(32);
        
    second.first = 5;
    second.second = myFirst.second;
    second.waitWhat = 1.7f;
        
    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myFirst, second, data);
    myCoder.DeltaDecode(myFirst, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(second.first, result.first);
    EXPECT_EQ(second.second, result.second);
    EXPECT_EQ(second.waitWhat, result.waitWhat);
}

TEST_F(TestDeltaCoder, EncodeAgainstIdentity) 
{
    BitStream data(32);

    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myIdentity, myFirst, data);
    
    EXPECT_FALSE(data.Pull1Bit());
    EXPECT_FALSE(data.Pull1Bit());    
    EXPECT_EQ(1, data.PullU32(8));
        
    EXPECT_FALSE(data.Pull1Bit());
    EXPECT_FALSE(data.Pull1Bit());    
    EXPECT_EQ(2, data.PullU32(18));
        
    EXPECT_FALSE(data.Pull1Bit());
    EXPECT_FALSE(data.Pull1Bit());
    auto asUint32 = data.PullU32(32);
    float asFloat;

    // aliasing is not allowed, memcpy.
    ASSERT_EQ(sizeof(float), sizeof(uint32_t));
    memcpy(&asFloat, &asUint32, sizeof(float));

    EXPECT_EQ(3.141f, asFloat);
}



TEST_F(TestDeltaCoder, RandomStates)
{
    minstd_rand generator;
    uniform_int_distribution<uint32_t> even;
    uniform_int_distribution<uint32_t> even100(0,99);
    
    vector<DeltaTester> states;
            
    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    generator.seed(1);
    
    for (int i = 0; i < 100; i++)
    {
        states.push_back(DeltaTester{
            {0,0,0},
            static_cast<uint32_t>(even(generator) & 0xFF),
            static_cast<uint32_t>(even(generator) & 0x3FFFF),
            static_cast<float>(even(generator)) / 37.0f,
            false,
            0,
            0});
    }
    
    for (int i = 0; i < 1000; i++)
    {
        uint32_t from;
        uint32_t to;
        DeltaTester result;
        
        BitStream stream(32);
        
        from = even100(generator);
        to = even100(generator);

        myCoder.DeltaEncode(states[from], states[to], stream);
        myCoder.DeltaDecode(states[from], result, stream);
                
        ASSERT_EQ(states[to].first, result.first)
                << "From:" << ::testing::PrintToString(from)
                << " To: " << ::testing::PrintToString(to)
                << " i: " << i
                << "\nstate from: " << ::testing::PrintToString(states[from])
                << "\nstate to  : " << ::testing::PrintToString(states[to])
                << "\nresult    : " << ::testing::PrintToString(result);
        ASSERT_EQ(states[to].second, result.second);
        ASSERT_EQ(states[to].waitWhat, result.waitWhat);
    }
}

TEST_F(TestDeltaCoder, MapFloatFull)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, vector[0]), MapFloatFull{}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, vector[1]), MapFloatFull{}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, vector[2]), MapFloatFull{}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, waitWhat), MapFloatFull{}},
    };

    auto result = TestMapFirstToSecond(map);

    EXPECT_FLOAT_EQ(mySecond.vector[0], result.vector[0]);
    EXPECT_FLOAT_EQ(mySecond.vector[1], result.vector[1]);
    EXPECT_FLOAT_EQ(mySecond.vector[2], result.vector[2]);
    EXPECT_FLOAT_EQ(mySecond.waitWhat, result.waitWhat);
}


TEST_F(TestDeltaCoder, MapUnsigned)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, first), MapUnsigned{6_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, second), MapUnsigned{31_bits}},
    };

    auto result = TestMapFirstToSecond(map);

    EXPECT_EQ(mySecond.first & 0x3F, result.first);
    EXPECT_EQ(mySecond.second & 0x7FFFFFF, result.second);
    EXPECT_NE(mySecond.second & 0xFFFFFFF, result.second);
}

TEST_F(TestDeltaCoder, MapUnsignedNegativeBits)
{
    auto map = std::vector<DeltaMapItem>
    {
        // RAM: TODO: Support compiling -8_bits!
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, first), MapUnsigned{{-8}}}
    };

    auto result = TestMapFirstToSecond(map);

    EXPECT_EQ(myFirst.first, result.first);
}

TEST_F(TestDeltaCoder, MapUnsigned0Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, first), MapUnsigned{0_bits}}
    };

    auto result = TestMapFirstToSecond(map);

    EXPECT_EQ(myFirst.first, result.first);
}

TEST_F(TestDeltaCoder, MapUnsigned33Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, second), MapUnsigned{33_bits}}
    };

    auto result = TestMapFirstToSecond(map);

    EXPECT_EQ(mySecond.second, result.second);
}


/*
TEST_F(TestDeltaCoder, Signed);
TEST_F(TestDeltaCoder, SignedNegativeBits);
TEST_F(TestDeltaCoder, Signed0Bits);
TEST_F(TestDeltaCoder, Signed33Bits);
TEST_F(TestDeltaCoder, FloatRangedAsFloat);
TEST_F(TestDeltaCoder, FloatRangedAsInt);
TEST_F(TestDeltaCoder, FloatRanged0Max);
TEST_F(TestDeltaCoder, FloatRanged24bitMaxRange);
TEST_F(TestDeltaCoder, FloatRangedStrict);
TEST_F(TestDeltaCoder, FloatRangedStrict0Bits);
TEST_F(TestDeltaCoder, FloatRangedStrict33Bits);
*/

}}} // namespace
