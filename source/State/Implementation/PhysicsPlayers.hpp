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

#ifndef PHYSICSPLAYERS_HPP
#define PHYSICSPLAYERS_HPP

#include "Types.hpp"

#include <btBulletDynamicsCommon.h>

#include <memory>
#include <vector>

namespace GameInABox { namespace State { namespace Implementation {

struct PhysicsPlayers
{
    // RAM: TODO: should be using units!
    static constexpr btScalar Mass{50.0};

    PhysicsPlayers();

    btSphereShape                                       playersShape;
    std::vector<std::unique_ptr<btDefaultMotionState>>  playersMotion;
    std::vector<std::unique_ptr<btRigidBody>>           players;
};

btVector3 Up(FlagsPlayer team);

btDiscreteDynamicsWorld& Add(btDiscreteDynamicsWorld& lhs, PhysicsPlayers& rhs)
{
    for (auto& player : rhs.players)
    {
        lhs.addRigidBody(player.get());
    }

    return lhs;
}

}}} // namespace

#endif // PHYSICSPLAYERS_HPP
