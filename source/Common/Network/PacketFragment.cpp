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

using namespace GameInABox::Common;
using namespace GameInABox::Common::Network;

bool PacketFragment::IsPacket(const std::vector<uint8_t>& buffer)
{
    if (!buffer.empty())
    {
        if (buffer.size() >= MinimumPacketSizeFragment)
        {
            if ((buffer[OffsetSequence] & MaskTopByteIsFragmented) != 0)
            {
                if ((buffer[0] != 0xFF) && (buffer[1] != 0xFF))
                {
                    return true;
                }
            }
        }
    }

    return false;
}

PacketFragment::PacketFragment(std::vector<uint8_t> rawData)
    : Packet(rawData)
{
}

PacketFragment::PacketFragment(Sequence sequence,
        uint8_t fragmentIndex,
        Fragment fragment,
        std::vector<uint8_t> payload)
    : Packet()
{
    data.reserve(OffsetFragmentPayload + payload.size());

    auto inserter = back_inserter(data);
    Push(inserter, sequence.Value());
    data.push_back(fragmentIndex);

    data[OffsetSequence] |= MaskTopByteIsFragmented;

    if (fragment == Fragment::Last)
    {
        data[OffsetFragmentId] |= MaskIsLastFragment;
    }

    data.insert(end(data), begin(payload), end(payload));
}

Sequence PacketFragment::GetSequence() const
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


