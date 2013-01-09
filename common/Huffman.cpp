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

#include "BitStream.h"

using namespace std;

Huffman::Huffman(const std::array<uint64_t, 256>& frequencies)
{    
    priority_queue<Node*, std::vector<Node*>, NodeCompare> trees;
 
    for (uint16_t i = 0; i < frequencies.size(); ++i)
    {
        if (frequencies[i] != 0)
        {
            trees.push(new NodeLeaf(frequencies[i], (uint8_t) i));
        }
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
    
    //return trees.top();
    // TODO: create EncodeMap!
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

std::unique_ptr<std::vector<uint8_t>> Huffman::Encode(const std::vector<uint8_t>& data) const
{
    BitStream encoded(data.size());
    
    for (uint8_t byte : data)
    {
        encoded.Push(myEncodeMap[byte].value, myEncodeMap[byte].bits);
    }
    
    return encoded.TakeBuffer();
}

std::unique_ptr<std::vector<uint8_t>> Huffman::Decode(const std::vector<uint8_t>& ) const
{
    unique_ptr<vector<uint8_t>> result;
    
    return result;
}
