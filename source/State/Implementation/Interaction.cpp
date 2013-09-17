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

#include "Interaction.hpp"

#include <array>

namespace GameInABox { namespace State { namespace Implementation {

using namespace std;

static const unsigned EntityTypeCount = static_cast<unsigned>(EntityType::MaxValue) + 1;

array<EntityType, EntityTypeCount> EntityTypes
{{
    EntityType::None,
    EntityType::Time,
    EntityType::Player,
    EntityType::PlayerAction,
    EntityType::Missle
}};

vector<Colliding> Collide(const vector<Entity>& theWorld)
{
    // Sort entites into arrays based on type.
    array<vector<const Entity*>, EntityTypeCount> primaries{{}};

    for (unsigned i = 0; i < primaries.size(); ++i)
    {
        for (const auto& ent : theWorld)
        {
            if (ent.type == EntityTypes[i])
            {
                primaries[i].push_back(&ent);
            }
        }
    }

    // enumerate over all the registered Interactions and, well, test for all the collisions.
    array<vector<Colliding>, EntityTypeCount> results{{}};

    /*
    for (unsigned i = 0; i < GIntersects.size(); ++i)
    {
        for (auto& primary : primaries[i])
        {
            results[i] = GIntersects[i].test(*primary, targets[i]);
        }
    }*/

    // collapse/join the results.
    vector<Colliding> result{};

    for (auto& toJoin : results)
    {
        result.insert(end(result), begin(toJoin), end(toJoin));
    }

    return result;
}

vector<Entity> React(const vector<Colliding>& /*collisions*/, const vector<Entity>& theWorld)
{
    // RAM: TODO: STUB!
    return theWorld;
}

}}} // namespace

