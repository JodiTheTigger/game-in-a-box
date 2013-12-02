#ifndef ENTITYCONSTANTS_HPP
#define ENTITYCONSTANTS_HPP

#include "EntityTypes.hpp"

namespace GameInABox { namespace State { namespace Implementation {

struct EntityConstants
{
    Period tick;

    AccelerationVector gravity;

    AccelerationScalar impulseMove;
    AccelerationScalar impulseJump;
    AccelerationScalar impulseJet;

    Scalar airControl;

    // Not sure this is the correct unit to use
    // as I cannot get the maths to make sense
    // converting this to acceleration.
    Energy jetEnergyUsedPerTick;
    Energy regenerationPerTickJetEnergy;

    Ammo ammoPerShot;
    Ammo regenerationPerTickAmmo;

    Health regenerationPerTickHealth;
    Health deadHealth;

    // Missle
    Length explodeSizeDeltaPerTick;
    Length explodeMaxSize;
    Speed missleSpeed;
};

}}} // namespace

#endif // ENTITYCONSTANTS_HPP
