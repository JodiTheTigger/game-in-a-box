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

#ifndef INTERSECT_HPP
#define INTERSECT_HPP

#include "Entity.hpp"

#include <vector>
#include <functional>

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Do we need this if a+b are pointers?
struct IntersectType
{
    EntityType a;
    EntityType b;
};

struct Intersect
{
    // RAM: TODO: Is this needed? We can just do a custom function that sorts
    // via a->type and b->type.
    IntersectType type;

    // RAM: TODO: Pointers or indexes, need to make up my mind.
    const Entity* a;
    const Entity* b;
    //size_t indexA;
    //size_t indexB;
};

// Defines the worker function that determines intersections between entities
// Can by physics, state, user input, whatever.
struct Intersection
{
    using Tester = std::function<std::vector<Intersect>(const Entity&, const std::vector<const Entity*>&)>;

    // What we intersect with.
    EntityType primary;

    // Types of entities we care about.
    std::vector<EntityType> targets;

    // Actual intersect test.
    Tester test;
};

// Used to generate a compile table of all Intersections we use
template<EntityType ENT>
struct IntersectFactory;

// Does all the intersections between all entities. This might take a while.
std::vector<Intersect> Intersections(const std::vector<Entity>& entities);

}}} // namespace

#endif // INTERSECT_HPP
