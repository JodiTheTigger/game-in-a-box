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
#ifndef ACTTIMETIME_HPP
#define ACTTIMETIME_HPP

#include "Interaction.hpp"
#include "Factory.hpp"

namespace GameInABox { namespace State { namespace Implementation {

bool CollideTimeTime(const Entity& protagonist, const Entity& antagonist);
Entity ReactTimeTime(Entity protagonist, const Entity& antagonist, const std::vector<const Entity*>& theWorld);

template<>
struct Factory<Interaction, EntityType::Time>
{
    static Interaction Get()
    {
        return
        {
            EntityType::Time,
            EntityType::Time,

            CollideTimeTime,
            ReactTimeTime
        };
    }
};

}}} // namespace

#endif // ACTTIMETIME_HPP
