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

    Ammo regenerationPerTickAmmo;
};

}}} // namespace

#endif // ENTITYCONSTANTS_HPP
