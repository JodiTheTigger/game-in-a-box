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

#ifndef PACKETSTRING_H
#define PACKETSTRING_H

#include <string>
#include "Packet.hpp"

namespace GameInABox { namespace Common { namespace Network {

template<Command TheCommand>
class PacketString : public Packet
{
public:
    PacketString(std::string message) : Packet(TheCommand)
    {
        copy(message.begin(), message.end(), back_inserter(myBuffer));
    }

    PacketString(std::vector<uint8_t> buffer) : Packet(buffer) {}

    virtual ~PacketString() {}

    virtual bool IsValid() const override
    {
        if (myBuffer.size() >= MinimumPacketSize)
        {
            if (Packet::GetCommand() == TheCommand)
            {
                return Packet::IsValid();
            }
        }

        return false;
    }

    std::string Message() const
    {
        return std::string(myBuffer.begin() + OffsetPayload, myBuffer.end());
    }

protected:
    static const std::size_t MinimumPacketSize = Packet::MinimumPacketSize;
    static const std::size_t OffsetPayload = 3;
};

}}} // namespace

#endif // PACKETSTRING_H
