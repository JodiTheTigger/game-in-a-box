/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012-2013 Richard Maxwell <jodi.the.tigger@gmail.com>
    
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

#ifndef PACKETCOMMAND_HPP
#define PACKETCOMMAND_HPP

#include "Packet.hpp"

namespace GameInABox { namespace Common { namespace Network {

template<Command TheCommand>
class PacketCommand : public Packet
{
public:
    static PacketCommand<TheCommand> WithBuffer(std::vector<uint8_t> payload) { return {TheCommand, payload}; }
    static PacketCommand<TheCommand> WithString(std::string payload) { return {TheCommand, payload}; }
    static bool IsPacket(const std::vector<uint8_t>& buffer) { return Packet::GetCommand(buffer) == TheCommand; }

    PacketCommand() : Packet(TheCommand) {}

    explicit PacketCommand(std::vector<uint8_t> fromBuffer) : Packet(fromBuffer) {}
    explicit PacketCommand(Common::Sequence sequence) : Packet(sequence, TheCommand) {}

    PacketCommand(const PacketCommand&) = default;
    PacketCommand(PacketCommand&&) = default;
    PacketCommand& operator=(const PacketCommand&) = default;
    PacketCommand& operator=(PacketCommand&&) = default;
    virtual ~PacketCommand() {}

    virtual bool IsValid() const override { return IsPacket(data); }

private:
    PacketCommand(Command command, std::vector<uint8_t> payload) : Packet({}, command, payload) {}
    PacketCommand(Command command, std::string payload) : Packet({}, command, payload) {}
};

}}} // namespace

#endif // PACKETCOMMAND_HPP
