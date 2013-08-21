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

#ifndef GAME_HPP
#define GAME_HPP

#include <btBulletDynamicsCommon.h>
#include <array>

namespace GameInABox { namespace State { namespace Implementation {

class Game
{
public:
    Game();

    // All game stuff happens here.
    // RAM: TODO: Need to pass max allowed time? How do we control the time
    // allowed?
    void Tick();

private:
    btDbvtBroadphase myPhysicsBroadPhase;
    btDefaultCollisionConfiguration myCollisionConfiguration;
    btCollisionDispatcher myPhysicsDispatcher;
    btSequentialImpulseConstraintSolver myPhyiscsSolver;
    btDiscreteDynamicsWorld myPhysicsWorld;

    /* RAM: Can't do, abstract base class.
    std::array<btCollisionShape, 6> myCollisionCube;
    btCollisionShape myCollisionPlayer;
    btCollisionShape myCollisionMissle;*/
};

}}} // namespace

#endif // GAME_HPP
