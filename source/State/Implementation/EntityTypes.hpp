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

#include "UnitsSI.hpp"
#include "Vector3.hpp"

// RAM: TODO: Move this to common.
#include <Network/WrappingCounter.hpp>

#include <cstdint>

namespace GameInABox { namespace State { namespace Implementation {

using Tick = Units::Quantity<Units::Second, GameInABox::Network::WrappingCounter<std::uint_fast32_t, 32>>;

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

struct Ammo
{
    std::uint_fast32_t value;
};

using Position      = Units::Quantity<Units::Meters, Vector3>;
using Orientation   = Units::Quantity<Units::Radians, Vector3>;
using Velocity      = Units::Quantity<Units::MetersPerSecond, Vector3>;
using Acceleration  = Units::Quantity<Units::MetersPerSecondSquared, Vector3>;
using Speed         = Units::Quantity<Units::MetersPerSecond, float>;
using Angle         = Units::Quantity<Units::Radians, float>;

struct Yaw
{
    Angle value;
};

}}} // namespace

#endif // ENTITYTYPES_HPP
