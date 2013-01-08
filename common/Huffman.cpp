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

Huffman::Huffman(std::unique_ptr<std::array<uint64_t, 256>> frequencies)
{    
    priority_queue<Node*, std::vector<Node*>, NodeCompare> trees;
 
    for (uint16_t i = 0; i < frequencies->size(); ++i)
    {
        if ((*frequencies)[i] != 0)
        {
            trees.push(new NodeLeaf((*frequencies)[i], (uint8_t) i));
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
    
    //return trees.top();
    // TODO: create EncodeMap!
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
