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
//#include <Common/UnitOperators.hpp>

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Why doesn't including UnitOperators work?!
// RAM: Due to ADL failure. FastVector+ is found first, so
// RAM: namespace seaching stops, so it never finds the global
// RAM: namespace. Copying the templates to here puts it into the
// RAM: GameInABox::State::Implementation namespace (local) so they are found.
template<class DATATYPE> inline bool operator==(const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value==rhs.value;}
template<class DATATYPE> inline bool operator!=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator==(lhs,rhs);}
template<class DATATYPE> inline bool operator< (const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value< rhs.value;}
template<class DATATYPE> inline bool operator> (const DATATYPE& lhs, const DATATYPE& rhs){return  operator< (rhs,lhs);}
template<class DATATYPE> inline bool operator<=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator> (lhs,rhs);}
template<class DATATYPE> inline bool operator>=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator< (lhs,rhs);}

template<class DATATYPE> inline DATATYPE& operator+=(DATATYPE& lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE& operator-=(DATATYPE& lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }

template<class DATATYPE> inline DATATYPE operator+(DATATYPE lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs)                     { lhs.value = -lhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator*(DATATYPE lhs, const DATATYPE& rhs){ lhs.value *= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator/(DATATYPE lhs, const DATATYPE& rhs){ lhs.value /= rhs.value;  return lhs; }


// 0-100km in 6 seconds is 60m/s acceleration.
constexpr Vector3 JetImpulse() { return Vector3(1.0f, Vector3::tagReplicate{}); }
constexpr Energy JetMaxEnergy() { return Energy{10000}; }
constexpr Energy JetEnergyRechargePerTick() { return Energy{10000 / (10 * 60)}; }
constexpr Energy JetEnergyBurnPerTick() { return Energy{10000 / (3 * 60)}; }

// Apply things like thrust and movement by setting velocity.

bool CollidePlayer(const Entity& protagonist, const Entity& antagonist)
{
    return (&protagonist == &antagonist);
}

Entity ReactPlayer(Entity protagonist, const Entity&, const std::vector<const Entity*>&)
{
    auto result = protagonist;
/*
    // Jet regen
    // RAM: TODO: Should energy regen be here? or at player/time colide?
    // RAM: All values should be either per second, or impulse values.
    auto jetEnergy = result.player.fuel + JetEnergyRechargePerTick();

    jetEnergy = Min(jetEnergy, JetMaxEnergy());

    // Jet Movement:
    // On ground: Inherit horizonal movement + up.
    // In air: up
    // In air with direction key pressed: Look direction.

    // Max velocity:
    // There are two max velocities:
    // Game max: nothing to go faster than this (game breaks)
    // Player max: player can't manually go faster than this

    // Pseudo code:
    // external influences
    auto velocity = currentVelocity;
    velocity += gravity;
    velocity += impulseFromExternalEvents;
    velocity *= oneMinusDrag;

    // Character input movement.
    auto intent = Vector3{};
    auto left = Vector3{-look.Y(), look.X(), look.Z()};

    if (Forward())
    {
        intent += look;
    }
    if (Back())
    {
        intent -= look;
    }
    if (Left())
    {
        intent += left;
    }
    if (Right())
    {
        intent -= left;
    }

    // Jetting
    // RAM: TODO: Use lambda to set value once.
    auto jetVelocity = Vector3{};
    if (Jetting() && jetEnery > jetSpend)
    {
        jetEnergy -= jetSpend;

        if (OnGround())
        {
            jetVelocity = Vector3{0.0f, 0.0f, 1.0f} * JetImpulse;
        }
        else
        {
            jetVelocity = intent * JetImpulse;
        }
    }

    // XY movement.
    if (!IsZero(intent))
    {
        intent = Normalise(intent);

        auto xy = Vector3{intent.X(), intent.Y()};

        auto delta = xy * moveValue * inAirModifier * timeStep;
        auto velocityNew = velocity + delta;
        auto speedNew = LengthF(velocityNew);
        auto speed = LengthF(Vector3{velocity.X(), velocity.Y()});

        // only if we are increasing our velocity AND we are faster than allowed
        // do we cap the velocity.
        if (speedNew > speed)
        {
            if (speedNew > MaxSpeed)
            {
                auto cap = std::max(speed, MaxSpeed);

                velocityNew = Normalise(velocitNew) * cap;
            }
        }
    }

    if (!IsZero(jetVelocity))
    {

    }
*/


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
