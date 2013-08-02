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
        , myBase()
        , myTarget()
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

        // Numbers chosen to excersise many things (sign, float as int, bit length).
        myBase = DeltaTester{{0,45.0,0},1,2,3.141f,false,0x11223344,22.22,12392};
        myTarget = DeltaTester{{1.0,-88.0,-42.3423},128,0xFFFFFFFF,-1024.0f,true,-44,88.88,-1};
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
    DeltaTester myBase;
    DeltaTester myTarget;

    DeltaTester TestMapBaseToTarget(std::vector<DeltaMapItem> map);
};

// /////////////////////
// Helpers
// /////////////////////
TestDeltaCoder::DeltaTester TestDeltaCoder::TestMapBaseToTarget(std::vector<DeltaMapItem> map)
{
    DeltaTester result = {{0,0,0}, 0, 0, 0, false, 0, 0, 0};
    BitStream stream(32);

    DeltaCoder<DeltaTester> coder(map, {true, true});

    coder.DeltaEncode(myBase, myTarget, stream);
    coder.DeltaDecode(myBase, result, stream);

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
    
    myCoder.DeltaEncode(myIdentity, myBase, data);
    myCoder.DeltaDecode(myIdentity, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(myBase.uint0, result.uint0);
    EXPECT_EQ(myBase.uint1, result.uint1);
    EXPECT_EQ(myBase.float3, result.float3);
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
    
    myCoder.DeltaEncode(myBase, uint1, data);
    myCoder.DeltaDecode(myBase, result, data);
    
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
    uint1.uint1 = myBase.uint1;
    uint1.float3 = 1.7f;
        
    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myBase, uint1, data);
    myCoder.DeltaDecode(myBase, result, data);
    
    // CBF defining a operator== for my struct.
    EXPECT_EQ(uint1.uint0, result.uint0);
    EXPECT_EQ(uint1.uint1, result.uint1);
    EXPECT_EQ(uint1.float3, result.float3);
}

TEST_F(TestDeltaCoder, EncodeAgainstIdentity) 
{
    BitStream data(32);

    DeltaCoder<DeltaTester> myCoder(myMap, {true, true});
    
    myCoder.DeltaEncode(myIdentity, myBase, data);
    
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

    auto result = TestMapBaseToTarget(map);

    EXPECT_FLOAT_EQ(myTarget.float03[0], result.float03[0]);
    EXPECT_FLOAT_EQ(myTarget.float03[1], result.float03[1]);
    EXPECT_FLOAT_EQ(myTarget.float03[2], result.float03[2]);
    EXPECT_FLOAT_EQ(myTarget.float3, result.float3);
}

TEST_F(TestDeltaCoder, MapUnsigned)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint0), MapUnsigned{6_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint1), MapUnsigned{31_bits}},
    };

    auto result = TestMapBaseToTarget(map);

    // Note that the result is always based on the base when actually delta'd,
    // so Only test the bits you care about, the other bits are "don't care" NOT "0".
    EXPECT_EQ(myTarget.uint0 & 0x3F, result.uint0 & 0x3F);
    EXPECT_EQ(myTarget.uint1 & 0x7FFFFFF, result.uint1 & 0x7FFFFFF);
}

TEST_F(TestDeltaCoder, MapUnsignedNegativeBits)
{
    auto map = std::vector<DeltaMapItem>
    {
        // RAM: TODO: Support compiling -8_bits!
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint0), MapUnsigned{{-8}}}
    };

    auto result = TestMapBaseToTarget(map);

    // However, ignored items (for this test) are untouched, so are whatever
    // you initially intilised the structure to. For use that's 0 for everything.
    EXPECT_EQ(0, result.uint0);
}

TEST_F(TestDeltaCoder, MapUnsigned0Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint0), MapUnsigned{0_bits}}
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_EQ(0, result.uint0);
}

TEST_F(TestDeltaCoder, MapUnsigned33Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, uint1), MapUnsigned{33_bits}}
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_EQ(myTarget.uint1, result.uint1);
}

TEST_F(TestDeltaCoder, MapSigned)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int0), MapSigned{8_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int1), MapSigned{31_bits}},
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_EQ(myTarget.int0, result.int0);
    EXPECT_EQ(myTarget.int1, result.int1);
}

TEST_F(TestDeltaCoder, MapSignedNegativeBits)
{
    auto map = std::vector<DeltaMapItem>
    {
        // RAM: TODO: Support compiling -8_bits!
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int0), MapSigned{{-8}}}
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_EQ(0, result.int0);
}

TEST_F(TestDeltaCoder, MapSigned0Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int0), MapSigned{0_bits}}
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_EQ(0, result.int0);
}

TEST_F(TestDeltaCoder, MapSigned33Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, int1), MapUnsigned{33_bits}}
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_EQ(myTarget.int1, result.int1);
}

TEST_F(TestDeltaCoder, MapFloatRangedAsFloat)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[2]), MapFloatRanged{10000}},
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_FLOAT_EQ(myTarget.float03[2], result.float03[2]);
}

TEST_F(TestDeltaCoder, MapFloatRangedAsInt)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[0]), MapFloatRanged{4}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[1]), MapFloatRanged{127}}
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_FLOAT_EQ(myTarget.float03[0], result.float03[0]);
    EXPECT_FLOAT_EQ(myTarget.float03[1], result.float03[1]);
}

TEST_F(TestDeltaCoder, MapFloatRanged0Max)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[0]), MapFloatRanged{0}},
    };

    auto result = TestMapBaseToTarget(map);

    // 0 range, so ignored.
    EXPECT_FLOAT_EQ(0, result.float03[0]);
}

TEST_F(TestDeltaCoder, MapFloatRanged24bitMaxRange)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[0]), MapFloatRanged{40000000}},
    };

    auto result = TestMapBaseToTarget(map);

    EXPECT_FLOAT_EQ(myTarget.float03[0], result.float03[0]);
}

TEST_F(TestDeltaCoder, MapFloatRangedStrict)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[0]), MapFloatRangeStrict{0,1,6_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[1]), MapFloatRangeStrict{-100,50,8_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[2]), MapFloatRangeStrict{-100,0,16_bits}},
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float3), MapFloatRangeStrict{-2000,2000,13_bits}},
    };

    auto result = TestMapBaseToTarget(map);

    // RAM: TODO: Due to less bits to encode, the error might be quite big,
    // so figure out the correct range to allow.
    EXPECT_FLOAT_EQ(myTarget.float03[0], result.float03[0]);
    EXPECT_FLOAT_EQ(myTarget.float03[1], result.float03[1]);
    EXPECT_FLOAT_EQ(myTarget.float03[2], result.float03[2]);
    EXPECT_FLOAT_EQ(myTarget.float3, result.float3);
}

TEST_F(TestDeltaCoder, MapFloatRangedStrict0Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[1]), MapFloatRangeStrict{-100,50,0_bits}},
    };

    auto result = TestMapBaseToTarget(map);

    // 0 bits, so ignore.
    EXPECT_FLOAT_EQ(0, result.float03[1]);
}

TEST_F(TestDeltaCoder, MapFloatRangedStrict33Bits)
{
    auto map = std::vector<DeltaMapItem>
    {
        {MAKE_OFFSET(TestDeltaCoder::DeltaTester, float03[1]), MapFloatRangeStrict{-100,50,33_bits}},
    };

    auto result = TestMapBaseToTarget(map);

    // 33 bits should be treated as 32 bits.
    EXPECT_FLOAT_EQ(myTarget.float03[1], result.float03[1]);
}

}}} // namespace
