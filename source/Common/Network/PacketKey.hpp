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

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <string>
#include <vector>
#endif

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
        // Dependent name lookup for C++ templates
        // is something I wish I didn't have to know about.
        using std::begin;
        using std::end;

        // NetworkKey is internally stored as a byte array so
        // we don't need to worry about endianess.
        data.insert(end(data), begin(key), end(key));
    }

    PacketKey(NetworkKey key, std::string message)
        : PacketKey(key)
    {
        data.insert(end(data), begin(message), end(message));
    }

    PacketKey(NetworkKey key, std::vector<uint8_t> payload)
        : PacketKey(key)
    {
        data.insert(end(data), begin(payload), end(payload));
    }

    PacketKey(std::vector<uint8_t> buffer) : Packet(buffer) {}

    virtual ~PacketKey() {}

    virtual bool IsValid() const override
    {
        if (data.size() >= (PayloadSize + MinimumPacketSize))
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

    void Append(std::vector<uint8_t> toAppend)
    {
        data.insert(end(data), begin(toAppend), end(toAppend));
    }

    NetworkKey Key() const
    {
        NetworkKey result;        

        if (data.size() >= (PayloadSize + MinimumPacketSize))
        {
            std::copy(
                begin(data) + OffsetKey,
                begin(data) + OffsetKey + PayloadSize,
                result.data);
        }

        return result;
    }

    std::vector<uint8_t> GetBuffer() const
    {
        std::vector<uint8_t> result;

        result.reserve(data.size() - OffsetPayload);

        result.assign(begin(data) + OffsetPayload, end(data));

        return result;
    }

    std::string Message() const
    {
        return std::string(begin(data) + OffsetPayload, end(data));
    }

protected:
    static const std::size_t PayloadSize = 16;
    static const std::size_t OffsetKey = MinimumPacketSize;
    static const std::size_t OffsetPayload = OffsetKey + PayloadSize;
};

}}} // namespace

#endif // PACKETKEY_H
