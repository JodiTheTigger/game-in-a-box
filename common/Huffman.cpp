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
        // Note: if we encode 0 frequencies, then the table goes
        // 100% unbalanced and we end up having 32+ bits.
        if (frequencies[i] > 0)
        {
            trees.push(new NodeLeaf(frequencies[i], (uint8_t) i));
        }
    }
    
    // EOF is special, as it isn't a value as such.
    // I don't want to make 257 sized arrays!
    trees.push(new Node());
    
    while (trees.size() > 1)
    {
        Node* childLeft = trees.top();
        trees.pop();
 
        Node* childRight = trees.top();
        trees.pop();
 
        Node* parent = new NodeInternal(childLeft, childRight);
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
    size_t bits;
    
    for (int i = 0; i < 256; i++)
    {
        auto node = myEncodeMap[i];
        sorted[node.bits][i] = (uint8_t) i;
    }
    
    code = 0;
    for (bits = 0; bits < sorted.size(); bits++)
    {        
        // Ignore 0 bit items, as they are invalid.
        // They are there because not all 256 bytes
        // have a non-zero frequency.
        if (bits == 0)
        {
            continue;
        }
        
        for (auto mapItem : sorted[bits])
        {
            myEncodeMap[sorted[bits][mapItem.first]].bits = bits;
            myEncodeMap[sorted[bits][mapItem.first]].value = code;
            
            code++;
        }
        
        code = (code << 1);
    }    
    
    // finally, the EOF marker
    myEofMarker.value = code;
    myEofMarker.bits = bits;
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
        else
        {
            // EOF marker.
            myEofMarker = prefix;
        }
    }
}

std::vector<uint16_t> Huffman::GetXBitBytesStartingWith(uint16_t startValue, uint8_t bitSize, uint8_t totalBitSize)
{
    vector<uint16_t> result;
    
    if ((bitSize < totalBitSize) && (bitSize > 0))
    {
        uint16_t count;
        
        count = 1 << (totalBitSize - bitSize);
                
        startValue <<= (totalBitSize - bitSize);
        
        for (int i = 0; i < count; i++)
        {
            result.push_back(startValue | i);
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
    uint8_t mapIndex;
    map<uint16_t, uint8_t> indexLookup;
    
    // always have a index 0 map which is 9 bit.
    mapIndex = 0;
    myDecodeMap.push_back(vector<ValueAndBits>());
    myDecodeMap[0].resize(512);
    
    for (uint16_t result = 0; result < 257; result++)
    {
        vector<uint16_t> all9Bits;
        ValueAndBits point;
        uint8_t thisIndex;
        
        // EOF marker?
        if (result < 256)
        {            
            point = myEncodeMap[result];
        }
        else
        {
            point = myEofMarker;
            result = myEofValue;
        }
        
        // not used? ok.
        if (point.bits == 0)
        {
            continue;
        }
        
        if (point.bits > 16)
        {
            // Bollox!
            throw std::logic_error("Huffman map uses more than 16 bits (uses " + to_string(point.bits) + ").");
        }
         
        thisIndex = 0; 
        if (point.bits < 10)
        {
            thisIndex = 0;
            all9Bits = GetXBitBytesStartingWith(point.value, point.bits, 9);        
        }   
        else
        {
            uint16_t top9;
            uint8_t  bottom7;
            
            // shrink it down please.
            point.bits -= 9;
            
            top9 = point.value >> point.bits;
            bottom7 = point.value & ((1 << point.bits) - 1);
            thisIndex = indexLookup[top9];
            
            // map auto generates indicies that doesn't exist
            if (thisIndex == 0)
            {
                mapIndex++;
                
                // All second maps are 128 bytes (7 bits) to make
                // my life easier.
                myDecodeMap.push_back(std::vector<ValueAndBits>());
                myDecodeMap[mapIndex].resize(1 << 7);
                
                // RAM: debug - reset to zero?
                for (int a = 0; a < 128; a++)
                {
                    myDecodeMap[mapIndex][a].value = 0;
                    myDecodeMap[mapIndex][a].bits = 0;
                }
                
                thisIndex = mapIndex;
                indexLookup[top9] = mapIndex;
            }            
            
            // When the value is > 256, the bits are ignored.
            myDecodeMap[0][top9] = ValueAndBits(256 + thisIndex, 9);
            
            all9Bits = GetXBitBytesStartingWith(bottom7, point.bits, 7);        
        }
     
        for (auto byte : all9Bits)
        {
            myDecodeMap[thisIndex][byte].value = result;
            myDecodeMap[thisIndex][byte].bits = point.bits;
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
    
    // EOF
    encoded.Push(myEofMarker.value, myEofMarker.bits);
    
    return move(encoded.TakeBuffer());
}

std::unique_ptr<std::vector<uint8_t>> Huffman::Decode(const std::vector<uint8_t>& data) const
{
    unique_ptr<vector<uint8_t>> result;
    BitStreamReadOnly inBuffer(data);
    
    result.reset(new vector<uint8_t>());
   
    while ((inBuffer.PositionReadBits() / 8) < data.size())
    {
        ValueAndBits codeWord;
        uint16_t bits9;
        
        bits9 = inBuffer.PullU16(9);
        
        codeWord = myDecodeMap[0][bits9];
        
        if (codeWord.bits == 0)
        {
            // well, shit.
            throw std::logic_error("Decode map produced 0 bit codeword. Corrupt Stream.");
        }        
        
        if (codeWord.value < 256)
        {
            result->push_back(codeWord.value);
            inBuffer.Rewind(9 - codeWord.bits);
        }        
        else
        {   
            uint8_t bitsRead;
            uint8_t index;
            
            if (codeWord.value == myEofValue)
            {
                // EOF marker!
                break;
            }
            
            index = codeWord.value - 256;
            
            // The second decode map is 7 bits, so read that many.
            bitsRead = 7;
            
            bits9 = inBuffer.PullU8(bitsRead);            
            codeWord = myDecodeMap[index][bits9];
            
            if (codeWord.value == myEofValue)
            {
                // EOF marker!
                break;
            }
            
            result->push_back(codeWord.value);
            inBuffer.Rewind(7 - codeWord.bits);
        }
    }
    
    return result;
}
