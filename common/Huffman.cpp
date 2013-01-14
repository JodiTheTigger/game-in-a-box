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
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "Huffman.h"

#include <queue>
#include <stdexcept>
#include <map>

#include "BitStream.h"

using namespace std;

Huffman::Huffman(const std::array<uint64_t, 256>& frequencies)
{    
    priority_queue<Node*, std::vector<Node*>, NodeCompare> trees;
 
    for (uint16_t i = 0; i < frequencies.size(); ++i)
    {
        // Encode even 0 probability items (as 1) since we need a tree
        // with all 256 entires.
        // Note: if we encode 0 frequencies, then the table goes
        // 100% unbalanced and we end up having 32+ bits.
        trees.push(new NodeLeaf(frequencies[i] + 1, (uint8_t) i));
    }
    
    while (trees.size() > 1)
    {
        Node* childRight = trees.top();
        trees.pop();
 
        Node* childLeft = trees.top();
        trees.pop();
 
        Node* parent = new NodeInternal(childRight, childLeft);
        trees.push(parent);
    }
    
    // Generate the Encode Map
    GenerateEncodeMap(trees.top(), ValueAndBits(0,0));
    GenerateCanonicalEncodeMap();
    GenerateDecodeMap();
}

void Huffman::GenerateCanonicalEncodeMap()
{
    map<uint8_t, map<uint8_t, uint8_t>> sorted;
    uint16_t code;
    
    for (int i = 0; i < 256; i++)
    {
        auto node = myEncodeMap[i];
        sorted[node.bits][i] = (uint8_t) i;
    }
    
    code = 0;
    for (size_t bits = 0; bits < sorted.size(); bits++)
    {
        for (size_t j = 0; j < sorted[bits].size(); j++)
        {
            myEncodeMap[sorted[bits][j]].bits = bits;
            myEncodeMap[sorted[bits][j]].value = code;
            
            code++;
        }
        
        code = (code << 1);
    }    
}

void Huffman::GenerateEncodeMap(const Huffman::Node* node, Huffman::ValueAndBits prefix)
{
    const NodeLeaf* leaf            = dynamic_cast<const NodeLeaf*>(node);
    const NodeInternal* internal    = dynamic_cast<const NodeInternal*>(node);
    
    if (leaf != nullptr)
    {
        myEncodeMap[leaf->myByte] = prefix;
    }
    else 
    {
        if (internal != nullptr)
        {
            ValueAndBits prefixLeft;
            ValueAndBits prefixRight;
            
            prefixLeft = prefix;
            prefixRight = prefix;
            
            prefixLeft.bits++;
            prefixLeft.value <<= 1;
            //prefixLeft.value |= 0;
            GenerateEncodeMap(internal->myLeft.get(), prefixLeft);
            
            prefixRight.bits++;
            prefixRight.value <<= 1;
            prefixRight.value |= 1;
            GenerateEncodeMap(internal->myRight.get(), prefixRight);            
        }
    }
}

// Assume that the bits that are at the top of the 9 bits
std::vector<uint16_t> Huffman::Get9BitBytesStartingWith(uint16_t startValue, uint8_t bitSize)
{
    vector<uint16_t> result;
    
    if ((bitSize < 9) && (bitSize > 0))
    {
        uint8_t mask;
        uint8_t count;
        
        count = 1 << (9 - bitSize);
        mask = count - 1;
        startValue &= mask;
        
        for (int i = 0; i < count; i++)
        {
            result.push_back(startValue | count);
        }
    }
    else
    {
        result.push_back(startValue);
    }
    
    return result;
}

void Huffman::GenerateDecodeMap()
{
    uint8_t lookupCount;
    
    // always have a index 0 map which is 512 bit.
    lookupCount = 0;
    myDecodeMap.push_back(vector<ValueAndBits>());
    myDecodeMap[0].resize(512);
    
    for (int result = 0; result < 256; result++)
    {
        vector<uint16_t> all9Bits;
        ValueAndBits point;
        uint8_t thisIndex;
        
        point = myEncodeMap[result];
        
        if (point.bits > 16)
        {
            // well, shit.
            throw std::logic_error("Huffman map uses more than 16 bits (uses " + to_string(point.bits) + ").");
        }
         
        if (point.bits < 10)
        {
            thisIndex = 0;
            all9Bits = Get9BitBytesStartingWith(point.value, point.bits);
        }   
        else
        {
            // make a new lookup please.
            lookupCount++;
            myDecodeMap[0][point.value] = ValueAndBits(256 + lookupCount, point.bits);
            
            // make a new lookup please.
            lookupCount++;
            myDecodeMap.push_back(vector<ValueAndBits>());
            point.value >>= 9;
            point.bits -= 9;
            myDecodeMap[lookupCount].resize(1 << point.bits);
            
            all9Bits = Get9BitBytesStartingWith(point.value, point.bits);        
            thisIndex = lookupCount;
        }
        
        for (auto byte : all9Bits)
        {
            myDecodeMap[thisIndex][byte].value = result;
            myDecodeMap[thisIndex][byte].value = point.bits;
        }
    }
}

std::unique_ptr<std::vector<uint8_t>> Huffman::Encode(const std::vector<uint8_t>& data) const
{
    BitStream encoded(data.size());
    
    for (uint8_t byte : data)
    {
        encoded.Push(myEncodeMap[byte].value, myEncodeMap[byte].bits);
    }
    
    return encoded.TakeBuffer();
}

std::unique_ptr<std::vector<uint8_t>> Huffman::Decode(const std::vector<uint8_t>& data) const
{
    unique_ptr<vector<uint8_t>> result;
    BitStreamReadOnly inBuffer(data);
    
    while (inBuffer.PositionRead() < data.size())
    {
        ValueAndBits codeWord;
        uint16_t bits9;
        
        bits9 = inBuffer.PullU16(9);
        
        codeWord = myDecodeMap[0][bits9];
        
        if (codeWord.value < 256)
        {
            result->push_back(codeWord.value);
            inBuffer.Rewind(9 - codeWord.bits);
        }        
        else
        {
            uint8_t bitsRead;
            uint8_t index;
            
            index = codeWord.value - 256;
            bitsRead = codeWord.bits - 9;
            
            bits9 = inBuffer.PullU8(bitsRead);
            
            codeWord = myDecodeMap[index][bits9];
            result->push_back(codeWord.value);
            inBuffer.Rewind(bitsRead - codeWord.bits);            
        }
    }
    
    return result;
}
