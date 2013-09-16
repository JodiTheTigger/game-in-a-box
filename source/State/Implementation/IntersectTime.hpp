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

#ifndef INTERSECTTIME_HPP
#define INTERSECTTIME_HPP

#include "Intersect.hpp"
#include "Factory.hpp"

namespace GameInABox { namespace State { namespace Implementation {

std::vector<Intersect> IntersectTime(const Entity& primary, const std::vector<const Entity*>& targets);

// Intersection parameters
template<>
struct Factory<Intersection, EntityType::Time>
{
    static Intersection Get()
    {
        return
        {
            // RAM: TODO: Copy and pasting from template type, don't do this!
            EntityType::Time,

            // RAM: TODO: Flesh this out
            std::vector<EntityType>{EntityType::Missle},
            IntersectTime
        };
    }
};

}}} // namespace

#endif // INTERSECTTIME_HPP