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

#ifndef PACKETTYPE_HPP
#define PACKETTYPE_HPP

#include "Packet.hpp"

namespace GameInABox { namespace Common { namespace Network {

template<Command TheCommand>
class PacketType : public Packet
{
public:
    static PacketType<TheCommand> WithBuffer(std::vector<uint8_t> payload) { return {TheCommand, payload}; }
    static PacketType<TheCommand> WithString(std::string payload) { return {TheCommand, payload}; }
    static bool IsPacket(const std::vector<uint8_t>& buffer) { return Packet::GetCommand(buffer) == TheCommand; }

    PacketType() : Packet(TheCommand) {}

    explicit PacketType(std::vector<uint8_t> fromBuffer) : Packet(fromBuffer) {}
    explicit PacketType(Common::Sequence sequence) : Packet(sequence, TheCommand) {}

    PacketType(const PacketType&) = default;
    PacketType(PacketType&&) = default;
    PacketType& operator=(const PacketType&) = default;
    PacketType& operator=(PacketType&&) = default;
    virtual ~PacketType() {}

    virtual bool IsValid() const override { return IsPacket(data); }

private:
    PacketType(Command command, std::vector<uint8_t> payload) : Packet({}, command, payload) {}
    PacketType(Command command, std::string payload) : Packet({}, command, payload) {}
};

}}} // namespace

#endif // PACKETTYPE_HPP
