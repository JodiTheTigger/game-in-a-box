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

#include "Huffman.hpp"

#include <Common/MakeUnique.hpp>
#include <Common/BitStream.hpp>

#include <queue>
#include <stdexcept>
#include <map>
#include <string>
#include <iostream>
#include <bitset>
#include <string>

using namespace std;
using namespace GameInABox::Unused;
using GameInABox::Common::BitStream;
using GameInABox::Common::BitStreamReadOnly;

namespace GameInABox { namespace Unused {

class Node
{
public:
    enum class Mode
    {
        Eof,
        Internal,
        Leaf
    };

    const uint64_t frequency;
    const uint8_t value;
    const Mode mode;
    const Node* left;
    const Node* right;

    Node(uint64_t frequencyToUse, uint8_t byte)
          : Node(frequencyToUse, byte, Mode::Leaf, nullptr, nullptr)
    {
    }

    Node()
        : Node(1, 0, Mode::Eof, nullptr, nullptr)
    {

    }

    Node(Node* left, Node* right)
        : Node(left->frequency + right->frequency, 0, Mode::Internal, left, right)
    {
    }

private:

    Node(uint64_t frequency, uint8_t byte, Mode mode, Node* leftToUse, Node* rightToUse)
        : frequency(frequency)
        , value(byte)
        , mode(mode)
        , left(leftToUse)
        , right(rightToUse)
    {
    }
};

struct NodeCompare
{
    bool operator()(const Node* left, const Node* right) const
    {
        if (left->frequency == right->frequency)
        {
            // If the frequencies are the same, give precedence to
            // leaf nodes as they represent more common values.
            // I was only doing a single compare with the left node,
            // but that breaks strict weak ordering.
            return
                (
                    (left->mode == Node::Mode::Leaf) &&
                    (right->mode != Node::Mode::Leaf)
                );
        }
        else
        {
            return (left->frequency > right->frequency);
        }
    }
};

}} // namespace

Huffman::Huffman(const std::array<uint64_t, 256>& frequencies)
    : myEncodeMap()
    , myDecodeMap()
    , myEofMarker()
{    
    priority_queue<Node*, std::vector<Node*>, NodeCompare> trees;
    std::vector<unique_ptr<Node>> nodes;
 
    for (uint16_t i = 0; i < frequencies.size(); ++i)
    {
        // Note: if we encode 0 frequencies, then the table goes
        // 100% unbalanced and we end up having 32+ bits.
        if (frequencies[i] > 0)
        {
            nodes.emplace_back(make_unique<Node>(frequencies[i], (uint8_t) i));
            trees.push(nodes.back().get());
        }
    }
    
    // EOF is special, as it isn't a value as such.
    // I don't want to make 257 sized arrays!
    nodes.emplace_back(make_unique<Node>());
    trees.push(nodes.back().get());
    
    while (trees.size() > 1)
    {
        auto childLeft = trees.top();
        trees.pop();
 
        auto childRight = trees.top();
        trees.pop();
 
        nodes.emplace_back(make_unique<Node>(childLeft, childRight));
        trees.push(nodes.back().get());
    }
    
    // Generate the Encode Map
    GenerateEncodeMap(trees.top(), {});
    GenerateCanonicalEncodeMap();
    GenerateDecodeMap();
}

void Huffman::GenerateCanonicalEncodeMap()
{
    map<uint8_t, map<uint8_t, uint8_t>> sorted;
    uint8_t maxBits = 0;

    uint8_t i = 0;
    do
    {
        auto node = myEncodeMap[i];
        sorted[node.bits][i] = i;

        maxBits = std::max(node.bits, maxBits);
    }
    while (i++ != 255);

    // map is sorted, so the order it iterates is also ordered.
    uint16_t code = 0;
    for (auto bits = 0; bits <= maxBits; ++bits)
    {
        // Ignore 0 bit items, as they are invalid.
        // They are there because not all 256 bytes
        // have a non-zero frequency.
        if (bits == 0)
        {
            continue;
        }
        
        for (const auto& mapItem : sorted[bits])
        {
            myEncodeMap[mapItem.first].bits = bits;
            myEncodeMap[mapItem.first].value = code;

            code++;
        }

        code = (code << 1);

        // finally, the EOF marker (can be > 255, hence why not uint8_t).
        myEofMarker.value = code;
        myEofMarker.bits = bits + 1;
    }
}

