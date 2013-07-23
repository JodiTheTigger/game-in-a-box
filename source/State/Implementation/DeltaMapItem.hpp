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

#include "TypesDelta.hpp"

#include <memory>

namespace GameInABox { namespace State { namespace Implementation {
struct DeltaMapItemInternal;

class DeltaMapItem
{
public:
    // Can't have a destructor in the header file if using
    // unique_ptr + pimpl idiom.
    ~DeltaMapItem();

    DeltaMapItem(Offset offsetToUse, MapFloatFull specs);
    DeltaMapItem(Offset offsetToUse, MapSigned specs);
    DeltaMapItem(Offset offsetToUse, MapUnsigned specs);
    DeltaMapItem(Offset offsetToUse, MapFloatRanged specs);
    DeltaMapItem(Offset offsetToUse, MapFloatRangeStrict specs);

private:
    DeltaMapItem(Offset offsetToUse);

    std::unique_ptr<DeltaMapItemInternal> myPimpl;
};

}}} // namespace

#endif // DELTAMAPITEM_HPP
