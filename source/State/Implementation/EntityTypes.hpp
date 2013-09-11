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

#ifndef ENTITYTYPES_HPP
#define ENTITYTYPES_HPP

#include <cstdint>

// RAM: TODO: Move this to common.
#include <Network/WrappingCounter.hpp>

namespace GameInABox { namespace State { namespace Implementation {

struct Tick
{
    GameInABox::Network::WrappingCounter<std::uint_fast32_t, 32> value;
};

struct Id
{
    std::uint_fast32_t value;
};

struct Health
{
    std::uint_fast32_t value;
};

struct Energy
{
    std::uint_fast32_t value;
};

// RAM: TODO: Was I going to bother with boost::units?
// Meters
struct Position
{
        float x;
        float y;
        float z;
};

// RAM: TODO: Was I going to bother with boost::units?
// Rads
struct Orientation
{
        float x;
        float y;
        float z;
};

}}} // namespace

#endif // ENTITYTYPES_HPP
