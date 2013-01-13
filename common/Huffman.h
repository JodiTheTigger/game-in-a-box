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

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <cstdint>
#include <vector>
#include <array>
#include <memory>

// forward references
class BitStream;

// Based off http://rosettacode.org/wiki/Huffman_coding#C.2B.2B
class Huffman
{
public:
    Huffman(const std::array<uint64_t, 256>& frequencies);
    
    std::unique_ptr<std::vector<uint8_t>> Encode(const std::vector<uint8_t>& data) const;
    std::unique_ptr<std::vector<uint8_t>> Decode(const std::vector<uint8_t>& data) const;
    
private:
    // Class fails if the max code is greater than 16 bits.
    struct ValueAndBits
    {
        uint16_t value;
        uint8_t bits;
        
        ValueAndBits() : ValueAndBits(0,0) 
        {            
        }
        
        ValueAndBits(uint16_t newValue, uint8_t newBits) 
            : value(newValue)
            , bits(newBits)
        {                
        }
    };
    
    class Node
    {
    public:
        const uint64_t myFrequency;
        
        virtual ~Node() {}
    protected:
        Node(uint64_t frequency) : myFrequency(frequency) {}
    };
    
    class NodeInternal : public Node
    {
    public:
        std::unique_ptr<Node> myLeft;
        std::unique_ptr<Node> myRight;
        
        NodeInternal(Node* left, Node* right) 
            : Node(left->myFrequency + right->myFrequency)
            , myLeft(left)
            , myRight(right)
        {
        }        
    };
    
    class NodeLeaf : public Node
    {
    public:
        const uint8_t myByte;
        
        NodeLeaf(uint64_t frequency, uint8_t byte) 
            : Node(frequency)
            , myByte(byte)
        {
        }
    };    
    
    struct NodeCompare
    {
        bool operator()(const Node* left, const Node* right) const 
        {
            return (left->myFrequency > right->myFrequency);            
        }
    };
        
    std::array<ValueAndBits, 256> myEncodeMap;
    std::vector<std::vector<ValueAndBits>> myDecodeMap;
    
    void GenerateEncodeMap(const Node* node, ValueAndBits prefix);
    void GenerateDecodeMap();
    
    std::vector<uint16_t> Get9BitBytesStartingWith(uint16_t startValue, uint8_t bitSize);
};

#endif // HUFFMAN_H
