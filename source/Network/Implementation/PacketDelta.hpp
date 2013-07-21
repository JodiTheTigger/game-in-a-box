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

#ifndef PACKETDELTA_HPP
#define PACKETDELTA_HPP

#include "Sequence.hpp"
#include "Packet.hpp"

#include <boost/optional.hpp>

namespace GameInABox { namespace Network { namespace Implementation {

class PacketDelta : public Packet
{
public:
    // Delta distance is stored as a byte.
    static constexpr std::size_t MaximumDeltaDistance() { return std::numeric_limits<uint8_t>::max(); }
    static bool IsPacket(const std::vector<uint8_t>& buffer);

    PacketDelta() : PacketDelta(std::vector<uint8_t>()) {}
    explicit PacketDelta(std::vector<uint8_t> rawData);

    PacketDelta(
            Sequence sequence,
            boost::optional<Sequence> sequenceAck,
            uint8_t sequenceDelta,
            std::vector<uint8_t> deltaPayload);

    // Client Delta
    PacketDelta(
            Sequence sequence,
            boost::optional<Sequence> sequenceAck,
            uint8_t sequenceDelta,
            uint16_t idConnection,
            std::vector<uint8_t> deltaPayload);

    // Rule of 5 (class contents are just one vector, so use defaults).
    PacketDelta(const PacketDelta&) = default;
    PacketDelta(PacketDelta&&) = default;
    PacketDelta& operator=(const PacketDelta&) = default;
    PacketDelta& operator=(PacketDelta&&) = default;
    virtual ~PacketDelta() = default;

    // Values are undefined if !IsValid().
    Sequence GetSequenceBase() const;
    boost::optional<Sequence> GetSequenceAck() const;

    bool IsValid() const override { return IsPacket(data); }

    std::size_t OffsetPayload() const override { return OffsetData; }

protected:
    // No, I'm not going to use a struct to determine offsets.
    static const std::size_t OffsetSequenceAck = 2;
    static const std::size_t OffsetIsServerFlags = 2;
    static const std::size_t OffsetDeltaBase = 4;
    static const std::size_t OffsetData = 5;
    static const std::size_t MinimumPacketSize = OffsetData;

    static const uint16_t InvalidSequence = 0xFFFF;
    static const uint16_t MaskSequenceAck = MaskSequence;

private:
    explicit PacketDelta(
            std::size_t payloadSize,
            Sequence sequence,
            boost::optional<Sequence> sequenceAck,
            uint8_t sequenceDelta);
};

boost::optional<uint16_t> IdConnection(const PacketDelta& delta);
std::vector<uint8_t> ClientPayload(const PacketDelta& delta);

}}} // namespace

#endif // PACKETDELTA_HPP
