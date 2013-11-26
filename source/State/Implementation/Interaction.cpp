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
#include <tuple>

namespace GameInABox { namespace State { namespace Implementation {

using namespace std;

// /////////////////////
// NOTE: Idealally the loops can be parallelised.
// /////////////////////
vector<Entity> Think(const vector<Entity>& oldWorld, GameCode theGame)
{
    array<vector<unsigned>, static_cast<int>(EntityType::MaxValue) + 1> filters;
    auto newWorld = vector<Entity>{};
    const auto size = oldWorld.size();

    newWorld.reserve(oldWorld.size());

    // Step 1 - Copy the old world to the new world and Think.
    //          Also get a list of creators and nones.

    auto creators = vector<unsigned>{};
    auto nothings = vector<unsigned>{};

    for (unsigned i = 0; i < size; ++i)
    {
        Entity newGuy = oldWorld[i];

        if (theGame.canThink(newGuy))
        {
            for (auto constantIndex : filters[static_cast<unsigned>(EntityType::Constants)])
            {
                //-gravity to all players
                //-move/jet/jump to all players
                //-jet to all missles
                //-jet energy (can player jet)
                //-health (is player alive)
                //-has game finished
                //-missle timer (can player fire missle)
                //-fire missles
                //-exploder missle
                //-transfer points from/to missle target
                //-kill player is player health < 0
                //-slow player speed if on ground (friction)
                //-slow player speed if in air (drag)
                //-cap speed if larger than max game speed
                //-player hitting ground on a fall
                //-changing type
                newGuy = theGame.think(newGuy, oldWorld[constantIndex].constants);
            }
        }

        if (theGame.canCreate(newGuy))
        {
            creators.emplace_back(i);
        }
        else
        {
            if (newGuy.type == EntityType::None)
            {
                nothings.emplace_back(i);
            }
        }

        newWorld.emplace_back(newGuy);
    }

    // oldWorld is no longer referenced from here on.

    // Step 2 - Convert None entities to something else (eg a missle).
    auto totalCreates = min(
            nothings.size(),
            creators.size());

    for (unsigned i = 0; i < totalCreates; ++i)
    {
        tie(newWorld[nothings[i]], newWorld[creators[i]]) = theGame.create(newWorld[creators[i]]);
    }

    // Step 3 - Detect Collisions
    auto collisions = vector<pair<unsigned, unsigned>>{};

    for (unsigned i = 0; i < size; ++i)
    {
        const auto& a = newWorld[i];

        if (theGame.canCollide(a))
        {
            for (unsigned j = i + 1; j < size; ++j)
            {
                const auto& b = newWorld[j];

                if (theGame.canCollide(b))
                {
                    if (theGame.collides(a, b))
                    {
                        collisions.emplace_back(i,j);
                    }
                }
            }
        }
    }

    // Step 4 - Process Collisions
    for (const auto& collision : collisions)
    {
        tie(newWorld[collision.first], newWorld[collision.second]) =
                theGame.resolve(newWorld[collision.first], newWorld[collision.second]);
    }

    // Step 5 - Apply Acceleration to Velocity, Velocity to Position
    for (auto& entity : newWorld)
    {
        if (theGame.canMove(entity))
        {
            entity = theGame.move(entity);
        }
    }

    return newWorld;
}

}}} // namespace

