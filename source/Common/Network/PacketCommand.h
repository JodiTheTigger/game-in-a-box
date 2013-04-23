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

// RAM: Is this a normal class, base class, an interface, or an implementation class
// TODO: Make up your mind and implement it!
class PacketCommand
{
public:    
    // RAM: TODO: Should this be a sub-class, or just a normal class in the same namespace?
    // Oh, yea, and namespaces please!
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

    static PacketCommand::Command GetCommand(const std::vector<uint8_t>& bufferToCheck);

    // RAM: TODO: If making a base class we need a public constructor please.

    // ----------------------------------------

    // RAM: TODO: Explain wy this is public, or hide it please!
    std::vector<uint8_t> myBuffer;

    // ----------------------------------------

    virtual ~PacketCommand();

    // ----------------------------------------

    PacketCommand::Command GetCommand() const { return GetCommand(myBuffer); }

    // RAM: TODO: NVI Pattern please, make PrivateIsValid()
    virtual bool IsValid() const = 0;

protected:
    static const std::size_t MinimumPacketSize = 3;
    static const std::size_t OffsetCommand = 2;

    PacketCommand(std::vector<uint8_t> fromBuffer);
    PacketCommand(Command command);
};

}}} // namespace

#endif // PACKETCOMMAND_H
