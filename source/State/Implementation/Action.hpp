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

#ifndef ACTION_HPP
#define ACTION_HPP

#include "Entity.hpp"
#include "Intersect.hpp"

#include <vector>

namespace GameInABox { namespace State { namespace Implementation {

struct Actors
{
    Entity protagonist;
    Entity antagonist;
};

// Defines the worker function that determines intersections between entities
// Can by physics, state, user input, whatever.
struct ActionSet
{
    using Action = std::function<
        Actors(
            const Entity&,
            const std::vector<const Entity*>&,
            const std::vector<Entity>&)>;

    EntityType protagonist;
    EntityType antagonist;

    // Actual work.
    Action work;
};

std::vector<Entity> Act(
        const std::vector<Entity>& entities,
        const std::vector<Intersect>& intersections);

}}} // namespace

#endif // ACTION_HPP
