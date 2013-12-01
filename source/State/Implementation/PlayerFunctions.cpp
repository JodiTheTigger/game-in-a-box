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

}}} // namespace
