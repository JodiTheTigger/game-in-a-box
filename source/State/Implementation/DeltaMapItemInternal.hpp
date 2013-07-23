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

#ifndef DELTAMAPITEMINTERNAL_HPP
#define DELTAMAPITEMINTERNAL_HPP

#include "Units.hpp"
#include <string>

namespace GameInABox { namespace State { namespace Implementation {

// Used internally by the delta coder. Don't use directly.
// RAM: TODO: Investigate if we can make this scope private by putting into a cpp file.
// RAM: TODO: Maybe merge with the code the does the actual delta?
enum class MapType
{
    Ignore,
    FloatFull,
    FloatRanged,
    FloatRangedStrict,
    Unsigned,
    Signed,
};

struct DeltaMapItemInternal
{
    std::string name{""};
    Bytes offset{0};
    MapType type{MapType::Unsigned};

    signed bits{0};

    unsigned maxRange{0};

    float m{1.0f};
    float c{0.0f};
};

}}} // namespace

#endif // DELTAMAPITEMINTERNAL_HPP
