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

WrappingCounter<uint16_t> PacketDelta::GetSequence()
{
    if (IsValid())
    {
        return WrappingCounter<uint16_t>(GetUint16(myBuffer, OffsetSequenceAck) & MaskIsServerPacket);
    }
    else
    {
        return WrappingCounter<uint16_t>(0);
    }
}

WrappingCounter<uint16_t> PacketDelta::GetBase()
{
    if (IsValid())
    {
        uint16_t base(GetUint16(myBuffer, OffsetSequenceAck) & MaskIsServerPacket);

        return WrappingCounter<uint16_t>(base - (myBuffer[OffsetDeltaBaseAndFlags] & MaskDeltaBase));
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
            if (0 == (myBuffer[OffsetSequenceAck] & MaskTopByteIsServerPacket))
            {
                // top bit set of ack == server packet
                if (myBuffer.size() >= MinimumPacketSizeServer)
                {
                    result = true;
                }
            }
            else
            {
                if (myBuffer.size() >= MinimumPacketSizeClient)
                {
                    result = true;
                }
            }
        }
    }

    return result;
}

bool PacketDelta::HasClientId() const
{
    if (IsValid())
    {
        return (0 == (myBuffer[OffsetSequenceAck] & MaskTopByteIsServerPacket));
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

uint16_t PacketDelta::GetUint16(const std::vector<uint8_t>& buffer, std::size_t offset)
{
    return uint16_t(buffer[offset]) |  uint16_t(buffer[offset + 1]);
}
