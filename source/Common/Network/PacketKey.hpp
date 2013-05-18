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

#include "Packet.hpp"
#include "NetworkKey.hpp"

namespace GameInABox { namespace Common { namespace Network {

template<Command TheCommand>
class PacketKey : public Packet
{
public:
    PacketKey(NetworkKey key)
        : Packet(TheCommand)
    {
        // NetworkKey is internally stored as a byte array so
        // we don't need to worry about endianess.
        myBuffer.insert(myBuffer.end(), key.begin(), key.end());
    }    

    PacketKey(std::vector<uint8_t> buffer) : Packet(buffer) {}

    virtual ~PacketKey() {}

    virtual bool IsValid() const override
    {
        if (myBuffer.size() >= (PayloadSize + MinimumPacketSize))
        {
            if (GetCommand() == TheCommand)
            {
                if (!(Key().is_nil()))
                {
                    return true;
                }
            }
        }

        return false;
    }

    NetworkKey Key() const
    {
        NetworkKey result;        

        if (myBuffer.size() >= (PayloadSize + MinimumPacketSize))
        {
            std::copy(
                myBuffer.begin() + OffsetKey,
                myBuffer.begin() + OffsetKey + PayloadSize,
                result.data);
        }

        return result;
    }

protected:
    static const std::size_t PayloadSize = 16;
    static const std::size_t OffsetKey = MinimumPacketSize;
};

}}} // namespace

#endif // PACKETKEY_H
