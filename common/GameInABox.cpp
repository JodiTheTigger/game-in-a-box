/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>
    
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

#include "GameInABox.h"

uint16_t GameInABox::PrivateCurrentStateTick()
{
    // RAM: TODO!
    return 0;
}

IStateManager::ClientHandle* GameInABox::PrivateConnect(
        std::vector<uint8_t>,
        bool& fail,
        std::string&)
{
    // RAM: TODO!
    fail = true;
    //return IStateManager::ClientHandle();
    return nullptr;
}

void GameInABox::PrivateDisconnect(ClientHandle*)
{
    // RAM: TODO
}

void GameInABox::PrivateDeltaGet(uint16_t, uint16_t, uint16_t&, BitStream&) const
{
    // RAM: TODO!
}

void GameInABox::PrivateDeltaSet(uint16_t, uint16_t, BitStreamReadOnly&)
{
    // RAM: TODO!
}
