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

#ifndef DELTAMAPITEM_HPP
#define DELTAMAPITEM_HPP

#include "DeltaTypes.hpp"
#include "Units.hpp"

#include <memory>

namespace GameInABox { namespace State { namespace Implementation {

class DeltaMapItem
{
public:    

    // I tried hiding the internals, but it made using the class too hard.
    // If the user wants to bork it all up by being stupid, let them.
    // TODO: Hide the internals and keep it easy to use.
    enum class MapType
    {
        Ignore,
        FloatRanged,
        FloatRangedStrict,
        Unsigned,
        Signed,
    };

    Offset          offsetInfo{"", 0_bytes};
    MapType         type{MapType::Ignore};

    // RAM: TODO: Change this to the bits datatype after making
    // the bits datatype signed.
    signed          bits{0};

    unsigned        maxRange{0};

    float           m{1.0f};
    float           inversem{1.0f};
    float           c{0.0f};

    DeltaMapItem(Offset offsetToUse, MapFloatFull specs);
    DeltaMapItem(Offset offsetToUse, MapSigned specs);
    DeltaMapItem(Offset offsetToUse, MapUnsigned specs);
    DeltaMapItem(Offset offsetToUse, MapFloatRanged specs);
    DeltaMapItem(Offset offsetToUse, MapFloatRangeStrict specs);

    DeltaMapItem() = default;
    DeltaMapItem(const DeltaMapItem&) = default;
    DeltaMapItem(DeltaMapItem&&) = default;
    DeltaMapItem& operator=(const DeltaMapItem&) = default;
    DeltaMapItem& operator=(DeltaMapItem&&) = default;
    ~DeltaMapItem() = default;
};

}}} // namespace

#endif // DELTAMAPITEM_HPP
