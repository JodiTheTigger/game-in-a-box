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


      ------------------------

      What do I need to actually do?

      -gravity to all players
      -move/jet/jump to all players
      -jet to all missles
      -is player on ground
      -jet energy (can player jet)
      -health (is player alive)
      -collisions
      -has game finished
      -missle timer (can player fire missle)
      -fire missles
      -exploder missle
      -transfer points from/to missle target
      -kill player is player health < 0
      -slow player speed if on ground (friction)
      -slow player speed if in air (drag)
      -cap speed if larger than max game speed
      -player hitting ground on a fall



      for (player : players)                        player.acceleration += gravity;
      for (player : players(moving, on ground))     player.acceleration += look * playerMoveMs2;
      for (player : players(moving, in air))        player.acceleration += look * playerMoveAirMs2;
      for (player : players(jump, on ground))       player.acceleration += jumpImpulse;
      for (player : players(jet, can jet))          player.acceleration += jetDirection * jetImpulse;

      what I've been trying to do is to split all those tasks into something that can be done in parallel.
      problem is, is that I lose locality and get cache misses.

      finally, concerning collisions I need to update two entites at once in a collision function, so that's
      two copies. But what if I only want to reference one and modify the other, that's a waste of a copy
      (stupid functional programming).
      What if multiple collisions reference the same entity, how do i modify it mutliple times if multithreading
      the collsion handling? (how about reduce? vector or vectors is scattered in memory, need to keep all together)

      Ok, so collisons are pairs of indexes (index1, index2). But wait, we need a collision type if we want
      to generalise this away from purely physics collsions. ((index1, type), (index2, type)).
      sort to keep index1-index2 distance < cache line size or some equilivant metric?

      [](p1, p2)
      {
        d1 = abs(p1.i1 - p1.i2);
        d2 = abs(p2.i1 - p2.i2);

        // return the one with the biggest distance
        if (d1 != d2)
        {
            if ((d1 > MAXDISTANCE) || (d2 > MAXDISTANCE))
            {
              return d1 > d2;
            }
        }

        // otherwise sort by index1, index2
        if (p1.i1 != p2.i1)
        {
          return p1.i1 > p2.i2;
        }

        return p1.i2 > p2.i2;
      }

      - another question, what's faster? pre sorting lists, or just testing conditions and only doing one pass?
        as I assume memory reads are more expensive than comparisons, so minimising passes would be better.

      - how about loop though entire world (in parallel), and for each item apply singles
        (gravity, player input, state calculation). Then a second pass with the entire world which is all cache messy?

      - how about I just do one big loop, trying to keep pure so I can parallel later. Worry about caching when I have
        a working game loop?

      - Should have googled this first: http://www.gamasutra.com/view/feature/130247/multithreaded_game_engine_.php?print=1

      - Hmm, different types of dependencies: flow dependent, memory dependent.

      -----------

      - So, how do I do the game loop? I haven't written working code yet!

      - loop though all ents and do a think function. This is the same as colliding all ents with a time ent.
        (but how does time update itself? It's special, updated out of scope). Hey, time could be a constant too.

      - and firing a missle is the same as colliding a player with an empty when the player can and is firing
        and in order to not have multiple players using the same empty, it's a map function with all the ents.
        so I need to filter all empties and non-empties and do a map (maybe empty) = map(empty, non-empty).

      - so again, im at this filter, collide, act loop. But so I can get started generalise later, after working code!
        Iterate to perfection as opposed to perfecting first shot.


        for (loop(world))
        {
            newWorld[loop] = Collide(oldWorld[loop], constants);
        }

        // filters.
        for (loop(world))
        {
            if (oldWorld[loop].type == EntityNone)
            {
                nothings.push_back(loop)
            }
            else
            {
                somethings.push_back(loop)

                if (WantsToCreate(oldWorld[loop]))
                {
                    creators.push_back[loop];
                }
            }
        }

        // SYNC!

        // create things from nothing (missles, messages)
        // map
        for (loop(nothings, creators))
        {
            newWorld[nothings[loop]], newWorld[somethings[loop]] =
                Create(newWorld[nothings[loop]], newWorld[creators[loop]]);
        }

        // SYNC!

        for (loop(collisions))
        {
            // need to think about this, as can have multiple collisions with multiple things
            // so that each collision resolution becomes dependent on the previous one (yuck).
        }
    */
}

}}} // namespace

