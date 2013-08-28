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

#include "Game.hpp"

#include "Types.hpp"

#include "PhysicsCube.hpp"
#include "PhysicsPlayers.hpp"

#include "Network/DeltaTypes.hpp"
#include "Network/DeltaMapItem.hpp"
#include "Network/Units.hpp"

#include <Common/MakeUnique.hpp>
#include <vector>

using namespace GameInABox::Network;
using namespace std;

namespace GameInABox { namespace State { namespace Implementation {

Game::Game()
    : myPhysicsBroadPhase()
    , myCollisionConfiguration()
    , myPhysicsDispatcher(&myCollisionConfiguration)
    , myPhyiscsSolver()
    , myPhysicsWorld(&myPhysicsDispatcher, &myPhysicsBroadPhase, &myPhyiscsSolver, &myCollisionConfiguration)
    , myPhysicsLevel(make_unique<PhysicsCube>())
    , myPhysicsPlayers(make_unique<PhysicsPlayers>())
{
    auto mapPlayer = std::vector<DeltaMapItem>
    {
        // RAM: TODO: This is all inavlid! Fix!
        {MAKE_OFFSET(StatePlayer, position.x), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
        {MAKE_OFFSET(StatePlayer, position.y), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
        {MAKE_OFFSET(StatePlayer, position.z), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},

        {MAKE_OFFSET(StatePlayer, lookAndDo.orientation.x), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
        {MAKE_OFFSET(StatePlayer, lookAndDo.orientation.y), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
        {MAKE_OFFSET(StatePlayer, lookAndDo.orientation.z), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},

        {MAKE_OFFSET(StatePlayer, health), MapUnsigned{10_bits}},
        {MAKE_OFFSET(StatePlayer, energy), MapUnsigned{10_bits}},

        // RAM: TODO: Automate this using enum::maxvalue and constexpr calculating max bits.
        {MAKE_OFFSET(StatePlayer, lookAndDo.flags), MapUnsigned{7_bits}},
    };

    auto mapMissle = std::vector<DeltaMapItem>
    {
        // RAM: TODO: This is all inavlid! Fix!
        // RAM: TODO: Find max bits.
        {MAKE_OFFSET(StateMissle, owner), MapUnsigned{32_bits}},

        {MAKE_OFFSET(StateMissle, source.x), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
        {MAKE_OFFSET(StateMissle, source.y), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},
        {MAKE_OFFSET(StateMissle, source.z), MapFloatRangeStrict{500.0f, -500.0f, 17_bits}},

        {MAKE_OFFSET(StateMissle, orientation.x), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
        {MAKE_OFFSET(StateMissle, orientation.y), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},
        {MAKE_OFFSET(StateMissle, orientation.z), MapFloatRangeStrict{1.0f, -1.0f, 12_bits}},

        // RAM: TODO: Find max bits, wrapping counter?
        {MAKE_OFFSET(StateMissle, lastAction), MapUnsigned{32_bits}},

        {MAKE_OFFSET(StateMissle, flags), MapUnsigned{2_bits}},
    };


    // /////////////////////
    // Physics setup.
    // /////////////////////
    // Gravity is team dependant.
    myPhysicsWorld.setGravity(btVector3(0,0,0));

    // For our first game, everyone is a sphere 500mm in diameter.
    // we have a network resolution of 20mm

    auto missle = btSphereShape{0.1};

    Add(myPhysicsWorld, *myPhysicsLevel);
    Add(myPhysicsWorld, *myPhysicsPlayers);

    // well then, we're good to go I assume.
}

Game::~Game()
{
    // require as smart pointers are used, and they don't like default constructed destructors.
}

void Game::Tick()
{
    // Update all player's movement and gravity

    // physics!
    // RAM: TODO: Should I make this alterable?
    // The game sends network ticks every 50ms (20hz).
    // how fast should I simulate? should it be a multiple of 50ms?
    // 50 ticks/second = 20ms time step. If this can take a while, I might split it up into
    // 5ms. But the default is 16.6ms. Hmm, lets stick to 20ms and see if we go unstable.
    myPhysicsWorld.stepSimulation(0.02, 1, 0.02);

    // Update game state transitions
}

}}} // namspace
