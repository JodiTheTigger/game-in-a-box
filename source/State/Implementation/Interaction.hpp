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

#include <boost/optional.hpp>

namespace GameInABox { namespace State { namespace Implementation {

struct Colliding
{
    unsigned indexProtagonist;
    unsigned indexAntagonist;
};

struct Interaction
{
    using Filter = std::function<bool(const Entity&)>;
    using Collision = std::function<bool(const Entity&, const Entity&)>;
    using Reaction = std::function<Entity(Entity, const Entity&, const std::vector<const Entity*>&)>;

    Filter filterProtagonist;
    Filter filterAntogonist;

    Collision collide;

    Reaction react;
    Reaction reactReverse;

    static inline Entity Copy(Entity copied, const Entity&, const std::vector<const Entity*>&) { return copied; }
};

std::vector<Colliding>  Collide(const std::vector<Entity>& theWorld);
std::vector<Entity>     React(const std::vector<Colliding>& collisions, const std::vector<Entity>& theWorld);

}}} // namespace

#endif // INTERACTION_HPP
