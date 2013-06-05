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

#ifndef PACKETDELTA_H
#define PACKETDELTA_H

#ifndef USING_PRECOMPILED_HEADERS
#include <vector>
#include <boost/optional.hpp>
#endif

#include "Common/Sequence.hpp"
#include "Packet.hpp"

namespace GameInABox { namespace Common { namespace Network {

class PacketDelta : public Packet
{
public:
    // Delta distance is stored as a byte.
    static constexpr std::size_t MaximumDeltaDistance() { return std::numeric_limits<uint8_t>::max(); }
    static bool IsPacketDelta(const std::vector<uint8_t>& buffer);

    PacketDelta() : PacketDelta(std::vector<uint8_t>()) {}
    explicit PacketDelta(std::vector<uint8_t> rawData);

    PacketDelta(
            Sequence sequence,
            Sequence sequenceAck,
            uint8_t sequenceAckDelta,
            boost::optional<uint16_t> clientId,
            std::vector<uint8_t> deltaPayload);

    PacketDelta(
            PacketDelta toFragment,
            std::size_t maxPacketSize,
            uint8_t fragmentId);

    // Returns a valid delta only if all fragments to one sequence are supplied.
    // If different sequencies supplied interleaved, returns the last sequenced delta.
    // Even if complete delta could be made, won't be valid if it's interleaved with
    // a newer sequenced delta fragment (that isn't complete).
    // Ignores non-fragmented packets.
    // NOTE: Buffer can be corrupted if someone spoofs the "last packet" with an
    // in correct packet size. Fix by sending the actual total packet length
    // along with the current packet length start with each packet (for 65k max
    // that's an extra 3 bytes header overhead per packet.)
    explicit PacketDelta(std::vector<PacketDelta> fragments);

    // Rule of 5 (class contents are just one vector, so use defaults).
    PacketDelta(const PacketDelta&) = default;
    PacketDelta(PacketDelta&&) = default;
    PacketDelta& operator=(const PacketDelta&) = default;
    PacketDelta& operator=(PacketDelta&&) = default;
    virtual ~PacketDelta() = default;

    // http://stackoverflow.com/questions/4421706/operator-overloading/4421719#4421719
    // Should be non static non method, but since it compares a non-public member, I'll keep it as a member.
    inline bool operator==(const PacketDelta& other) const {return this->data == other.data; }
    inline bool operator!=(const PacketDelta& other) const {return !operator==(other);}
    inline bool operator< (const PacketDelta& other) const {return (this->data < other.data);}
    inline bool operator> (const PacketDelta& other) const {return (this->data > other.data);}
    inline bool operator<=(const PacketDelta& other) const {return !operator>(other);}
    inline bool operator>=(const PacketDelta& other) const {return !operator<(other);}

    Sequence GetSequence() const;
    Sequence GetSequenceBase() const;
    Sequence GetSequenceAck() const;

    bool IsValid() const;    

    bool IsFragmented() const;
    bool IsLastFragment() const;
    uint8_t FragmentId() const;

    bool HasIdConnection() const;
    uint16_t IdConnection() const;

    // No checks are done here to make sure the packet is valid.
    // It's up the the consumer to check and trust the data.
    std::vector<uint8_t> GetPayload() const;
private:
    // No, I'm not going to use a struct to determine offsets.
    static const std::size_t OffsetSequence = 0;
    static const std::size_t OffsetIsFragmented = OffsetSequence;
    static const std::size_t OffsetSequenceAck = 2;
    static const std::size_t OffsetIsServerFlags = 2;
    static const std::size_t OffsetDeltaBase = 4;
    static const std::size_t OffsetConnectionId = 5;
    static const std::size_t OffsetDataClient = 7;
    static const std::size_t OffsetDataServer = 5;
    static const std::size_t MinimumPacketSizeClient = OffsetDataClient;
    static const std::size_t MinimumPacketSizeServer = OffsetDataServer;

    // Fragmented packets
    static const std::size_t OffsetFragmentId = 2;
    static const std::size_t OffsetFragmentPayload = 3;
    static const std::size_t MinimumPacketSizeFragment = OffsetFragmentPayload;
    static const uint8_t MaskIsLastFragment = 0x80;

    static const uint8_t MaskTopByteIsServerPacket = 0x80;
    static const uint8_t MaskTopByteIsFragmented = 0x80;
    static const uint16_t MaskIsServerPacket = MaskTopByteIsServerPacket << 8;
    static const uint16_t MaskIsFragmented = MaskTopByteIsFragmented << 8;
    static const uint16_t MaskSequenceAck = MaskIsServerPacket - 1;
    static const uint16_t MaskSequence = MaskIsFragmented - 1;

    static const std::size_t MinimumPacketSizeCommon = MinimumPacketSizeFragment;
};

}}} // namespace

#endif // PACKETDELTA_H
