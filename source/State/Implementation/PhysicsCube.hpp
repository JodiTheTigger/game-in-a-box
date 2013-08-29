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

#ifndef PHYSICSCUBE_HPP
#define PHYSICSCUBE_HPP

#include <btBulletDynamicsCommon.h>

namespace GameInABox { namespace State { namespace Implementation {

// the game is in a 1kmx1kmx1km box (+/- 1km = 1000m = 100,000cm = 50,000 network units = 17 bits).
struct PhysicsCube
{
    std::array<btStaticPlaneShape, 6> sides
    {{
        btStaticPlaneShape{btVector3(1,0,0),0},
        btStaticPlaneShape{btVector3(-1,0,0),0},
        btStaticPlaneShape{btVector3(0,1,0),0},
        btStaticPlaneShape{btVector3(0,-1,0),0},
        btStaticPlaneShape{btVector3(0,0,1),0},
        btStaticPlaneShape{btVector3(0,0,-1),0}
    }};

    std::array<btDefaultMotionState, 6> sideMotion
    {{
        btDefaultMotionState{btTransform(btQuaternion(0,0,0,1),btVector3(-500,0,0))},
        btDefaultMotionState{btTransform(btQuaternion(0,0,0,1),btVector3(500,0,0))},
        btDefaultMotionState{btTransform(btQuaternion(0,0,0,1),btVector3(0,-500,0))},
        btDefaultMotionState{btTransform(btQuaternion(0,0,0,1),btVector3(0,500,0))},
        btDefaultMotionState{btTransform(btQuaternion(0,0,0,1),btVector3(0,0,-500))},
        btDefaultMotionState{btTransform(btQuaternion(0,0,0,1),btVector3(0,0,500))}
    }};

    std::array<btRigidBody, 6> walls
    {{
            btRigidBody{btRigidBody::btRigidBodyConstructionInfo{0, &sideMotion[0], &sides[0], btVector3(0,0,0)}},
            btRigidBody{btRigidBody::btRigidBodyConstructionInfo{0, &sideMotion[1], &sides[1], btVector3(0,0,0)}},
            btRigidBody{btRigidBody::btRigidBodyConstructionInfo{0, &sideMotion[2], &sides[2], btVector3(0,0,0)}},
            btRigidBody{btRigidBody::btRigidBodyConstructionInfo{0, &sideMotion[3], &sides[3], btVector3(0,0,0)}},
            btRigidBody{btRigidBody::btRigidBodyConstructionInfo{0, &sideMotion[4], &sides[4], btVector3(0,0,0)}},
            btRigidBody{btRigidBody::btRigidBodyConstructionInfo{0, &sideMotion[5], &sides[5], btVector3(0,0,0)}},
    }};
};

inline btDiscreteDynamicsWorld& Add(btDiscreteDynamicsWorld& lhs, PhysicsCube& rhs)
{
    for (auto& wall : rhs.walls)
    {
        lhs.addRigidBody(&wall);
    }

    return lhs;
}

inline btDiscreteDynamicsWorld& Remove(btDiscreteDynamicsWorld& lhs, PhysicsCube& rhs)
{
    for (auto& wall : rhs.walls)
    {
        lhs.removeRigidBody(&wall);
    }

    return lhs;
}

}}} // namespace

#endif // PHYSICSCUBE_HPP
