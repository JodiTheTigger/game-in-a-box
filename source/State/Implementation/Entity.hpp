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

#ifndef ENTITY_HPP
#define ENTITY_HPP

#include "EntityTypes.hpp"
#include "EntityTime.hpp"
#include "EntityPlayer.hpp"
#include "EntityMissle.hpp"
#include "EntityConstants.hpp"

#include <type_traits>

namespace GameInABox { namespace State { namespace Implementation {

enum class EntityType
{
    None,
    Time,
    Player,
    PlayerAction,
    Missle,
    Constants,

    MaxValue = Constants
};

struct EntityNone
{

};

struct Entity
{
    EntityType type;

    union
    {
        EntityNone nothing;
        EntityTime time;
        EntityPlayer player;
        EntityMissle missle;
        EntityConstants constants;
    };
};

// Error checking.
static_assert(std::is_pod<Entity>::value, "Entity is not a plain old data structure (POD).");
// Not supported by the GCC standard library yet.
//static_assert(std::is_trivially_copyable<Entity>::value, "Entity is not trivially copyable.");

}}} // namespace

#endif // ENTITY_HPP
