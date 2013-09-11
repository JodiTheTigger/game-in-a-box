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

#ifndef INTERSECTNONE_HPP
#define INTERSECTNONE_HPP

#include "Intersect.hpp"

namespace GameInABox { namespace State { namespace Implementation {

Intersect IntersectNone(const Entity& a, const Entity& b);

// RAM: TODO: MAke this work.
// http://stackoverflow.com/questions/10694943/c-template-specialization-based-on-compile-time-value
template<EntityType ENT>
struct IntersectFactory;

template<>
struct IntersectFactory<EntityType::None>
{
/* RAM: TODO: Why doesn't this work?
    static Intersect GetIntersect()
    {
        return
        {
            EntityType::None,
            std::vector<EntityType>{},
            nullptr
        };
    }*/

    static std::vector<EntityType> Get()
    {
        return {};
    }

    static EntityType Get2()
    {
        return EntityType::None;
    }
};

/*
template <>
class IntersectFactory<EntityType::None>
{
public:
    static Intersect GetIntersect()
    {
        return
        {
            EntityType::None,
            {},
            IntersectNone
        };
    }
};
*/
}}} // namespace

#endif // INTERSECTNONE_HPP
