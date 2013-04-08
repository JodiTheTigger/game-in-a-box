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

#include "PacketDelta.h"

PacketDelta::PacketDelta(std::vector<uint8_t> rawData)
    : myBuffer(rawData)
{
}

PacketDelta::PacketDelta(
        WrappingCounter<uint16_t> sequence,
        WrappingCounter<uint16_t> sequenceAck,
        uint8_t sequenceAckDelta,
        uint16_t* clientId,
        std::vector<uint8_t> deltaPayload)
    : PacketDelta()
{
    if (clientId != nullptr)
    {
        myBuffer.reserve(MinimumPacketSizeClient + deltaPayload.size());
    }
    else
    {
        myBuffer.reserve(MinimumPacketSizeServer + deltaPayload.size());
    }

    Push(myBuffer, sequence.Value());
    Push(myBuffer, sequenceAck.Value());
    myBuffer.push_back(sequenceAckDelta);

    if (clientId != nullptr)
    {
        Push(myBuffer, *clientId);
    }
    else
    {
        myBuffer[OffsetIsServerFlags] |= MaskTopByteIsServerPacket;
    }

    myBuffer.insert( myBuffer.end(), deltaPayload.begin(), deltaPayload.end() );
}

WrappingCounter<uint16_t> PacketDelta::GetSequence()
{
    if (IsValid())
    {
        return WrappingCounter<uint16_t>(GetUint16(myBuffer, OffsetSequence));
    }
    else
    {
        return WrappingCounter<uint16_t>(0);
    }
}

WrappingCounter<uint16_t> PacketDelta::GetSequenceBase()
{
    if (IsValid())
    {
        uint16_t base(GetUint16(myBuffer, OffsetSequenceAck) & MaskSequenceAck);

        return WrappingCounter<uint16_t>(base - myBuffer[OffsetDeltaBase]);
    }
    else
    {
        return WrappingCounter<uint16_t>(0);
    }
}

WrappingCounter<uint16_t> PacketDelta::GetSequenceAck()
{
    if (IsValid())
    {
        return WrappingCounter<uint16_t>(GetUint16(myBuffer, OffsetSequenceAck) & MaskSequenceAck);
    }
    else
    {
        return WrappingCounter<uint16_t>(0);
    }
}

bool PacketDelta::IsValid() const
{
    bool result(false);

    if (myBuffer.size() >= MinimumPacketSizeCommon)
    {
        if ((myBuffer[0] != 0xFF) && (myBuffer[1] != 0xFF))
        {
            // high byte first.
            if (0 == (myBuffer[OffsetIsServerFlags] & MaskTopByteIsServerPacket))
            {
                // top bit set of ack == server packet
                if (myBuffer.size() >= MinimumPacketSizeClient)
                {
                    result = true;
                }
            }
            else
            {
                if (myBuffer.size() >= MinimumPacketSizeServer)
                {
                    result = true;
                }
            }
        }
    }

    return result;
}


bool PacketDelta::IsFragmented() const
{
    if (IsValid())
    {
        return (0 != (myBuffer[OffsetIsFragmented] & MaskTopByteIsFragmented));
    }
    else
    {
        return false;
    }
}

bool PacketDelta::IsLastFragment() const
{
    if (IsFragmented())
    {
        // RAM: TODO!
        return false;
    }
    else
    {
        return false;
    }
}

uint8_t PacketDelta::FragmentId() const
{
    // RAM: TODO!
    return 0;
}

bool PacketDelta::HasClientId() const
{
    if (IsValid())
    {
        return (0 == (myBuffer[OffsetIsServerFlags] & MaskTopByteIsServerPacket));
    }
    else
    {
        return false;
    }
}

uint16_t PacketDelta::ClientId() const
{
    if (HasClientId())
    {
        return GetUint16(myBuffer, OffsetClientId);
    }
    else
    {
        return 0;
    }
}

std::vector<uint8_t> PacketDelta::GetPayload()
{
    if (IsValid())
    {
        if (HasClientId())
        {
            return std::vector<uint8_t>(myBuffer.begin() + OffsetDataClient, myBuffer.end());
        }
        else
        {
            return std::vector<uint8_t>(myBuffer.begin() + OffsetDataServer, myBuffer.end());
        }
    }
    else
    {
        return std::vector<uint8_t>();
    }
}

uint16_t PacketDelta::GetUint16(const std::vector<uint8_t>& buffer, std::size_t offset)
{
    return (uint16_t(buffer[offset]) << 8) |  uint16_t(buffer[offset + 1]);
}

void PacketDelta::Push(std::vector<uint8_t>& buffer, uint16_t data)
{
    // sets the iterator before incrementing it.
    buffer.push_back(uint8_t(data >> 8));
    buffer.push_back(uint8_t(data & 0xFF));
}
