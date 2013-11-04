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

#ifndef FILTERS_HPP
#define FILTERS_HPP

#include "Entity.hpp"
#include "Flags.hpp"

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Move these to some sort of game global settings header.
static constexpr Ammo AmmoUsedPerShot{2500};

// Quick filters
inline constexpr bool FilterNone(const Entity& toTest) { return toTest.type == EntityType::None; }
inline constexpr bool FilterTime(const Entity& toTest) { return toTest.type == EntityType::Time; }
inline constexpr bool FilterPlayer(const Entity& toTest) { return toTest.type == EntityType::Player; }

inline constexpr bool FilterNotTimeNotNone(const Entity& toTest)
{
    return (toTest.type != EntityType::Time) && toTest.type != (EntityType::None);
}

inline bool constexpr FilterFiring(const Entity& toTest)
{
//    RAM: TODO: Fix this
//    return  FlagIsSet(toTest.player.input.action, FlagsPlayerAction::Fire) &&
//            FlagIsSet(toTest.player.allowedAction, FlagsPlayerAction::Fire) &&
//            (toTest.player.energyShoot >= AmmoUsedPerShot);
        return  FlagIsSet(toTest.player.input.action, FlagsPlayerAction::Fire) &&
                FlagIsSet(toTest.player.allowedAction, FlagsPlayerAction::Fire);
}

}}} // namespace

#endif // FILTERS_HPP
