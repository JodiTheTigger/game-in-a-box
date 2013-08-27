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

#include "PhysicsPlayers.hpp"

#include "Types.hpp"

#include <Common/MakeUnique.hpp>

namespace GameInABox { namespace State { namespace Implementation {

btVector3 Position(FlagsPlayer team, unsigned number)
{
    // just put people in an evenly spaced grid (max 1024 players per team).
    static_assert(StateGameSnapshot::maxPlayersPerTeam < 1025, "I assumed no more than 1024 players per team. Bad assumption. Bother.");

    auto x = float{static_cast<float>(number % 32)};
    auto y = float{static_cast<float>(number & 31)};

    switch (team)
    {
        case FlagsPlayer::TeamR:
        {
            return btVector3{x, y, 400};
        }

        case FlagsPlayer::TeamG:
        {
            return btVector3{400, x, y};
        }

        default:
        case FlagsPlayer::TeamB:
        {
            return btVector3{y, 400, x};
        }
    }
}

PhysicsPlayers::PhysicsPlayers()
    : playersShape{0.5}
    , playersMotion{}
    , players{}
{
    // state my assumptions.
    static_assert(StateGameSnapshot::maxTeams == 3, "I assumed 3 teams, this seems to be incorrect. Bother.");

    // no inertia when starting.
    auto inertia = btVector3{0,0,0};
    playersShape.calculateLocalInertia(Mass, inertia);

    // motion states
    for (unsigned player = 0; player < StateGameSnapshot::maxPlayersPerTeam; ++player)
    {
        playersMotion.insert(
                    end(playersMotion),
                    make_unique<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), Position(FlagsPlayer::TeamR, player))));
    }

    for (unsigned player = 0; player < StateGameSnapshot::maxPlayersPerTeam; ++player)
    {
        playersMotion.insert(
                    end(playersMotion),
                    make_unique<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), Position(FlagsPlayer::TeamG, player))));
    }

    for (unsigned player = 0; player < StateGameSnapshot::maxPlayersPerTeam; ++player)
    {
        playersMotion.insert(
                    end(playersMotion),
                    make_unique<btDefaultMotionState>(btTransform(btQuaternion(0,0,0,1), Position(FlagsPlayer::TeamB, player))));
    }

    // ridged bodies.
    for (auto& motion : playersMotion)
    {
        players.insert(
                    end(players),
                    make_unique<btRigidBody>(btRigidBody::btRigidBodyConstructionInfo
                    {
                        Mass,
                        motion.get(),
                        static_cast<btCollisionShape*>(&playersShape)
                    }));
    }
}

}}} // namespace
