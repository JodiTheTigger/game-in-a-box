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

#ifndef NETWORKFRAGMENT_H
#define NETWORKFRAGMENT_H

#include <cstdint>
#include <memory>
#include <vector>
#include <boost/asio/ip/udp.hpp>

// NOTE: Not casting memory into structs because I don't want to deal with packing and endianess issues.

// forward references
class NetworkPacket;

// Packet Format
// =============
// Info coded high byte first.
// 
// 00: uint16_t sequence (0xFFFF == Out of band command packet)
// if (sequence != 0xFFFF)
// {
// 02: uint16_t qport
// If (sequence & 0x8000)
// {
// 04: uint16_t fragment offset
// 06: uint16_t fragment total length in bytes
// 08: uint8_t[] data
// }
// else
// {
// 04: uint8_t[] data
// }
// }
// else
// {
// 04: uint32_t key
// 08: uint8_t command
// 09: uint8_t[] data
// }

class NetworkFragment
{
public:
    static std::unique_ptr<NetworkFragment> GetFragmentFromData(NetworkPacket& packetData);
    
    enum class Command : uint8_t
    {
        Invalid = 0,
        Challenge,
        ChallengeResponse,
        GetInfo,
        Connect,
        
        // Commands created by the IStateManager
        // RAM: TODO: NEEDED? State = 255
    };
    
    uint16_t SequenceGet() const;

    bool IsCommand() const;
    bool IsFragmented() const;
    
    // returns 0 if IsCommand() is true.
    uint16_t QPortGet() const;
    
    // returns 0 unless IsFragmented() is true.
    uint16_t FragmentOffset() const;
    uint16_t FragmentTotalSizeInBytes() const;
    
    // returns 0 unless IsCommand() is true.
    uint32_t KeyGet() const;
    Command CommandGet() const;
    
    boost::asio::ip::udp::endpoint AddressGet() const;
    
private:
    const size_t PacketSizeMinimum = 2 + 2 + 4;
    
    boost::asio::ip::udp::endpoint myAddress;
    std::unique_ptr<std::vector<uint8_t>> myData;
    uint8_t* myDataRaw;
        
    // It's assumed this is a valid fragment by the time this
    // is used.
    NetworkFragment(
        boost::asio::ip::udp::endpoint address,
        std::unique_ptr<std::vector<uint8_t>> data);
    
    uint16_t U16Get(size_t offsetInBytes) const;
    uint32_t U32Get(size_t offsetInBytes) const;
};

#endif // NETWORKFRAGMENT_H
