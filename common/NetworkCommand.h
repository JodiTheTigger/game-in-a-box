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

#ifndef NETWORKCOMMAND_H
#define NETWORKCOMMAND_H

#include <memory>
#include <boost/asio/ip/udp.hpp>

// forward references
class NetworkPacket;

class NetworkCommand
{
public:
    static std::unique_ptr<NetworkCommand> FromData(NetworkPacket& packetData);

    enum class CommandType : uint8_t
    {
        Invalid = 0,
        Challenge,
        ChallengeResponse,
        Info,
        InfoResponse,
        Connect,
        // ConnectResponse doesn't exist, I just start sending deltas.
        
        // Argh, no easy way to tell if a uint8_t is a valid enum
        // Have to make sure they are all sequential from 0, and test
        // against this first. That or a huge convert case statement. Argh!
        CommandCount
    };
    
    uint32_t key;
    CommandType command;
    size_t dataOffset;    
    
    boost::asio::ip::udp::endpoint              address;
    std::unique_ptr<std::vector<uint8_t>>       data;

private:
    // 2: Sequence
    // 1: command
    // 4: key
    static const size_t MinimumSize = 2 + 1 + 4;
    
    NetworkCommand(NetworkPacket& packetData);
};

#endif // NETWORKCOMMAND_H
