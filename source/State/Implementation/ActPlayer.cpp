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
#include "VectorFast.hpp"

//#include <Common/UnitOperators.hpp>

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Why doesn't includeing UnitOperators work?!
// Comparison and Binary operators for struct based types
// with a solo member called "value"
template<class DATATYPE> inline bool operator==(const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value==rhs.value;}
template<class DATATYPE> inline bool operator!=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator==(lhs,rhs);}
template<class DATATYPE> inline bool operator< (const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value< rhs.value;}
template<class DATATYPE> inline bool operator> (const DATATYPE& lhs, const DATATYPE& rhs){return  operator< (rhs,lhs);}
template<class DATATYPE> inline bool operator<=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator> (lhs,rhs);}
template<class DATATYPE> inline bool operator>=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator< (lhs,rhs);}

// RAM: TODO: unit test this.
template<class DATATYPE> inline DATATYPE& operator+=(DATATYPE& lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE& operator-=(DATATYPE& lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }

template<class DATATYPE> inline DATATYPE operator+(DATATYPE lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs)                     { lhs.value = -lhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator*(DATATYPE lhs, const DATATYPE& rhs){ lhs.value *= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator/(DATATYPE lhs, const DATATYPE& rhs){ lhs.value /= rhs.value;  return lhs; }



// 0-100km in 6 seconds is 60m/s acceleration.
constexpr VectorFast JetImpulse() { return VectorFast(1.0f, VectorFast::tagReplicate{}); }
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

    // Jet regen
    // RAM: TODO: Should energy regen be here? or at player/time colide?
    auto jetEnergy = result.player.fuel + JetEnergyRechargePerTick();

    // RAM: TODO: Just use min()
    if (jetEnergy > JetMaxEnergy())
    {
        jetEnergy = JetMaxEnergy();
    }

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
            auto velocity       = VectorFast{result.player.velocity.value};
            auto orientation    = VectorFast{result.player.jetting.value};

            // Don't assume ImpulseJet will *always* be 1.0. Otherwise this
            // would be just an add instad.
            auto newVelocity    = Mad(orientation, JetImpulse(), velocity);

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
        auto movement = VectorFast{};

        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Foward))
        {
            movement += VectorFast(0,0,1);
        }
        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Back))
        {
            movement += VectorFast(0,0,-1);
        }
        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Left))
        {
            movement += VectorFast(-1,0,0);
        }
        if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Right))
        {
            movement += VectorFast(1,0,0);
        }

        // Rotate to facing
        // Normaise
        // Multiple by speed per tick
        // save
    }

    // save values
    result.player.fuel = jetEnergy;

    return result;
}

}}} // namespace
