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

#ifndef PACKETBUFFER_H
#define PACKETBUFFER_H

#include "PacketCommand.h"

template<PacketCommand::Command TheCommand>
class PacketBuffer : public PacketCommand
{
public:
    PacketBuffer() : PacketCommand(TheCommand) {}
    PacketBuffer(std::vector<uint8_t> buffer) : PacketCommand(buffer) {}

    virtual ~PacketBuffer() {}

    virtual bool IsValid() const override
    {
        if (myBuffer.size() >= MinimumPacketSize)
        {
            if (PacketCommand::GetCommand() == TheCommand)
            {
                return PacketCommand::IsValid();
            }
        }

        return false;
    }

    std::vector<uint8_t> GetBuffer()
    {
        std::vector<uint8_t> result;

        result.reserve(myBuffer.size() - OffsetPayload);

        result.assign(myBuffer.begin() + OffsetPayload, myBuffer.end());

        return result;
    }

protected:
    static const std::size_t MinimumPacketSize = PacketCommand::MinimumPacketSize;
    static const std::size_t OffsetPayload = 3;
};

#endif // PACKETBUFFER_H
