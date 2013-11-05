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

#include "ActPlayer.hpp"
#include "Flags.hpp"
#include "Vector3.hpp"

#include <cmath>

namespace GameInABox { namespace State { namespace Implementation {

// 0-100km in 6 seconds is 60m/s acceleration.
constexpr Vector3 JetImpulse() { return Vector3{1.0f, 1.0f, 1.0f}; }
constexpr Energy JetMaxEnergy() { return Energy{10000}; }
constexpr Energy JetEnergyRechargePerTick() { return Energy{10000 / (10 * 60)}; }
constexpr Energy JetEnergyBurnPerTick() { return Energy{10000 / (3 * 60)}; }

// Apply things like thrust and movement by setting velocity.

bool CollidePlayer(const Entity& protagonist, const Entity& antagonist)
{
    return (&protagonist == &antagonist);
}

struct PlayerVelocityKnobs
{
    // gravity and external forces.
    AccelerationVector external;

    Orientation look;

    bool isForward;
    bool isBack;
    bool isLeft;
    bool isRight;
    bool isJet;

    bool onTheGround;
};

struct PlayerVelocityConstants
{
    AccelerationScalar jet;
    AccelerationScalar move;

    Period tick;

    Scalar drag;
    Scalar airControl;

    Mass weight;

    Speed maxSpeed;
    Speed maxSpeedInAir;
    Speed maxSpeedGame;
};

static constexpr PlayerVelocityConstants Defaults
{
    2.0_m_s2,
    1.0_m_s2,
    {1.0/64.0},
    0.25_scalar,
    0.05_scalar,

    80.0_kg,

    8.0_m_s,
    30.0_m_s,
    50.0_m_s
};

// assumes Y is up.
// +/- Z is forward/backwards
// +/- X is right/left.
Velocity PlayerVelocity(Velocity currentVelocity, PlayerVelocityKnobs knobs, PlayerVelocityConstants constants)
{
    auto velocity = currentVelocity;
    velocity += knobs.external * constants.tick;
    velocity *= constants.drag;

    // Look
    auto intent = Orientation{0.0f};
    auto left = Orientation
    {
            -knobs.look.value.Z(),
            knobs.look.value.Y(),
            knobs.look.value.X()
    };

    if (knobs.isForward)
    {
        intent += knobs.look;
    }
    if (knobs.isBack)
    {
        intent -= knobs.look;
    }
    if (knobs.isLeft)
    {
        intent += left;
    }
    if (knobs.isRight)
    {
        intent -= left;
    }

    // Jet
    // Only know I can jet, I don't manage the "Can I Jet" state here.
    auto jet = AccelerationVector{0.0f};
    if (knobs.isJet)
    {
        if (knobs.onTheGround)
        {
            jet = Orientation{0.0f, 1.0f} * constants.jet;
        }
        else
        {
            jet = intent * constants.jet;
        }
    }

    auto velocityNew = Velocity{0.0f};
    if (!IsZero(intent.value))
    {
        // Movement
        auto control = knobs.onTheGround ? Scalar{1.0f} : constants.airControl;
        auto xyz = Normalise(intent);
        auto xy = Vector{xyz.value.X(), xyz.value.Y()};

        // what's our new velocity delta?
        auto delta = xy * constants.move * constants.tick * control;

        velocityNew = velocity + delta;

        auto speedNew = GetSpeed(velocityNew);
        auto speed = GetSpeed(velocity);

        // Only allow an increase of velocity if it's below our max speed
        if (speedNew > speed)
        {
            if (speedNew > constants.maxSpeed)
            {
                auto cap = std::max(speed, constants.maxSpeed);

                velocityNew = Normalise(velocityNew) * cap;
            }
        }
    }

    // Jet Movement.
    if (!IsZero(jet.value))
    {
        auto delta = jet * constants.tick;

        velocityNew = velocityNew + delta;

        auto speedNew = GetSpeed(velocityNew);
        auto speed = GetSpeed(velocity);

        Speed maxSpeed = {0.0f};
        if (knobs.onTheGround)
        {
            maxSpeed = constants.maxSpeed;
        }
        else
        {
            maxSpeed = constants.maxSpeedInAir;
        }

        // only if we are increasing our velocity AND we are faster than allowed
        // do we cap the velocity.
        if (speedNew > speed)
        {
            if (speedNew > maxSpeed)
            {
                auto cap = std::max(speed, maxSpeed);

                velocityNew = Normalise(velocityNew) * cap;
            }
        }
    }

    // cap to max game speed
    auto speed = GetSpeed(velocityNew);
    if (speed > constants.maxSpeedGame)
    {
        velocityNew = Normalise(velocityNew) * constants.maxSpeedGame;
    }

    return velocityNew;
}

Entity ReactPlayer(Entity player, const Entity&, const std::vector<const Entity*>&)
{
    auto result = player;

    /* To be removed, but kept here for reference until I figure out how to do stuff.
    // Jet latches on the direction you were facing when you start jetting.
    if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Jet))
    {
        if (!FlagIsSet(result.player.allowedAction, FlagsPlayerAction::Jet))
        {
            result.player.allowedAction = FlagSet(result.player.allowedAction, FlagsPlayerAction::Jet);
            result.player.jetting = result.player.input.look;
        }

        if (jetEnergy > JetEnergyBurnPerTick())
        {
            jetEnergy -= JetEnergyBurnPerTick();

            // Vector Maths bit.
            auto velocity       = Vector3{result.player.velocity.value};
            auto orientation    = Vector3{result.player.jetting.value};

            // Don't assume ImpulseJet will *always* be 1.0. Otherwise this
            // would be just an add instad.
            auto newVelocity    = orientation * JetImpulse() + velocity;

            result.player.velocity.value = newVelocity.ToVector();
        }
    }
    else
    {
        result.player.allowedAction = FlagClear(result.player.allowedAction, FlagsPlayerAction::Jet);
    }

    // Movement
    if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Ground))
    {
        auto movement = Vector3(0.0, Vector3::tagReplicate{});

        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Foward))
        {
            movement += Vector3(0,0,1);
        }
        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Back))
        {
            movement += Vector3(0,0,-1);
        }
        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Left))
        {
            movement += Vector3(-1,0,0);
        }
        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Right))
        {
            movement += Vector3(1,0,0);
        }

        movement = Normalise(movement);

        // Rotate to facing
        // Take advantage that movement y is zero.
        // Bah, do matrix maths manually.
//        {
//            float sinx = std::sin(result.player.input.bodyFacing.value);
//            float cosx = std::cos(result.player.input.bodyFacing.value);
//            auto r1 = FastVector(cosx, 0, sinx);
//            auto r2 = FastVector(0, 1, 0);
//            auto r3 = FastVector(-sinx, 0, cosx);

//            auto result =
//        }

        // Normaise
        // Multiple by speed per tick
        // save
    }

    // save values
    result.player.fuel = jetEnergy;*/

    return result;
}

Entity ReactPlayerMove(Entity player, const Entity& constants, const std::vector<const Entity*>&)
{
    if
    (
        (player.type != EntityType::Player) &&
        (constants.type != EntityType::Constants)
    )
    {
        // Assert(player.type == EntityType::Player)
        // Assert(constants.type == EntityType::Constants)

        return player;
    }


    auto intent = Orientation{0.0f};
    auto left = Orientation
    {
            -player.player.input.look.value.Z(),
            player.player.input.look.value.Y(),
            player.player.input.look.value.X()
    };

    if (FlagIsSet(player.player.input.action, FlagsPlayerAction::Foward))
    {
        intent += player.player.input.look;
    }
    if (FlagIsSet(player.player.input.action, FlagsPlayerAction::Back))
    {
        intent -= player.player.input.look;
    }
    if (FlagIsSet(player.player.input.action, FlagsPlayerAction::Left))
    {
        intent += left;
    }
    if (FlagIsSet(player.player.input.action, FlagsPlayerAction::Right))
    {
        intent -= left;
    }

    if (!IsZero(intent.value))
    {
        // Movement
        auto control = FlagIsSet(player.player.allowedAction, FlagsPlayerAction::Ground)
                ? Scalar{1.0f}
                : constants.constants.airControl;

        auto xyz = Normalise(intent);
        auto xz = Vector{xyz.value.X(), 0.0f,  xyz.value.Z()};

        // what's our new velocity delta?
        auto delta = xz * constants.constants.impulseMove * constants.constants.tick * control;

        player.player.velocity += delta;
    }

    return player;
}

Entity ReactPlayerJump(Entity protagonist, const Entity&, const std::vector<const Entity*>&)
{
    // RAM: TODO:
    return protagonist;
}

Entity ReactPlayerJet(Entity protagonist, const Entity&, const std::vector<const Entity*>&)
{
    // RAM: TODO:
    return protagonist;
}


}}} // namespace
