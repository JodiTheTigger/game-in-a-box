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

#include "PlayerFunctions.hpp"
#include "Flags.hpp"

namespace GameInABox { namespace State { namespace Implementation {

Orientation Intent(const EntityPlayer& player)
{
    auto intent = Orientation{0.0f};
    auto left = Orientation
    {
            -player.input.look.value.Z(),
            player.input.look.value.Y(),
            player.input.look.value.X()
    };

    if (FlagIsSet(player.input.action, FlagsPlayerAction::Foward))
    {
        intent += player.input.look;
    }
    if (FlagIsSet(player.input.action, FlagsPlayerAction::Back))
    {
        intent -= player.input.look;
    }
    if (FlagIsSet(player.input.action, FlagsPlayerAction::Left))
    {
        intent += left;
    }
    if (FlagIsSet(player.input.action, FlagsPlayerAction::Right))
    {
        intent -= left;
    }

    return intent;
}

AccelerationVector PlayerMovement(const EntityPlayer& player, const EntityConstants& constants)
{
    AccelerationVector result = constants.gravity;

    bool onGround = FlagIsSet(player.allowedAction, FlagsPlayerAction::Ground);
    bool jetAllowed = FlagIsSet(player.allowedAction, FlagsPlayerAction::Jet);
    bool jetWanted = FlagIsSet(player.allowedAction, FlagsPlayerAction::Jet);

    auto intent = Intent(player);

    auto xz = IsZero(intent.value) ?
                Normalise(Vector{intent.value.X(), 0.0f,  intent.value.Z()}) :
                Vector{0.0f};

    Scalar control = onGround ?
                    constants.airControl :
                    Scalar{1.0f};

    // jump or jet?
    if (jetWanted)
    {
        if (onGround)
        {
            // jumping
            result += Vector{0.0,1.0} * constants.impulseJump;
        }
        else
        {
            if (jetAllowed)
            {
                // jetting
                auto xyz = Normalise(Vector{xz.value.X(), 1.0f,  xz.value.Z()});

                result += xyz * constants.impulseJet;
            }
        }
    }

    result += xz * constants.impulseMove * control;

    return result;
}



AccelerationVector ResolveCollision(const EntityPlayer&, const EntityPlayer&, const Position&)
{
    // RAM: TODO!
    // RAM: Hard!
    return {0.0f};
}


std::pair<Velocity, Velocity> ResolveCollisionAtCollisionPointSphere(const Velocity& v1, const Mass& m1, const Velocity& v2, const Mass& m2)
{
    return
    {
        ((v1 * (m1 - m2)) + (v2 * Scalar{2} * m2)) / (m1 + m2),
        ((v2 * (m2 - m1)) + (v1 * Scalar{2} * m1)) / (m1 + m2)
    };
}

/* RAM: Won't build till I've done the time rewind and result return parts.

// std::pair<std::tuple<Position, Velocity>, std::tuple<Position, Velocity>>
void ResolveCollisionSphere(
    const Position& p1,
    const Velocity& v1,
    const Mass& m1,
    const Position& p2,
    const Velocity& v2,
    const Mass& m2)
{
    // Mix of the following articles:
    // http://nicoschertler.wordpress.com/2013/10/07/elastic-collision-of-circles-and-spheres/
    // http://stackoverflow.com/questions/345838/ball-to-ball-collision-detection-and-handling

    // first step is to rewind time to the point of collision.
    // RAM: TODO

    auto np1 = p1;
    auto np2 = p2;

    auto collisionNormal = Normalise(np1 - np2);

    // Decompose v1 in parallel and orthogonal part
    auto v1Dot = Dot(collisionNormal, v1);
    auto v1Collide = collisionNormal * v1Dot;
    auto v1Remainder = v1 - v1Collide;

    // Decompose v2 in parallel and orthogonal part
    auto v2Dot = Dot(collisionNormal, v2);
    auto v2Collide = collisionNormal * v2Dot;
    auto v2Remainder = v2 - v2Collide;

    // Calculate new velocities
    auto v1New = v1Remainder + (((v1Collide * (m1 - m2)) + (v2Collide * Scalar{2} * m2)) / (m1 + m2));
    auto v2New = v2Remainder + (((v2Collide * (m2 - m1)) + (v1Collide * Scalar{2} * m1)) / (m1 + m2));

    // Now fast forward the collision again.
    // RAM: TODO:
}

*/

}}} // namespace
