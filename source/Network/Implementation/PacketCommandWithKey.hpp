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

#ifndef PACKETCOMMANDWITHKEY_H
#define PACKETCOMMANDWITHKEY_H

#include <cstdint>
#include <string>
#include <vector>

#include "Packet.hpp"
#include "NetworkKey.hpp"

namespace GameInABox { namespace Network { namespace Implementation {

template<Command TheCommand>
class PacketCommandWithKey : public Packet
{
public:
    explicit PacketCommandWithKey(NetworkKey key)
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

    PacketCommandWithKey(NetworkKey key, std::string payloadString)
        : PacketCommandWithKey(key)
    {
        data.insert(end(data), begin(payloadString), end(payloadString));
    }

    PacketCommandWithKey(NetworkKey key, std::vector<uint8_t> payload)
        : PacketCommandWithKey(key)
    {
        data.insert(end(data), begin(payload), end(payload));
    }

    explicit PacketCommandWithKey(std::vector<uint8_t> buffer) : Packet(buffer) {}

    virtual ~PacketCommandWithKey() {}

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

    std::size_t OffsetPayload() const override { return PacketCommandWithKey::OffsetData; }

protected:
    static const std::size_t PayloadSize = 16;
    static const std::size_t OffsetKey = MinimumPacketSize;
    static const std::size_t OffsetData = OffsetKey + PayloadSize;
};

}}} // namespace

#endif // PACKETCOMMANDWITHKEY_H
