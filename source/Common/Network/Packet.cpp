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

#ifndef USING_PRECOMPILED_HEADERS
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "Packet.hpp"

using namespace std;
using namespace GameInABox::Common::Network;

Packet::Packet(std::vector<uint8_t> fromBuffer)
    : data(fromBuffer)
{
}

Packet::Packet(Command command)
    : Packet({0xFF, 0xFF, uint8_t(command)})
{
}

Packet::Packet(Command command, std::vector<uint8_t> payload) : Packet(command)
{
    data.insert(end(data), begin(payload), end(payload));
}

Packet::Packet(Command command, std::string payload) : Packet(command)
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
        if ((bufferToCheck[0] == 0xFF) && (bufferToCheck[1] == 0xFF))
        {
            auto value = bufferToCheck[OffsetCommand];

            if (value <= static_cast<uint8_t> (Command::Disconnect))
            {
                return static_cast<Command>(value);
            }
        }
    }

    return Command::Unrecognised;
}

bool Packet::IsValid() const
{
    return (GetCommand() != Command::Unrecognised);
}


std::vector<uint8_t> GameInABox::Common::Network::GetPayloadBuffer(const Packet& packet)
{
    std::vector<uint8_t> result{};

    if (packet.data.size() > packet.OffsetPayload())
    {
        result.reserve(packet.data.size() - packet.OffsetPayload());
        result.assign(begin(packet.data) + packet.OffsetPayload(), end(packet.data));
    }

    return result;
}


std::string GameInABox::Common::Network::GetPayloadString(const Packet& packet)
{
    std::string result{};

    if (packet.data.size() > packet.OffsetPayload())
    {
        result.reserve(packet.data.size() - packet.OffsetPayload());
        result = std::string{begin(packet.data) + packet.OffsetPayload(), end(packet.data)};
    }

    return result;
}