void Huffman::GenerateEncodeMap(const Node* node, Huffman::ValueAndBits prefix)
{
    switch (node->mode)
    {
        case Node::Mode::Leaf:
        {
            myEncodeMap[node->value] = prefix;
            break;
        }

        case Node::Mode::Internal:
        {
            auto prefixLeft = prefix;
            auto prefixRight = prefix;

            prefixLeft.bits++;
            prefixLeft.value <<= 1;
            //prefixLeft.value |= 0;
            GenerateEncodeMap(node->left, prefixLeft);

            prefixRight.bits++;
            prefixRight.value <<= 1;
            prefixRight.value |= 1;

            GenerateEncodeMap(node->right, prefixRight);
            break;
        }

        case Node::Mode::Eof:
        {
            myEofMarker = prefix;
            break;
        }
    }
}

std::vector<uint16_t> Huffman::GetXBitBytesStartingWith(uint16_t startValue, uint8_t bitSize, uint8_t totalBitSize)
{
    vector<uint16_t> result;
    
    if ((bitSize < totalBitSize) && (bitSize > 0))
    {
        auto count = 1 << (totalBitSize - bitSize);
                
        startValue <<= (totalBitSize - bitSize);
        
        for (auto i = 0; i < count; i++)
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
    myDecodeMap.push_back({});
    myDecodeMap[0].resize(512);
    
    // 257 due to EOF marker.
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
            result = Huffman::EofValue;
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
            // shrink it down please.
            point.bits -= 9;
            
            auto top9 = point.value >> point.bits;
            auto bottom7 = point.value & ((1 << point.bits) - 1);
            thisIndex = indexLookup[top9];
            
            // map auto generates indicies that doesn't exist
            if (thisIndex == 0)
            {
                mapIndex++;
                
                // All second maps are 128 bytes (7 bits) to make
                // my life easier.
                myDecodeMap.push_back({});
                myDecodeMap[mapIndex].resize(1 << 7);                
                
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

std::vector<uint8_t> Huffman::Encode(const std::vector<uint8_t>& data) const
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

std::vector<uint8_t> Huffman::Decode(const std::vector<uint8_t>& data) const
{
    vector<uint8_t> result{};
    BitStreamReadOnly inBuffer(data);
    size_t lastPosition = 0;
   
    while (inBuffer.PositionReadBits() < (data.size() * 8))
    {
        auto bits9      = inBuffer.PullU16(9);
        auto codeWord   = myDecodeMap[0][bits9];
        
        if (codeWord.bits == 0)
        {
            // well, shit.
            throw std::logic_error("Decode map produced 0 bit codeword. Corrupt Stream.");
        }        
        
        if (codeWord.value < 256)
        {
            result.push_back(static_cast<uint8_t>(codeWord.value));
            inBuffer.Rewind(9 - codeWord.bits);
        }        
        else
        {
            if (codeWord.value == Huffman::EofValue)
            {
                // EOF marker!
                break;
            }
            
            auto index = static_cast<uint8_t>(codeWord.value - 256);
            
            // The second decode map is 7 bits, so read that many.
            bits9 = inBuffer.PullU8(7);
            codeWord = myDecodeMap[index][bits9];
            
            if (codeWord.value == Huffman::EofValue)
            {
                // EOF marker!
                break;
            }

			if (codeWord.value < 256)
			{            
                result.push_back(static_cast<uint8_t>(codeWord.value));
				inBuffer.Rewind(7 - codeWord.bits);
			}
			else
			{
				// Ugh.
                throw std::logic_error("Second round codeWord. Value is bigger than a uint8_t. No idea what happend, but it's wrong.");
			}
        }

        // Need to test if we're at the end of the buffer due to
        // a corrupt stream so we don't loop forever. Test this
        // by making sure we're always moving forward across the buffer.
        if (inBuffer.PositionReadBits() <= lastPosition)
        {
            break;
        }
        else
        {
            lastPosition = inBuffer.PositionReadBits();
        }
    }
    
    return std::move(result);
}
