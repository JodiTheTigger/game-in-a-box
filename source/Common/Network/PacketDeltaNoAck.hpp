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

#ifndef PACKETDELTANOACK_HPP
#define PACKETDELTANOACK_HPP

#include "PacketType.hpp"

namespace GameInABox { namespace Common { namespace Network {

class PacketDeltaNoAck : public PacketType<Command::DeltaNoAck>
{
public:
    // Delta distance is stored as a byte.
    static constexpr std::size_t MaximumDeltaDistance() { return std::numeric_limits<uint8_t>::max(); }
    static bool IsPacket(const std::vector<uint8_t>& buffer);

    PacketDeltaNoAck() : PacketDeltaNoAck(std::vector<uint8_t>()) {}
    explicit PacketDeltaNoAck(std::vector<uint8_t> rawData);

    PacketDeltaNoAck(
            Sequence sequence,
            uint8_t sequenceAckDelta,
            std::vector<uint8_t> deltaPayload);

    PacketDeltaNoAck(const PacketDeltaNoAck&) = default;
    PacketDeltaNoAck(PacketDeltaNoAck&&) = default;
    PacketDeltaNoAck& operator=(const PacketDeltaNoAck&) = default;
    PacketDeltaNoAck& operator=(PacketDeltaNoAck&&) = default;
    virtual ~PacketDeltaNoAck() = default;

    // Values are undefined if !IsValid().
    Sequence GetSequenceBase() const;

    bool IsValid() const override { return IsPacket(data); }

    std::size_t OffsetPayload() const override { return MinimumPacketSize; }

protected:
    static const std::size_t OffsetDeltaBase = Packet::MinimumPacketSize;
    static const std::size_t MinimumPacketSize = OffsetDeltaBase + 1;
};

}}} // namespace

#endif // PACKETDELTANOACK_HPP
