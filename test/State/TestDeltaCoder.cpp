/*
    Game-in-a-box. Simple uint0 Person Shooter Network Game.
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
        , myuint0()
        , myuint1()
    {
    }

    virtual void SetUp()
    {
        myMap = std::vector<DeltaMapItem>
        {
            {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint0), MapUnsigned{8_bits}},
            {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint1), MapUnsigned{18_bits}},
            {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float3), MapFloatFull{}}
        };

        myIdentity = DeltaTester{{0,0,0}, 0, 0, 0, false, 0, 0, 0};
        myuint0 = DeltaTester{{0,0,0},1,2,3.141f,false,0x11223344,22.22,12392};
        myuint1 = DeltaTester{{1.0,0,-42.3423},128,0xFFFFFFFF,-1024.0f,true,-44,88.88,-1};
    }

protected:
    
    struct DeltaTester
    {
        float float03[3];
        uint32_t uint0;
        uint32_t uint1;
        float float3;
        bool ignored;
        int32_t int0;
        double ignored2;
        int32_t int1;
    };
    
    vector<DeltaMapItem> myMap;
    DeltaTester myIdentity;
    DeltaTester myuint0;
    DeltaTester myuint1;

    DeltaTester TestMapuint0Touint1(std::vector<DeltaMapItem> map);
};

// /////////////////////
// Helpers
// /////////////////////
TestDeltaCoder::DeltaTester TestDeltaCoder::TestMapuint0Touint1(std::vector<DeltaMapItem> map)
{
    DeltaTester result;
    BitStream stream(32);

    DeltaCoder<DeltaTester> coder(map, {true, true});

    coder.DeltaEncode(myuint0, myuint1, stream);
    coder.DeltaDecode(myuint0, result, stream);

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
    
    myCoder.DeltaEncode(myIdentity, myuint0, data);
    myCoder.DeltaDecode(myIdentity, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(myuint0.uint0, result.uint0);
    EXPECT_EQ(myuint0.uint1, result.uint1);
    EXPECT_EQ(myuint0.float3, result.float3);
}

TEST_F(TestDeltaCoder, EncodeDecodeFullDiff) 
{
    DeltaTester uint1;
    DeltaTester result;
    BitStream data(32);
        
    uint1.uint0 = 4;
    uint1.uint1 = 8;
    uint1.float3 = 1.7f;
        
    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myuint0, uint1, data);
    myCoder.DeltaDecode(myuint0, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(uint1.uint0, result.uint0);
    EXPECT_EQ(uint1.uint1, result.uint1);
    EXPECT_EQ(uint1.float3, result.float3);
}

TEST_F(TestDeltaCoder, EncodeDecodePartialDiff) 
{
    DeltaTester uint1;
    DeltaTester result;
    BitStream data(32);
        
    uint1.uint0 = 5;
    uint1.uint1 = myuint0.uint1;
    uint1.float3 = 1.7f;
        
    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myuint0, uint1, data);
    myCoder.DeltaDecode(myuint0, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(uint1.uint0, result.uint0);
    EXPECT_EQ(uint1.uint1, result.uint1);
    EXPECT_EQ(uint1.float3, result.float3);
}

TEST_F(TestDeltaCoder, EncodeAgainstIdentity) 
{
    BitStream data(32);

    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myIdentity, myuint0, data);
    
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
                
        ASSERT_EQ(states[to].uint0, result.uint0)
                << "From:" << ::testing::PrintToString(from)
                << " To: " << ::testing::PrintToString(to)
                << " i: " << i
                << "\nstate from: " << ::testing::PrintToString(states[from])
                << "\nstate to  : " << ::testing::PrintToString(states[to])
                << "\nresult    : " << ::testing::PrintToString(result);
        ASSERT_EQ(states[to].uint1, result.uint1);
        ASSERT_EQ(states[to].float3, result.float3);
    }
}

TEST_F(TestDeltaCoder, MapFloatFull)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[0]), MapFloatFull{}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[1]), MapFloatFull{}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[2]), MapFloatFull{}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float3), MapFloatFull{}},
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_FLOAT_EQ(myuint1.float03[0], result.float03[0]);
    EXPECT_FLOAT_EQ(myuint1.float03[1], result.float03[1]);
    EXPECT_FLOAT_EQ(myuint1.float03[2], result.float03[2]);
    EXPECT_FLOAT_EQ(myuint1.float3, result.float3);
}

TEST_F(TestDeltaCoder, MapUnsigned)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint0), MapUnsigned{6_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint1), MapUnsigned{31_bits}},
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint1.uint0 & 0x3F, result.uint0);
    EXPECT_EQ(myuint1.uint1 & 0x7FFFFFF, result.uint1);
    EXPECT_NE(myuint1.uint1 & 0xFFFFFFF, result.uint1);
}

TEST_F(TestDeltaCoder, MapUnsignedNegativeBits)
{
    auto map = std::vector<DeltaMapItem>
    {
        // RAM: TODO: Support compiling -8_bits!
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint0), MapUnsigned{{-8}}}
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint0.uint0, result.uint0);
}

TEST_F(TestDeltaCoder, MapUnsigned0Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint0), MapUnsigned{0_bits}}
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint0.uint0, result.uint0);
}

TEST_F(TestDeltaCoder, MapUnsigned33Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint1), MapUnsigned{33_bits}}
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint1.uint1, result.uint1);
}

TEST_F(TestDeltaCoder, MapSigned)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int0), MapSigned{6_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int1), MapSigned{31_bits}},
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint1.int0, result.int0);
    EXPECT_EQ(myuint1.int1, result.int1);
}

TEST_F(TestDeltaCoder, MapSignedNegativeBits)
{
    auto map = std::vector<DeltaMapItem>
    {
        // RAM: TODO: Support compiling -8_bits!
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int0), MapSigned{{-8}}}
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint0.int0, result.int0);
}

TEST_F(TestDeltaCoder, MapSigned0Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int0), MapSigned{0_bits}}
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint0.int0, result.int0);
}

TEST_F(TestDeltaCoder, MapSigned33Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int1), MapUnsigned{33_bits}}
    };

    auto result = TestMapuint0Touint1(map);

    EXPECT_EQ(myuint1.int1, result.int1);
}


/*
TEST_F(TestDeltaCoder, FloatRangedAsFloat);
TEST_F(TestDeltaCoder, FloatRangedAsInt);
TEST_F(TestDeltaCoder, FloatRanged0Max);
TEST_F(TestDeltaCoder, FloatRanged24bitMaxRange);
TEST_F(TestDeltaCoder, FloatRangedStrict);
TEST_F(TestDeltaCoder, FloatRangedStrict0Bits);
TEST_F(TestDeltaCoder, FloatRangedStrict33Bits);
*/

}}} // namespace
