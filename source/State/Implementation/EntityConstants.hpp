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
};

}}} // namespace

#endif // ENTITYCONSTANTS_HPP
