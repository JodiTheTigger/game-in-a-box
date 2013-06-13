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

#include "PacketDelta2.hpp"
#include "BufferSerialisation.hpp"

using namespace GameInABox::Common;
using namespace GameInABox::Common::Network;

bool PacketDelta2::IsPacket(const std::vector<uint8_t>& buffer)
{
    if (!buffer.empty())
    {
        if (buffer.size() >= MinimumPacketSize)
        {
            if ((buffer[0] != 0xFF) && (buffer[1] != 0xFF))
            {
                // Fragmented?
                if ((buffer[OffsetSequence] & MaskTopByteIsFragmented) == 0)
                {
                    // Server packet?
                    if (0 != (buffer[OffsetIsServerFlags] & MaskTopByteIsServerPacket))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

PacketDelta2::PacketDelta2(std::vector<uint8_t> rawData)
    : Packet(rawData)
{
}

PacketDelta2::PacketDelta2(
        Sequence sequence,
        Sequence sequenceAck,
        uint8_t sequenceAckDelta,
        std::vector<uint8_t> deltaPayload)
    : PacketDelta2()
{
    data.reserve(MinimumPacketSize + deltaPayload.size());

    auto inserter = back_inserter(data);
    Push(inserter, sequence.Value());
    Push(inserter, sequenceAck.Value());
    data.push_back(sequenceAckDelta);

    data[OffsetIsServerFlags] |= MaskTopByteIsServerPacket;

    data.insert(end(data), begin(deltaPayload), end(deltaPayload));
}

Sequence PacketDelta2::GetSequence() const
{
    if (IsValid())
    {
        uint16_t rawSequence;
        Pull(begin(data) + OffsetSequence, rawSequence);

        return Sequence(rawSequence & MaskSequence);
    }
    else
    {
        return Sequence(0);
    }
}

Sequence PacketDelta2::GetSequenceBase() const
{
    if (IsValid())
    {
        uint16_t to;
        Pull(begin(data) + OffsetSequence, to);
        to &= MaskSequenceAck;

        return Sequence{Sequence{to} - Sequence{data[OffsetDeltaBase]}};
    }
    else
    {
        return Sequence(0);
    }
}

Sequence PacketDelta2::GetSequenceAck() const
{
    if (IsValid())
    {
        uint16_t rawSequence;
        Pull(begin(data) + OffsetSequenceAck, rawSequence);

        return Sequence(rawSequence & MaskSequence);
    }
    else
    {
        return Sequence(0);
    }
}
