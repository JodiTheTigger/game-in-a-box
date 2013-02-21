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

#ifndef PACKETKEY_H
#define PACKETKEY_H

#include "PacketCommand.h"

template<typename CommandType, CommandType TheCommand>
class PacketKey : public PacketCommand
{
public:
    PacketKey(uint32_t key)
        : PacketCommand(TheCommand)
    {
        // High byte first.
        myBuffer.push_back(uint8_t(key >> 24));
        myBuffer.push_back(uint8_t(key >> 16));
        myBuffer.push_back(uint8_t(key >> 8));
        myBuffer.push_back(uint8_t(key >> 0));
    }

    virtual ~PacketKey() {}

    virtual bool IsValid() const override
    {
        if (myBuffer.size() == (PayloadSize + MinimumPacketSize))
        {
            if (Command() == TheCommand)
            {
                if (Key() != 0)
                {
                    return true;
                }
            }
        }

        return false;
    }

    uint32_t Key() const
    {
        return  uint32_t(myBuffer[OffsetKey + 0] << 24) |
                uint32_t(myBuffer[OffsetKey + 2] << 16) |
                uint32_t(myBuffer[OffsetKey + 3] << 8)  |
                uint32_t(myBuffer[OffsetKey + 4]);
    }

protected:
    static const std::size_t PayloadSize = 4;
    static const std::size_t OffsetKey = 3;
};

#endif // PACKETKEY_H
