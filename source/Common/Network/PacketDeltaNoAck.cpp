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

#include "PacketDeltaNoAck.hpp"
#include "BufferSerialisation.hpp"

using namespace GameInABox::Common;
using namespace GameInABox::Common::Network;

bool PacketDeltaNoAck::IsPacket(const std::vector<uint8_t>& buffer)
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

PacketDeltaNoAck::PacketDeltaNoAck(std::vector<uint8_t> rawData)
    : PacketType<Command::DeltaNoAck>(rawData)
{
}

PacketDeltaNoAck::PacketDeltaNoAck(
        Sequence, // RAM: TODO: FIX!  sequence,
        uint8_t sequenceDelta,
        std::vector<uint8_t> deltaPayload)
    : PacketDeltaNoAck()
{
    data.reserve(MinimumPacketSize + deltaPayload.size());
    data.push_back(sequenceDelta);

    data.insert(end(data), begin(deltaPayload), end(deltaPayload));
}

Sequence PacketDeltaNoAck::GetSequenceBase() const
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
