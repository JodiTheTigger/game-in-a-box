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

#include "PacketDeltaClient.hpp"
#include "BufferSerialisation.hpp"

using namespace GameInABox::Common;
using namespace GameInABox::Common::Network;

bool PacketDeltaClient::IsPacket(const std::vector<uint8_t>& buffer)
{
    if (!buffer.empty())
    {
        if (buffer.size() >= MinimumPacketSizeClient)
        {
            if ((buffer[0] != 0xFF) && (buffer[1] != 0xFF))
            {
                // Fragmented?
                if ((buffer[OffsetSequence] & MaskTopByteIsFragmented) == 0)
                {
                    // Server packet?
                    if (0 == (buffer[OffsetIsServerFlags] & MaskTopByteIsServerPacket))
                    {
                        return true;
                    }
                }
            }
        }
    }

    return false;
}

PacketDeltaClient::PacketDeltaClient(std::vector<uint8_t> rawData)
    : PacketDelta(rawData)
{
}

PacketDeltaClient::PacketDeltaClient(
        Sequence sequence,
        Sequence sequenceAck,
        uint8_t sequenceAckDelta,
        uint16_t idConnection,
        std::vector<uint8_t> deltaPayload)
    : PacketDeltaClient()
{
    data.reserve(MinimumPacketSize + deltaPayload.size());

    auto inserter = back_inserter(data);
    Push(inserter, sequence.Value());
    Push(inserter, sequenceAck.Value());
    data.push_back(sequenceAckDelta);
    Push(inserter, idConnection);

    data[OffsetIsServerFlags] &= (0xFF ^ MaskTopByteIsServerPacket);

    data.insert(end(data), begin(deltaPayload), end(deltaPayload));
}


uint16_t PacketDeltaClient::IdConnection() const
{
    if (IsValid())
    {
        uint16_t id;

        Pull(begin(data) + OffsetConnectionId, id);
        return id;
    }

    return 0;
}

