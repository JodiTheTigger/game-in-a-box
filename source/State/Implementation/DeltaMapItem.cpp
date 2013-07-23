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

#include "DeltaMapItem.hpp"

#include "DeltaMapItemInternal.hpp"

#include <Common/MakeUnique.hpp>

namespace GameInABox { namespace State { namespace Implementation {

DeltaMapItem::~DeltaMapItem()
{
    // Empty.
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse)
    : myPimpl(make_unique<DeltaMapItemInternal>())
{
    myPimpl->name = offsetToUse.name;
    myPimpl->offset = offsetToUse.offset;
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatFull)
    : DeltaMapItem(offsetToUse)
{
    myPimpl->type = MapType::FloatFull;
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapSigned specs)
    : DeltaMapItem(offsetToUse, MapUnsigned{specs.resolution})
{
    myPimpl->type = MapType::Signed;
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapUnsigned specs)
    : DeltaMapItem(offsetToUse)
{
    if (specs.resolution.value < 1)
    {
        myPimpl->type = MapType::Ignore;
    }
    else
    {
        myPimpl->bits = specs.resolution.value;
        if (myPimpl->bits > 32)
        {
            myPimpl->bits = 32;
        }
    }
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatRanged specs)
    : DeltaMapItem(offsetToUse)
{
    if (specs.maxValue > 0)
    {
        // figure out the range in bits.
        Bits resolution;

        while ((1U << resolution.value) < (2U*specs.maxValue))
        {
            ++(resolution.value);
        }

        // float has 23 bits of mantessa, no point doing this is we require a larger dynamic range.
        if (resolution.value > 22)
        {
            myPimpl->type = MapType::FloatFull;
        }
        else
        {
            myPimpl->type = MapType::FloatRanged;
            myPimpl->bits = resolution.value;
            myPimpl->maxRange = 1 << resolution.value;
        }
    }
    else
    {
        myPimpl->type = MapType::Ignore;

        // RAM: TODO: LOG!
    }
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatRangeStrict specs)
    : DeltaMapItem(offsetToUse)
{
    if (specs.resolution.value >= 1)
    {
        auto range = specs.maxValue - specs.minValue;

        if (range > 0)
        {
            myPimpl->type = MapType::FloatRangedStrict;

            myPimpl->c = - specs.minValue;
            myPimpl->m = ((1 << specs.resolution.value) - 1) / range;
            myPimpl->bits = specs.resolution.value;
        }
        else
        {
            myPimpl->type = MapType::Ignore;

            // RAM: TODO: LOG!
        }
    }
    else
    {
        myPimpl->type = MapType::Ignore;

        // RAM: TODO: LOG!
    }
}

}}} // namespace
