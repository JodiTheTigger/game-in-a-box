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

#include "EntityIntersect.hpp"

#include <algorithm>

namespace GameInABox { namespace State { namespace Implementation {

using namespace std;

vector<Intersection> Intersect(const vector<Entity>& entities)
{
    // RAM: TODO: Figure out a better way of turning a vector<A> to vector<A*>.
    vector<const Entity*> pointers;

    for(const auto& ent : entities)
    {
        pointers.push_back(&ent);
    }

    // sorted stuff goes faster right?
    sort(
        begin(pointers),
        end(pointers),
        [](const Entity* a, const Entity* b)
        {
            return (a->type) < (b->type);
        });

    // do all the intersections here please.
    //for (auto time : pointers_that_are_time)
    //{
    //    for (auto notTime : pointers_that_are_not_time)
    //    {
    //        result.push_back(IntersectTime(time, nottime));
    //    }
    //}

    return {};
}

}}} // namespace
