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

#include "Common/WrappingCounter.h"

#include <vector>

class PacketDelta
{
public:
    PacketDelta() : PacketDelta(std::vector<uint8_t>()) {}
    PacketDelta(std::vector<uint8_t> rawData);

    PacketDelta(
            WrappingCounter<uint16_t> sequence,
            WrappingCounter<uint16_t> sequenceAck,
            uint8_t sequenceAckDelta,
            uint16_t* clientId,
            std::vector<uint8_t> deltaPayload);

    PacketDelta(
            PacketDelta toFragment,
            std::size_t maxPacketSize,
            uint8_t fragmentId);

    // http://stackoverflow.com/questions/4421706/operator-overloading/4421719#4421719
    // Should be non static non method, but since it compares a non-public member, I'll keep it as a member.
    inline bool operator==(const PacketDelta& other) const {return this->myBuffer == other.myBuffer; }
    inline bool operator!=(const PacketDelta& other) const {return !operator==(other);}
    inline bool operator< (const PacketDelta& other) const {return (this->myBuffer < other.myBuffer);}
    inline bool operator> (const PacketDelta& other) const {return (this->myBuffer > other.myBuffer);}
    inline bool operator<=(const PacketDelta& other) const {return !operator>(other);}
    inline bool operator>=(const PacketDelta& other) const {return !operator<(other);}

    // Returns a valid delta only if all fragments to once sequence are supplied.
    // If different sequencies supplied interleaved, returns the last sequenced delta.
    // Even if complete delta could be made, won't be valid if it's interleaved with
    // a newer sequenced delta fragment (that isn't complete).
    // Ignores non-fragmented packets.
    PacketDelta(std::vector<PacketDelta> fragments);

    WrappingCounter<uint16_t> GetSequence() const;
    WrappingCounter<uint16_t> GetSequenceBase() const;
    WrappingCounter<uint16_t> GetSequenceAck() const;

    bool IsValid() const;    

    bool IsFragmented() const;
    bool IsLastFragment() const;
    uint8_t FragmentId() const;

    bool HasClientId() const;
    uint16_t ClientId() const;

    std::size_t Size() const { return myBuffer.size(); }
    std::vector<uint8_t> GetPayload() const;
    std::vector<uint8_t> TakeBuffer() { return move(myBuffer); }

    // TODO: once we have namespaces, take this out of the class.
    // Hmm, couldn't we just use streams for this somehow?
    static uint16_t GetUint16(const std::vector<uint8_t>& buffer, std::size_t offset);
    static void Push(std::vector<uint8_t>& buffer, uint16_t data);

private:
    // No, I'm not going to use a struct to determine offsets.
    static const std::size_t OffsetSequence = 0;
    static const std::size_t OffsetIsFragmented = OffsetSequence;
    static const std::size_t OffsetSequenceAck = 2;
    static const std::size_t OffsetIsServerFlags = 2;
    static const std::size_t OffsetDeltaBase = 4;
    static const std::size_t OffsetClientId = 5;
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

    std::vector<uint8_t> myBuffer;
};

#endif // PACKETDELTA_H
