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

#ifndef PACKET_H
#define PACKET_H

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <vector>
#include <string>
#endif

namespace GameInABox { namespace Common { namespace Network {

enum class Command : uint8_t
{
    Unrecognised = 0,
    Challenge,
    ChallengeResponse,
    Info,
    InfoResponse,
    Connect,
    ConnectResponse,
    Disconnect,
};

class Packet
{
public:
    static Command GetCommand(const std::vector<uint8_t>& bufferToCheck);

    Packet() : Packet(std::vector<uint8_t>()) {}

    explicit Packet(std::vector<uint8_t> fromBuffer);
    explicit Packet(Command command);

    Packet(Command command, std::vector<uint8_t> payload);
    Packet(Command command, std::string payload);

    Packet(const Packet&) = default;
    Packet(Packet&&) = default;
    Packet& operator=(const Packet&) = default;
    Packet& operator=(Packet&&) = default;
    virtual ~Packet();

    Command GetCommand() const { return GetCommand(data); }
    virtual bool IsValid() const;

    virtual std::size_t OffsetPayload() const { return MinimumPacketSize; }

    std::vector<uint8_t> data;

protected:
    static const std::size_t MinimumPacketSize = 3;
    static const std::size_t OffsetCommand = 2;

};

std::vector<uint8_t> GetPayloadBuffer(const Packet& packet);
std::string GetPayloadString(const Packet& packet);

}}} // namespace

#endif // PACKET_H
