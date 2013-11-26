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

#ifndef INTERACTION_HPP
#define INTERACTION_HPP

#include "Entity.hpp"

#include <vector>
#include <tuple>
#include <functional>

namespace GameInABox { namespace State { namespace Implementation {

struct GameCode
{
    using TestSingle      = std::function<bool(const Entity&)>;
    using TestPair        = std::function<bool(const Entity&, const Entity&)>;

    using ActionThink     = std::function<Entity(Entity, const EntityConstants&)>;
    using ActionCreate    = std::function<std::pair<Entity, Entity>(Entity)>;
    using ActionMap       = std::function<Entity(Entity)>;
    using ActionMapPair   = std::function<std::pair<Entity, Entity>(Entity, Entity)>;

    // Tests
    TestSingle  canThink;
    TestSingle  canCreate;
    TestSingle  canCollide;
    TestSingle  canMove;

    TestPair    collides;

    // Actions
    ActionThink     think;
    ActionCreate    create;
    ActionMapPair   resolve;
    ActionMap       move;
};

std::vector<Entity> Think(const std::vector<Entity>& oldWorld, GameCode theGame);

}}} // namespace

#endif // INTERACTION_HPP
