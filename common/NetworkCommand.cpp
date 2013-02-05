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

#include "NetworkCommand.h"
#include "NetworkPacket.h"

using namespace std;

// Packet Format
// =============
// Info coded high byte first.
// 
// 00: uint16_t sequence (0xFFFF == Out of band command packet)
// 02: uint32_t key
// 06: uint8_t command
// 07: uint8_t[] data

std::unique_ptr<NetworkCommand> NetworkCommand::FromData(NetworkPacket& packetData)
{
    unique_ptr<NetworkCommand> result;
 
    if (packetData.data)
    {
        vector<uint8_t>& rawData = *(packetData.data.get());
    
        if (rawData.size() >= NetworkCommand::MinimumSize)
        {
            if ((rawData[0] == 0xFF) || (rawData[1] == 0xFF))
            {
                if (rawData[6] < (uint8_t) CommandType::CommandCount)
                {
                    // good to go
                    result.reset(new NetworkCommand(packetData));
                }
            }
        }
    }
    
    return result;
}

NetworkCommand::NetworkCommand(NetworkPacket& packetData)
: address(packetData.address)
, data(move(packetData.data))
{
    vector<uint8_t>& rawData = *(data.get());
    
    command = (CommandType) rawData[6];
    key = 
        (((uint32_t) rawData[2 + 0]) << 24) | 
        (((uint32_t) rawData[2 + 1]) << 16) | 
        (((uint32_t) rawData[2 + 2]) << 8) | 
        (((uint32_t) rawData[2 + 3]));
}
