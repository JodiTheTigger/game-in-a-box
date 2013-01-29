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

#ifndef ISTATEMANAGER_H
#define ISTATEMANAGER_H

#include <cstdint>
#include "BuildMacros.h"

// Forward declarations
class BitStream;
class BitStreamReadOnly;

class IStateManager
{
    CLASS_NOCOPY_NOASSIGN(IStateManager);
    
public:    
    uint16_t CurrentStateTick();
    
    void DeltaGet(uint16_t tickFrom, uint16_t tickTo, uint16_t& tickFromResult, BitStream& result) const;
    void DeltaSet(uint16_t tickFrom, uint16_t tickTo, BitStreamReadOnly& source);

private:
    // Don't delete via base class, use smart pointers or the derived pointer please.
    ~IStateManager();
    
    virtual uint16_t PrivateCurrentStateTick() = 0;
    
    virtual void PrivateDeltaGet(uint16_t tickFrom, uint16_t tickTo, uint16_t& tickFromResult, BitStream& result) const = 0;
    virtual void PrivateDeltaSet(uint16_t tickFrom, uint16_t tickTo, BitStreamReadOnly& source) = 0;
};

#endif // ISTATEMANAGER_H
