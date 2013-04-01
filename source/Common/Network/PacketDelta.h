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

#include "WrappingCounter.h"

#include <vector>

class PacketDelta
{
public:
    PacketDelta() : PacketDelta(std::vector<uint8_t>()) {}
    PacketDelta(std::vector<uint8_t> rawData);

    WrappingCounter<uint16_t> GetSequence();
    WrappingCounter<uint16_t> GetBase();

    bool IsValid() const;
    bool HasClientId() const;
    uint16_t ClientId() const;

private:
    static const std::size_t MinimumPacketSize = 3;
    static const std::size_t OffsetSequenceAck = 2;
    static const std::size_t OffsetDeltaBaseAndFlags = 4;
    static const std::size_t OffsetClientId = 5;
    static const std::size_t OffsetDataClient = 6;
    static const std::size_t OffsetDataServer = 5;
};

#endif // PACKETDELTA_H
