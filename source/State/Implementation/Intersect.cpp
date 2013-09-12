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

#include "Intersect.hpp"
#include "IntersectNone.hpp"
#include "IntersectTime.hpp"
#include "IntersectPlayer.hpp"
#include "IntersectPlayerAction.hpp"
#include "IntersectMissle.hpp"

#include <algorithm>
#include <array>

namespace GameInABox { namespace State { namespace Implementation {

using namespace std;

static const unsigned IntersectCount = static_cast<unsigned>(EntityType::MaxValue);

// I Hate cut 'n' paste code as much as anyone, but I hate macros more by the look of it.
array<Intersection, IntersectCount> GIntersects
{
    // RAM: TODO: Keep None around till I know I don't need it, then remove it.
    //IntersectFactory<EntityType::None>::GetIntersect(),
    IntersectFactory<EntityType::Time>::GetIntersect(),
    IntersectFactory<EntityType::Player>::GetIntersect(),
    IntersectFactory<EntityType::PlayerAction>::GetIntersect(),
    IntersectFactory<EntityType::Missle>::GetIntersect()
};

vector<Intersect> Intersections(const vector<Entity>& entities)
{
    // //////////////
    // keep in mind here that all loops aim to be done in parallel.
    // //////////////

    // filter all the lists to get primaries and targets.
    array<vector<const Entity*>, IntersectCount> primaries{{}};
    array<vector<const Entity*>, IntersectCount> targets{{}};

    for (unsigned i = 0; i < IntersectCount; ++i)
    {
        for (const auto& ent : entities)
        {
            if (GIntersects[i].primary == ent.type)
            {
                primaries[i].push_back(&ent);
            }

            for (const auto& target : GIntersects[i].targets)
            {
                if (target == ent.type)
                {
                    targets[i].push_back(&ent);
                }
            }
        }
    }

    // do the work!
    array<vector<Intersect>, IntersectCount> results{{}};

    for (unsigned i = 0; i < GIntersects.size(); ++i)
    {
        for (auto& primary : primaries[i])
        {
            results[i] = GIntersects[i].test(*primary, targets[i]);
        }
    }

    // collapse/join the results.
    vector<Intersect> result{};

    for (auto&& toJoin : results)
    {
        result.insert(end(result), begin(toJoin), end(toJoin));
    }

    return result;
}

}}} // namespace
