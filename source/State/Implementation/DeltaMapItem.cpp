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

#include <Common/Logging.hpp>

namespace GameInABox { namespace State { namespace Implementation {

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatFull)
    : offsetInfo(offsetToUse)
    , type(MapType::Unsigned)
    , resolution(32_bits)
{
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapSigned specs)
    : offsetInfo(offsetToUse)
    , type(MapType::Signed)
    , resolution(specs.resolution)
{
    if (resolution < 1_bit)
    {
        type = MapType::Ignore;

        Common::Log(
            Common::LogLevel::Debug,
            "DeltaMapItem ignored: ",
            offsetInfo.name.c_str(),
            ": resolution < 1 bit.");
    }
    else
    {
        if (resolution > 32_bits)
        {
            resolution = 32_bits;
        }
    }
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapUnsigned specs)
    : offsetInfo(offsetToUse)
    , type(MapType::Unsigned)
    , resolution(specs.resolution)
{
    if (resolution < 1_bit)
    {
        type = MapType::Ignore;

        Common::Log(
            Common::LogLevel::Debug,
            "DeltaMapItem ignored: ",
            offsetInfo.name.c_str(),
            ": resolution < 1 bit.");
    }
    else
    {
        if (resolution > 32_bits)
        {
            resolution = 32_bits;
        }
    }
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatRanged specs)
    : offsetInfo(offsetToUse)
{
    if (specs.maxValue > 0)
    {
        // figure out the range in bits.
        Bits maxResolution;

        while ((1U << maxResolution.value) < (2U*specs.maxValue))
        {
            ++(maxResolution.value);
        }

        // float has 23 bits of mantessa, no point doing this is we require a larger dynamic range.
        if (maxResolution > 22_bits)
        {
            // full float.
            type = MapType::Unsigned;
            resolution = 32_bits;

            Common::Log(
                Common::LogLevel::Debug,
                "DeltaMapItem: ",
                offsetInfo.name.c_str(),
                ": Float (range) resolution > 22 bits, using full float instead.");
        }
        else
        {
            type = MapType::FloatRanged;
            resolution = maxResolution;
            maxRange = 1 << maxResolution.value;
        }
    }
    else
    {
        type = MapType::Ignore;

        Common::Log(
            Common::LogLevel::Debug,
            "DeltaMapItem: ",
            offsetInfo.name.c_str(),
            ": Ignored: maxValue < 0.");
    }
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatRangeStrict specs)
    : offsetInfo(offsetToUse)
{
    if (specs.resolution >= 1_bit)
    {
        auto range = specs.maxValue - specs.minValue;

        if (range > 0)
        {
            type = MapType::FloatRangedStrict;

            if (specs.resolution > 32_bit)
            {
                specs.resolution = 32_bit;
            }

            c = - specs.minValue;
            m = ((1 << specs.resolution.value) - 1) / range;
            inversem = static_cast<float>(1.0 / m);
            resolution = specs.resolution;
        }
        else
        {
            type = MapType::Ignore;

            Common::Log(
                Common::LogLevel::Debug,
                "DeltaMapItem: ",
                offsetInfo.name.c_str(),
                ": Ignored: Float (range strict) range < 0.");
        }
    }
    else
    {
        type = MapType::Ignore;

        Common::Log(
            Common::LogLevel::Debug,
            "DeltaMapItem: ",
            offsetInfo.name.c_str(),
            ": Ignored: resolution < 1 bit.");
    }
}

}}} // namespace
