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

struct PlayerKnobs
{
    Acceleration gravity;
    Acceleration external;
    Acceleration jet;
    Acceleration move;

    Orientation look;

    Period tick;
    Scalar drag;
    Scalar airControl;
    Speed maxSpeed;
    Speed maxSpeedInAir;
    Speed maxSpeedGame;

    bool isForward;
    bool isBack;
    bool isLeft;
    bool isRight;
    bool isJet;

    bool onTheGround;
};

Velocity PlayerVelocity(Velocity currentVelocity, PlayerKnobs knobs)
{
    auto velocity = currentVelocity;
    velocity += knobs.gravity * knobs.tick;
    velocity += knobs.external * knobs.tick;
    velocity *= knobs.drag;

    // Look
    auto intent = Orientation{0.0f};
    auto left = Orientation
    {
            -knobs.look.value.Y(),
            knobs.look.value.X(),
            knobs.look.value.Z()
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
    auto jet = Acceleration{0.0f};
    if (knobs.isJet)
    {
        if (knobs.onTheGround)
        {
            jet = Orientation{0.0f, 0.0f, 1.0f} * knobs.jet;
        }
        else
        {
            jet = intent * knobs.jet;
        }
    }

    auto velocityNew = Velocity{0.0f};
    if (!IsZero(intent.value))
    {
        // Movement
        auto control = knobs.onTheGround ? Scalar{1.0f} : knobs.airControl;
        auto xyz = Orientation{Normalise(intent.value)};
        auto xy = Orientation{xyz.value.X(), xyz.value.Y()};

        // what's our new velocity delta?
        auto delta = xy * knobs.move * knobs.tick * control;

        velocityNew = velocity + delta;

        auto speedNew = Speed{LengthF(velocityNew.value)};
        auto speed = Speed{LengthF(velocity.value)};

        // Only allow an increase of velocity if it's below our max speed
        if (speedNew > speed)
        {
            if (speedNew > knobs.maxSpeed)
            {
                auto cap = std::max(speed, knobs.maxSpeed);

                velocityNew = Multiplier{Normalise(velocityNew.value)} * cap;
            }
        }
    }

    // Jet Movement.
    if (!IsZero(jet.value))
    {
        auto delta = jet * knobs.tick;

        velocityNew = velocityNew + delta;

        auto speedNew = Speed{LengthF(velocityNew.value)};
        auto speed = Speed{LengthF(velocity.value)};

        Speed maxSpeed = {0.0f};
        if (knobs.onTheGround)
        {
            maxSpeed = knobs.maxSpeed;
        }
        else
        {
            maxSpeed = knobs.maxSpeedInAir;
        }

        // only if we are increasing our velocity AND we are faster than allowed
        // do we cap the velocity.
        if (speedNew > speed)
        {
            if (speedNew > maxSpeed)
            {
                auto cap = std::max(speed, maxSpeed);

                velocityNew = Multiplier{Normalise(velocityNew.value)} * cap;
            }
        }
    }

    // cap to max game speed
    auto speed = Speed{LengthF(velocityNew.value)};
    if (speed > knobs.maxSpeedGame)
    {
        velocityNew = Multiplier{Normalise(velocityNew.value)} * knobs.maxSpeedGame;
    }

    return velocityNew;
}

Entity ReactPlayer(Entity protagonist, const Entity&, const std::vector<const Entity*>&)
{
    auto result = protagonist;

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

}}} // namespace
