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

#ifndef UNITSSI_HPP
#define UNITSSI_HPP

#include <boost/units/systems/si.hpp>
#include <boost/units/systems/angle/degrees.hpp>

// NOTE: Boost units don't have constexpr constructors, so the
// constexpr compiletime calculation optimsation is lost. However
// for now I'm willing to pay that price for unit correct calculations.
// StoppingBugs > PrematureOptimsation.

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Unit Structs
// /////////////////////
using Radians = boost::units::quantity<boost::units::si::plane_angle, float>;

// /////////////////////
// User defined literals for units.
// /////////////////////
Radians inline operator"" _radians(long double value)
{
    return {static_cast<float>(value) * boost::units::si::radians};
}
Radians inline operator"" _radian(long double value)
{
    return {static_cast<float>(value) * boost::units::si::radians};
}
/* RAM: TODO: Add if ever needed. Remove once gamestate is finished, and these are never referenced.
Radians inline operator"" _degrees(long double value)
{
    return {static_cast<Radians>(static_cast<float>(value) * boost::units::degree::degrees)};
}
Radians inline operator"" _degree(long double value)
{
    return {static_cast<Radians>(static_cast<float>(value) * boost::units::degree::degrees)};
}
*/
}}} // namespace

#endif // UNITSSI_HPP
