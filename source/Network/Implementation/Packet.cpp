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

#include "Packet.hpp"

#include <Common/BufferSerialisation.hpp>

using namespace GameInABox::Common;

namespace GameInABox { namespace Network { namespace Implementation {

Packet::Packet(std::vector<uint8_t> fromBuffer)
    : data(fromBuffer)
{
}

Packet::Packet(Sequence sequence, Command command)
    : Packet()
{
    Push(back_inserter(data), sequence.Value());
    data.push_back(static_cast<uint8_t>(static_cast<uint8_t>(command) | MaskTopByteIsCommand));
}

Packet::Packet(Sequence sequence, Command command, std::vector<uint8_t> payload)
    : Packet(sequence, command)
{
    data.insert(end(data), begin(payload), end(payload));
}

Packet::Packet(Sequence sequence, Command command, std::string payload)
    : Packet(sequence, command)
{
    data.insert(end(data), begin(payload), end(payload));
}

Packet::~Packet()
{
}

Command Packet::GetCommand(const std::vector<uint8_t>& bufferToCheck)
{
    if (bufferToCheck.size() >= MinimumPacketSize)
    {
        if ((bufferToCheck[OffsetIsFragmented] & MaskTopByteIsFragmented) == 0)
        {
            if ((bufferToCheck[OffsetIsCommand] & MaskTopByteIsCommand) != 0)
            {
                auto value = bufferToCheck[OffsetCommand] & (0xFF ^ MaskTopByteIsCommand);

                if (value <= static_cast<uint8_t> (Command::LastValidCommand))
                {
                    return static_cast<Command>(value);
                }
            }
        }
    }

    return Command::Unrecognised;
}

bool Packet::IsPacket(const std::vector<uint8_t>& buffer)
{
    return (GetCommand(buffer) != Command::Unrecognised);
}

Sequence Packet::GetSequence(const std::vector<uint8_t>& bufferToCheck)
{
    if (bufferToCheck.size() >= MinimumPacketSize)
    {
        uint16_t result;

        Pull(begin(bufferToCheck), result);

        return Sequence(result & MaskSequence);
    }
    else
    {
        return {};
    }
}

bool Packet::IsValid() const
{
    return IsPacket(data);
}

std::vector<uint8_t> GetPayloadBuffer(const Packet& packet)
{
    std::vector<uint8_t> result{};

    if (packet.data.size() > packet.OffsetPayload())
    {
        result.reserve(packet.data.size() - packet.OffsetPayload());
        result.assign(begin(packet.data) + packet.OffsetPayload(), end(packet.data));
    }

    return result;
}


std::string GetPayloadString(const Packet& packet)
{
    std::string result{};

    if (packet.data.size() > packet.OffsetPayload())
    {
        result.reserve(packet.data.size() - packet.OffsetPayload());
        result = std::string{begin(packet.data) + packet.OffsetPayload(), end(packet.data)};
    }

    return result;
}

}}} // namespace
