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
#include "DeltaTypes.hpp"
#include "DeltaMapItem.hpp"
#include "Units.hpp"

#include <vector>

using namespace GameInABox::Network;

namespace GameInABox { namespace State { namespace Implementation {

Game::Game()
    : myPhysicsBroadPhase()
    , myCollisionConfiguration()
    , myPhysicsDispatcher(&myCollisionConfiguration)
    , myPhyiscsSolver()
    , myPhysicsWorld(&myPhysicsDispatcher, &myPhysicsBroadPhase, &myPhyiscsSolver, &myCollisionConfiguration)/*
    , myCollisionCube(
    {
        btStaticPlaneShape(btVector3(-1,0,0),500),
        btStaticPlaneShape(btVector3(1,0,0),500),
        btStaticPlaneShape(btVector3(0,1,0),500),
        btStaticPlaneShape(btVector3(0,-1,0),500),
        btStaticPlaneShape(btVector3(0,0,1),500),
        btStaticPlaneShape(btVector3(0,0,-1),500)
    })
    , myCollisionPlayer(btSphereShape(0.5))
    , myCollisionMissle(btSphereShape(0.1))*/
{
    auto mapPlayer = std::vector<DeltaMapItem>
    {
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
    // the game is in a 1kmx1kmx1km box (+/- 1km = 1000m = 100,000cm = 50,000 network units = 17 bits).
}

}}} // namspace
