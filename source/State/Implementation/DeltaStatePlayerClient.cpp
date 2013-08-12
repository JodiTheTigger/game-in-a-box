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

#include "DeltaStatePlayerClient.hpp"

#include <Common/BitStream.hpp>
#include <Common/BitStreamReadOnly.hpp>
#include <Common/MathConstants.hpp>

using GameInABox::Common::BitStream;
using GameInABox::Common::BitStreamReadOnly;

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Unit test please.
DeltaStatePlayerClient::DeltaStatePlayerClient(Research settings)
    : myCoder({
        std::vector<DeltaMapItem>
        {
            {MAKE_OFFSET(StatePlayerClient, orientation.x), MapFloatRangeStrict{-2 * Pi(), 2 * Pi(), 17_bits}},
            {MAKE_OFFSET(StatePlayerClient, orientation.y), MapFloatRangeStrict{-2 * Pi(), 2 * Pi(), 17_bits}},
            {MAKE_OFFSET(StatePlayerClient, orientation.z), MapFloatRangeStrict{-2 * Pi(), 2 * Pi(), 17_bits}},
            {MAKE_OFFSET(StatePlayerClient, flags), MapUnsigned{7_bits}}
        },
        settings})
{
}

std::vector<uint8_t> DeltaStatePlayerClient::operator()(
        const StatePlayerClient& base,
        const StatePlayerClient& target)
{
    // buffer size should be less than the player state size.
    BitStream bits(sizeof(StatePlayerClient));

    if  (
            (base.orientation.x == target.orientation.x) &&
            (base.orientation.y == target.orientation.y) &&
            (base.orientation.z == target.orientation.z) &&
            (base.flags == target.flags)
        )
    {
        // Same == 1 bit.
        bits.Push(true);
    }
    else
    {
        // Not same = 1 bit + encoding.
        bits.Push(false);

        // Delta code between last and current.
        myCoder.DeltaEncode(base, target, bits);
    }

    return bits.TakeBuffer();
}

StatePlayerClient DeltaStatePlayerClient::operator()(
        const StatePlayerClient& base,
        const std::vector<uint8_t>& payload)
{
    BitStreamReadOnly bits(payload);

    if (bits.Pull1Bit())
    {
        return base;
    }
    else
    {
        StatePlayerClient result;

        myCoder.DeltaDecode(base, result, bits);

        return result;
    }
}

}}} // namespace
