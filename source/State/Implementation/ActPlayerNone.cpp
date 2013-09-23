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

#include "ActPlayerNone.hpp"

#include <cstdint>

namespace GameInABox { namespace State { namespace Implementation {

Entity ReactPlayerNone(Entity protagonist, const Entity&, const std::vector<const Entity*>&)
{
    auto result = protagonist;

    result.player.energyShoot.value -= AmmoUsedPerShot.value;

    return result;
}

Entity ReactNonePlayer(Entity protagonist, const Entity& antagonist, const std::vector<const Entity*>&)
{
    auto result = protagonist;

    // change to a missle
    result.type = EntityType::Missle;

    result.missle.owner = antagonist.player.id;
    result.missle.startPosition = antagonist.player.position;
    result.missle.state = EntityStateMissle::Flying;

    // RAM: TODO: How to get the current tick? Do we need it?
    //result.missle.startTick = ???;

    return result;
}

}}} // namespace
