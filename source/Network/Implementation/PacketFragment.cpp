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

#ifndef USING_PRECOMPILED_HEADERS
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "PacketFragment.hpp"
#include "BufferSerialisation.hpp"

using namespace GameInABox::Network::Implementation;

bool PacketFragment::IsPacket(const std::vector<uint8_t>& buffer)
{
    if (buffer.size() >= MinimumPacketSizeFragment)
    {
        if ((buffer[OffsetSequence] & MaskTopByteIsFragmented) != 0)
        {
            return true;
        }
    }

    return false;
}

PacketFragment::PacketFragment(std::vector<uint8_t> rawData)
    : Packet(rawData)
{
}

PacketFragment::PacketFragment(
        Sequence sequence,
        const std::vector<uint8_t>& payload,
        std::size_t maxPacketSize,
        uint8_t fragmentId)
    : Packet()
{
    if (fragmentId < MaskIsLastFragment)
    {
        auto size = (maxPacketSize - OffsetFragmentPayload);
        auto offset = size * fragmentId;

        if (offset < payload.size())
        {
            // tail end?
            if (offset + size >= payload.size())
            {
                size = payload.size() - offset;
                fragmentId |= MaskIsLastFragment;
            }

            // right, let's go.
            data.reserve(OffsetFragmentPayload + size);

            auto inserter = back_inserter(data);
            Push(inserter, sequence.Value());
            data.push_back(fragmentId);

            data[OffsetSequence] |= MaskTopByteIsFragmented;

            data.insert(end(data), begin(payload) + offset, begin(payload) + offset + size);
        }
    }
}

bool PacketFragment::IsValid() const
{
    return IsPacket(data);
}

bool PacketFragment::IsLastFragment() const
{
    if (IsValid())
    {
        return ((data[OffsetFragmentId] & MaskIsLastFragment) != 0);
    }
    else
    {
        return false;
    }
}

uint8_t PacketFragment::FragmentId() const
{
    if (IsValid())
    {
        return data[OffsetFragmentId] & (0xFF ^ MaskIsLastFragment);
    }
    else
    {
        return 0;
    }
}

std::size_t PacketFragment::OffsetPayload() const
{
    return OffsetFragmentPayload;
}


