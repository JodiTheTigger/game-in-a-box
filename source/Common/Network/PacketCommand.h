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

#ifndef PACKETCOMMAND_H
#define PACKETCOMMAND_H

#include <cstdint>
#include <vector>

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
    Disconnect
};

class PacketCommand
{
public:
    static Command GetCommand(const std::vector<uint8_t>& bufferToCheck);

    PacketCommand(std::vector<uint8_t> fromBuffer);
    PacketCommand(Command command);
    virtual ~PacketCommand();

    Command GetCommand() const { return GetCommand(myBuffer); }
    virtual bool IsValid() const;

    std::size_t Size() const { return myBuffer.size(); }
    std::vector<uint8_t> TakeBuffer() { return move(myBuffer); }

protected:
    static const std::size_t MinimumPacketSize = 3;
    static const std::size_t OffsetCommand = 2;

    std::vector<uint8_t> myBuffer;
};

}}} // namespace

#endif // PACKETCOMMAND_H
