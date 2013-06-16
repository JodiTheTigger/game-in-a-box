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

#ifndef PACKETDELTACLIENT_HPP
#define PACKETDELTACLIENT_HPP

#include "PacketDelta.hpp"

namespace GameInABox { namespace Common { namespace Network {

// RAM: TODO
// remove this class, and add the following functions
//uint16_t GetClientId(const PacketDelta& delta);
//uint16_t GetClientId(const PacketDeltaNoAck& delta);
//std::vector<uint8_t> GetClientPayload(PacketDelta delta);
//std::vector<uint8_t> GetClientPayload(PacketDeltaNoAck delta);

class PacketDeltaClient : public PacketDelta
{
    // Wart: Since I want to avoid code duplicate, I make PacketDeltaClient "is-a" PacketDelta2
    // However even though the class "is-a", the data isn't so IsValid will return false.

public:
    static constexpr std::size_t MaximumDeltaDistance() { return PacketDelta::MaximumDeltaDistance(); }
    static bool IsPacket(const std::vector<uint8_t>& buffer);

    PacketDeltaClient() : PacketDeltaClient(std::vector<uint8_t>()) {}
    explicit PacketDeltaClient(std::vector<uint8_t> rawData);

    PacketDeltaClient(
            Sequence sequence,
            Sequence sequenceAck,
            uint8_t sequenceAckDelta,
            uint16_t idConnection,
            std::vector<uint8_t> deltaPayload);

    // Rule of 5 (class contents are just one vector, so use defaults).
    PacketDeltaClient(const PacketDeltaClient&) = default;
    PacketDeltaClient(PacketDeltaClient&&) = default;
    PacketDeltaClient& operator=(const PacketDeltaClient&) = default;
    PacketDeltaClient& operator=(PacketDeltaClient&&) = default;
    virtual ~PacketDeltaClient() = default;

    bool IsValid() const override { return IsPacket(data); }

    uint16_t IdConnection() const;

    std::size_t OffsetPayload() const override { return OffsetDataClient; }

private:
    // No, I'm not going to use a struct to determine offsets.
    static const std::size_t OffsetConnectionId = 5;
    static const std::size_t OffsetDataClient = 7;
    static const std::size_t MinimumPacketSizeClient = OffsetDataClient;
};

}}} // namespace

#endif // PACKETDELTACLIENT_HPP
