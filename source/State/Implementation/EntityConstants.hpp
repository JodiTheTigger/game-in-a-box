#ifndef ENTITYCONSTANTS_HPP
#define ENTITYCONSTANTS_HPP

#include "EntityTypes.hpp"

namespace GameInABox { namespace State { namespace Implementation {

struct EntityConstants
{
    AccelerationVector gravity;

    AccelerationScalar impulseMove;
    AccelerationScalar impulseJump;
    AccelerationScalar impulseJet;
};

}}} // namespace

#endif // ENTITYCONSTANTS_HPP
