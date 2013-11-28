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

#ifndef ENTITYPLAYER_HPP
#define ENTITYPLAYER_HPP

#include "Types.hpp"
#include "EntityTypes.hpp"

#include <cstdint>

namespace GameInABox { namespace State { namespace Implementation {

enum class FlagsPlayerAction : uint32_t
{
    None    = 0,
    Foward  = (1 << 0),
    Back    = (1 << 1),
    Left    = (1 << 2),
    Right   = (1 << 3),
    Jet     = (1 << 4),
    Fire    = (1 << 5),

    // Server side only.
    Ground  = (1 << 6)
};

struct PlayerAction
{
    FlagsPlayerAction action;

    // Rift support. Look != Face != WeaponFace
    Orientation look;

    // Rotation around the YAxis
    Yaw bodyFacing;

    Orientation weaponPointing;
    Position weaponOffset;
};

struct EntityPlayer
{
    // RAM: TODO: Needed?
    Id id;

    // time is just another data point.
    Tick time;


    Velocity velocity;
    Velocity delta;

    Ammo energyShoot;

    // jetting
    Orientation jetting;
    Energy fuel;

    // Server side
    Tick lastFired;
    FlagsPlayerAction allowedAction;

    // USED IN CODE SO FAR
    // -------------------
    Position position;
    AccelerationVector acceleration;

    // Copied from the network system.
    PlayerAction input;

    Length collisionRadius;
};

}}} // namespace

#endif // ENTITYPLAYER_HPP
