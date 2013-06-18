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

#include "PacketDelta.hpp"
#include "BufferSerialisation.hpp"

using namespace GameInABox::Common;
using namespace GameInABox::Common::Network;

bool PacketDelta::IsPacket(const std::vector<uint8_t>& buffer)
{
    if (buffer.size() >= MinimumPacketSize)
    {
        if ((buffer[OffsetIsFragmented] & MaskTopByteIsFragmented) == 0)
        {
            if ((buffer[OffsetIsCommand] & MaskTopByteIsCommand) == 0)
            {
                return true;
            }
        }
    }

    return false;
}

PacketDelta::PacketDelta(std::vector<uint8_t> rawData)
    : Packet(rawData)
{
}

PacketDelta::PacketDelta(
        Sequence sequence,
        boost::optional<Sequence> sequenceAck,
        uint8_t sequenceDelta,
        std::vector<uint8_t> deltaPayload)
    : PacketDelta()
{
    data.reserve(MinimumPacketSize + deltaPayload.size());

    auto inserter = back_inserter(data);
    Push(inserter, sequence.Value());
    if(sequenceAck)
    {
        Push(inserter, sequenceAck->Value());
    }
    else
    {
        Push(inserter, InvalidSequence);
    }
    data.push_back(sequenceDelta);

    data.insert(end(data), begin(deltaPayload), end(deltaPayload));
}

Sequence PacketDelta::GetSequenceBase() const
{
    if (data.size() >= MinimumPacketSize)
    {
        return Sequence{GetSequence() - Sequence{data[OffsetDeltaBase]}};
    }
    else
    {
        return {};
    }
}

boost::optional<Sequence> PacketDelta::GetSequenceAck() const
{
    if (data.size() >= MinimumPacketSize)
    {
        uint16_t rawSequence;
        Pull(begin(data) + OffsetSequenceAck, rawSequence);

        if (rawSequence != InvalidSequence)
        {
            return Sequence(rawSequence & MaskSequence);
        }
    }

    return {};
}

boost::optional<uint16_t> IdConnection(const PacketDelta& delta)
{
    if (delta.data.size() >= (delta.OffsetPayload() + 2))
    {
        if (delta.IsValid())
        {
            uint16_t result;

            Pull(begin(delta.data) + delta.OffsetPayload(), result);

            return {result};
        }
    }

    return {};
}

std::vector<uint8_t> ClientPayload(const PacketDelta& delta)
{
    if (delta.data.size() > (delta.OffsetPayload() + 2))
    {
        if (delta.IsValid())
        {
            return {begin(delta.data) + delta.OffsetPayload() + 2, end(delta.data)};
        }
    }

    return {};
}
