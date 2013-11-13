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
#include <algorithm>

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

/* RAM: this is what it used to look like.
// I Hate cut 'n' paste code as much as anyone, but I hate macros more by the look of it.
array<Intersection, IntersectCount> GIntersects
{{
    // RAM: TODO: Keep None around till I know I don't need it, then remove it.
    //IntersectFactory<EntityType::None>::Get(),
    Factory<Intersection, EntityType::Time>::Get(),
    Factory<Intersection, EntityType::Player>::Get(),
    Factory<Intersection, EntityType::PlayerAction>::Get(),
    Factory<Intersection, EntityType::Missle>::Get()
}};
*/

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

// The actual gamestate programme for parallel processing:
struct SequenceProcess
{
    using Reduce = std::function<Entity(Entity, Entity)>;

    // all these can be done in parallel
    vector<Interaction> processes;
    Reduce shrinker;
};

struct SequenceProcessPreProcessed
{
    // All the filters, but only one of them
    vector<Interaction::Filter> uniqueFilters;
};

vector<Entity> Process(const vector<Entity>& theWorld, vector<SequenceProcess> program)
{
    auto result = theWorld;

    for (auto task : program)
    {
        // Get filter list
        auto filterList = vector<Interaction::Filter>{};

        for(const auto& filter : task.processes)
        {
            filterList.push_back(filter.filterProtagonist);
            filterList.push_back(filter.filterAntogonist);
        }

        // Filter
        // Gonna have to investigate doing all this stuff using pointers so I don't
        // copy everything every which way (even with c++11 && semantics).
        /*
        auto filtered = vector<vector<Entity>>{};
        for (auto f : filterList)
        {
            remove_copy_if
            filtered.push_back(remove_if(begin(theWorld), end(theWorld), f));
        }*/

        // Collide

        // React
    }

    return result;

    // More notes:
    /*

    - std::transform can take two arguments! does transform in lockstep though.
    - use std::bind as well if you want
    1) convert list of items to list of pointers (std::transform)
    2) filter list of pointers to get filter lists (does filter do it for all, or first find only?)
    3) for each item, matches a primary filter? If so, for each item matches the paired secondary filter? if so do collision?
      - means I need a list of filters and their secondaries
      - means I'm searching lists all the time, not good.

      main question is, what do we split the CPUs by? to all maths on a subgroup of entities? cache is king, so we
      want to prevent iterating of the world multiple times? But collision takes two ents, and even though the
      first ent is iterated only once, the second is done multiple times. Maybe we just get working code working
      then test for speed afterwards as premature optimsation is evil?

      foreach (filter in uniquefilterlist)
      {
        filteredArray[count++] = filter(theWorld);
      }

      foreach (process in SequenceProcess)
      {
        foreach (first in filteredArray[process.filterFirst])
        {
          foreach (second in filteredArray[process.filterSecond])
          {
            if (process.test(first, second))
            {
              collisions.push_back(pair(first, second));
            }
          }
        }
      }

      // order == pair(a, b) ? (a>b) : pair(a,b), pair(b,a)
      order(collisions);
      sort(collisions);


      foreach (c in collisions)
      {
        newPair = collide(c);
        newWorld[indexOf(newPair.a)] += newPair.a;
        newWorld[indexOf(newPair.b)] += newPair.b;
      }

      -Maybe make this list a bit smarter, only items at index x can interact with items at index x+1 -> end?
    */
}

}}} // namespace

