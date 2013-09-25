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

namespace GameInABox { namespace State { namespace Implementation {

// Apply things like thrust and movement by setting velocity.

bool CollidePlayer(const Entity& protagonist, const Entity& antagonist)
{
    return (&protagonist == &antagonist);
}

Entity ReactPlayer(Entity protagonist, const Entity&, const std::vector<const Entity*>&)
{
    auto result = protagonist;

    // Jet latches on the direction you were facing when you start jetting.
    if (FlagIsSet(result.player.input.action, FlagsPlayerAction::Jet))
    {
        if (!FlagIsSet(result.player.allowedAction, FlagsPlayerAction::Jet))
        {
            result.player.allowedAction = FlagSet(result.player.allowedAction, FlagsPlayerAction::Jet);
            result.player.jet = result.player.input.look;
        }

        // RAM: TODO: Add the actual thrust!
    }
    else
    {
        result.player.allowedAction = FlagClear(result.player.allowedAction, FlagsPlayerAction::Jet);
    }

    // RAM: TODO:
    // Jet thrust
    // Energy Regen
    // Movement

    return result;
}

}}} // namespace
