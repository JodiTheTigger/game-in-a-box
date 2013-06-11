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

#ifndef PACKETFRAGMENT_HPP
#define PACKETFRAGMENT_HPP

#ifndef USING_PRECOMPILED_HEADERS
#endif

#include "Common/Sequence.hpp"
#include "Packet.hpp"

namespace GameInABox { namespace Common { namespace Network {

class PacketFragment : public Packet
{
public:
    enum class Fragment
    {
        Normal,
        Last
    };

    static bool IsPacket(const std::vector<uint8_t>& buffer);

    PacketFragment() : PacketFragment(std::vector<uint8_t>()) {}
    explicit PacketFragment(std::vector<uint8_t> rawData);

    PacketFragment(
            Sequence sequence,
            uint8_t fragmentIndex,
            Fragment fragment,
            std::vector<uint8_t> payload);

    PacketFragment(const PacketFragment&) = default;
    PacketFragment(PacketFragment&&) = default;
    PacketFragment& operator=(const PacketFragment&) = default;
    PacketFragment& operator=(PacketFragment&&) = default;
    virtual ~PacketFragment() = default;

    Sequence GetSequence() const;

    bool IsValid() const override;

    bool IsLastFragment() const;
    uint8_t FragmentId() const;

    std::size_t OffsetPayload() const override;

private:
    static const std::size_t OffsetSequence = 0;

    static const std::size_t OffsetFragmentId = 2;
    static const std::size_t OffsetFragmentPayload = 3;

    // Zero sized fragments are not allowed, why would you do that?
    static const std::size_t MinimumPacketSizeFragment = OffsetFragmentPayload + 1;

    static const uint8_t MaskIsLastFragment = 0x80;
    static const uint8_t MaskTopByteIsFragmented = 0x80;
    static const uint16_t MaskIsFragmented = MaskTopByteIsFragmented << 8;
    static const uint16_t MaskSequence = MaskIsFragmented - 1;
};

}}} // namespace

#endif // PACKETFRAGMENT_HPP
