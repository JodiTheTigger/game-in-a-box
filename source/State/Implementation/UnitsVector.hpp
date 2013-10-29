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

#ifndef UNITSVECTOR_HPP
#define UNITSVECTOR_HPP

#include "Vector3.hpp"

#include <boost/units/systems/si.hpp>
//#include <boost/units/systems/angle/degrees.hpp>

namespace GameInABox { namespace State { namespace Implementation {

using Velocity = boost::units::quantity<boost::units::si::velocity, Vector3>;
using Acceleration = boost::units::quantity<boost::units::si::acceleration, Vector3>;
using Position = boost::units::quantity<boost::units::absolute<boost::units::si::meter>, Vector3>;
using Orientation = boost::units::quantity<boost::units::absolute<boost::units::si::plane_angle>, Vector3>;

/* Scalar stuff
using Angle = boost::units::quantity<boost::units::si::plane_angle, float>;
using Length = boost::units::quantity<boost::units::si::length, float>;

// /////////////////////
// User defined literals for units.
// /////////////////////
Angle constexpr operator"" _degrees(long double value)
{
    return value * boost::units::degree::degrees;
}

Angle constexpr operator"" _radians(long double value)
{
    return value * boost::units::si::radians;
}
*/

}}} // namespace

#endif // UNITSVECTOR_HPP
