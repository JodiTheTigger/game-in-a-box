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

#include "IStateManager.h"
#include "BitStream.h"
#include "BitStreamReadOnly.h"

IStateManager::~IStateManager()
{
    // Nothing for now.
}

uint16_t IStateManager::CurrentStateTick()
{
    return PrivateCurrentStateTick();
}

IStateManager::ClientHandle* IStateManager::Connect(std::vector<uint8_t> connectData, bool& fail, std::string &failReason)
{
    return PrivateConnect(connectData, fail, failReason);
}

void IStateManager::Disconnect(ClientHandle* toDisconnect)
{
    PrivateDisconnect(toDisconnect);
}
    
void IStateManager::DeltaGet(uint16_t tickFrom, uint16_t tickTo, uint16_t& tickFromResult, BitStream& result) const
{
    PrivateDeltaGet(tickFrom, tickTo, tickFromResult, result);
}

void IStateManager::DeltaSet(uint16_t tickFrom, uint16_t tickTo, BitStreamReadOnly& source)
{
    PrivateDeltaSet(tickFrom, tickTo, source);
}
