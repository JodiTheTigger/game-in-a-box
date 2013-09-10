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

#ifndef ENTITYMISSLE_HPP
#define ENTITYMISSLE_HPP

#include "Types.hpp"

// RAM: TODO: Move this to common.
#include <Network/WrappingCounter.hpp>
#include <cstdint>

namespace GameInABox { namespace State { namespace Implementation {

enum class EntityStateMissle : uint32_t
{
    Flying,
    Exploding
};

struct EntityMissle
{
    EntityStateMissle state;

    // RAM: TODO: Make this its own type.
    GameInABox::Network::WrappingCounter<std::uint_fast32_t, 32> startTick;

    // RAM: TODO: Make this its own type.
    Vec3 startPosition;

    // RAM: TODO: Make this its own type.
    uint32_t ownerId;
};

}}} // namespace


#endif // ENTITYMISSLE_HPP
