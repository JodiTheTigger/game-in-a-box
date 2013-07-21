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

#include <Implementation/Huffman.hpp>

#include <gtest/gtest.h>

#include <string>
#include <array>

using namespace std;

namespace GameInABox { namespace Network { namespace Implementation {

// Class definition!
class TestHuffman : public ::testing::Test 
{
public:
    TestHuffman()
        : ::testing::Test()
        , myTestBuffers()
    {

    }

    virtual void SetUp()
    {
        std::string testPhrase;
        int i;
        int j;
        int count;
        vector<uint8_t> frequencies;
        
        // Simple phrase so I could check the tree against online tree creators.
        testPhrase = "SILLY SALLY SAW SIXTYSIX THICK THISTLE STICKS";
        myTestBuffers.emplace_back(begin(testPhrase), end(testPhrase));

        testPhrase = "Silly Sally saw sixty-six thick thistle sticks";
        myTestBuffers.emplace_back(begin(testPhrase), end(testPhrase));
        
        // Do a Fibonacci Sequence to generate an unbalanced tree.
        // This is used to test codewords greater than 9 bits.
        count = 0;
        i = 0;
        j = 1;
        while (count < 12)
        {
            int sum;
            
            sum = i+j;
            
            for (int repeat = 0; repeat < sum; repeat++)
            {
                frequencies.push_back('A' + count);
            }
            
            i = j;
            j = sum;
            count++;
        }
        
        myTestBuffers.emplace_back(move(frequencies));
    }
    
protected:
    std::vector<std::vector<uint8_t>> myTestBuffers;
};

TEST_F(TestHuffman, TestBuffersSingle) 
{
    for (auto buffer : myTestBuffers)
    {
        // generate frequencies
        array<uint64_t, 256> frequencies = {{0}};
        
        for (uint8_t item : buffer)
        {
            frequencies[item]++;
        }
        
        Huffman toTest(frequencies);
        
        // Since I don't have frequencies for all 256 bytes
        // don't test buffers with other's trees.
        auto bufferToTest = buffer;
        {
            vector<uint8_t> encoded{};
            vector<uint8_t> decoded{};
            
            encoded = toTest.Encode(bufferToTest);
            decoded = toTest.Decode(encoded);
            
            EXPECT_EQ(bufferToTest, decoded);
        }        
    }
}

TEST_F(TestHuffman, TestBufferFull) 
{
    array<uint64_t, 256> frequencies = {{0}};
    int i;
    int j;
    
    // Make a full buffer for all bytes
    j = 29;
    for (i = 0; i < 256; i++)
    {
        frequencies[i] = (uint8_t) j;
        
        j <<= 1;
        j = j & 0x2A;
        j += i;
        j = j & 0xFF;
    }
    
    Huffman toTest(frequencies);
    
    // Until I have frequencies for all 256 bytes
    // don't test buffers with other's trees.
    for (auto bufferToTest : myTestBuffers)
    {
        vector<uint8_t> encoded;
        vector<uint8_t> decoded;

        encoded = toTest.Encode(bufferToTest);
        decoded = toTest.Decode(encoded);

        EXPECT_EQ(bufferToTest, decoded);
    }
}

TEST_F(TestHuffman, TestBufferInvalid)
{
    std::array<uint64_t, 256> frequenciesUseful;

    // This buffer parsed forever, it shouldn't have, it should have stopped.
    // It is invalid though.
    auto badArray = std::vector<uint8_t>{3,65,236,60};

    // all set or all zero more probable.
    frequenciesUseful.fill(1);

    // 8 bits
    frequenciesUseful[0x00] = 10;
    frequenciesUseful[0xFF] = 10;

    // 7 bits
    frequenciesUseful[0x7F] = 9;
    frequenciesUseful[0xFE] = 9;

    // 6 bits
    frequenciesUseful[0x3F] = 8;
    frequenciesUseful[0x7E] = 8;
    frequenciesUseful[0xFC] = 8;

    Huffman toTest(frequenciesUseful);

    // Shouldn't loop forever.
    toTest.Decode(badArray);
}


TEST_F(TestHuffman, TestBufferTwoBytes)
{
    std::array<uint64_t, 256> frequenciesUseful;

    // all set or all zero more probable.
    frequenciesUseful.fill(1);

    // 8 bits
    frequenciesUseful[0x00] = 10;
    frequenciesUseful[0xFF] = 10;

    // 7 bits
    frequenciesUseful[0x7F] = 9;
    frequenciesUseful[0xFE] = 9;

    // 6 bits
    frequenciesUseful[0x3F] = 8;
    frequenciesUseful[0x7E] = 8;
    frequenciesUseful[0xFC] = 8;

    Huffman toTest(frequenciesUseful);

    auto array = std::vector<uint8_t>(2);

    for (int i = 0; i < 65536; ++i)
    {
        array[0] = static_cast<uint8_t>(i & 0xFF);
        array[1] = static_cast<uint8_t>(i >> 8);

        auto coded = toTest.Encode(array);
        auto decoded = toTest.Decode(coded);

        ASSERT_EQ(array, decoded);
    }
}


}}} // namespace
