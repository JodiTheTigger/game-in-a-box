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

struct Ammo
{
    std::uint_fast32_t value;
};

using Vector                = Units::Quantity<Units::Unitless, Vector3>;
using Position              = Units::Quantity<Units::Meters, Vector3>;
using Orientation           = Units::Quantity<Units::Radians, Vector3>;
using Velocity              = Units::Quantity<Units::MetersPerSecond, Vector3>;
using AccelerationVector    = Units::Quantity<Units::MetersPerSecondSquared, Vector3>;

using AccelerationScalar    = Units::Quantity<Units::MetersPerSecondSquared, float>;
using Speed                 = Units::Quantity<Units::MetersPerSecond, float>;
using Angle                 = Units::Quantity<Units::Radians, float>;
using Period                = Units::Quantity<Units::Seconds, float>;
using Energy                = Units::Quantity<Units::Joules, float>;
using Mass                  = Units::Quantity<Units::Kilograms, float>;
using Scalar                = Units::Quantity<Units::Unitless, float>;

struct Yaw
{
    Angle value;
};

// ///////////////////
// Helpers
// ///////////////////
template<typename UNIT>
Vector Normalise(const Units::Quantity<UNIT, Vector3>& lhs)
{
    return Vector{Normalise(lhs.value)};
}

Speed GetSpeed(const Velocity& lhs)
{
    return Speed{LengthF(lhs.value)};
}

// /////////////////////
// User defined literals for units.
// /////////////////////
Period constexpr operator"" _s(long double value)
{
    return {static_cast<float>(value)};
}

Scalar constexpr operator"" _scalar(long double value)
{
    return {static_cast<float>(value)};
}

Mass constexpr operator"" _kg(long double value)
{
    return {static_cast<float>(value)};
}

Energy constexpr operator"" _joules(long double value)
{
    return {static_cast<float>(value)};
}

Speed constexpr operator"" _m_s(long double value)
{
    return {static_cast<float>(value)};
}

AccelerationScalar constexpr operator"" _m_s2(long double value)
{
    return {static_cast<float>(value)};
}

}}} // namespace

#endif // ENTITYTYPES_HPP
