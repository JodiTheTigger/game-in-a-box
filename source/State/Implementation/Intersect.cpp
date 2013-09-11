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

// yea, EntityType::None is a value, not a type, can't template drive that.
/*
// I Hate cut 'n' paste code as much as anyone, but I hate macros more by the look of it.
array<Intersection, static_cast<unsigned>(EntityType::MaxValue)> intersects
{

    IntersectFactory<EntityType::None>::GetIntersect(),
    IntersectFactory<EntityType::Time>::GetIntersect(),
    IntersectFactory<EntityType::Player>::GetIntersect(),
    IntersectFactory<EntityType::PlayerAction>::GetIntersect(),
    IntersectFactory<EntityType::Missle>::GetIntersect()

};
*/

vector<Intersect> Intersections(const vector<Entity>& entities)
{
    vector<Intersect> result{};

    // Get lists of everything please.
    // RAM: TODO: Is there a way to automate this so it isn't
    // cut and paste code?
    // or make arrays as we add the entites?
    vector<const Entity*> times{};
    vector<const Entity*> players{};
    vector<const Entity*> playerActions{};
    vector<const Entity*> missles{};

    for(const auto& ent : entities)
    {
        switch (ent.type)
        {
            case EntityType::Time: { times.push_back(&ent); break; }
            case EntityType::Player: { players.push_back(&ent); break; }
            case EntityType::PlayerAction: { playerActions.push_back(&ent); break; }
            case EntityType::Missle: { missles.push_back(&ent); break; }
            case EntityType::None: { break; }
            default: { break; }
        }
    }

    // RAM: TODO: Speed up by having intersection exclusions?
    // RAM: TODO: Copy/paste code! fix it somehow!
    // RAM: TODO: Invert the order? for each ent do each collision type?
    // RAM: TODO: What if there isn't an intersection? What happens then?
    for (auto a: times)
    {
        for (const auto& ent : entities)
        {
            result.push_back(IntersectTime(*a, ent));
        }
    }
    for (auto a: players)
    {
        for (const auto& ent : entities)
        {
            result.push_back(IntersectPlayer(*a, ent));
        }
    }
    for (auto a: playerActions)
    {
        for (const auto& ent : entities)
        {
            result.push_back(IntersectPlayerAction(*a, ent));
        }
    }
    for (auto a: missles)
    {
        for (const auto& ent : entities)
        {
            result.push_back(IntersectMissle(*a, ent));
        }
    }

    return result;
}

}}} // namespace
