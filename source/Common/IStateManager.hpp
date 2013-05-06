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
#include <vector>
#include <string>
#include <array>
#include "BuildMacros.hpp"

#include "Sequence.hpp"
#include "ClientHandle.hpp"

namespace GameInABox { namespace Common {

// Forward declarations
class BitStream;
class BitStreamReadOnly;

class IStateManager
{
    CLASS_NOCOPY_ASSIGN_MOVE(IStateManager);
    
public:
    IStateManager() {};

    std::array<uint64_t, 256>& GetHuffmanFrequencies() const;

    ClientHandle Connect(std::vector<uint8_t> connectData, bool& fail, std::string& failReason);
    void Disconnect(ClientHandle toDisconnect);

    void DeltaGet(
            ClientHandle client,
            Sequence& tickTo,
            Sequence& tickFrom,
            Sequence lastTickAcked,
            BitStream& result) const;

    void DeltaSet(
            ClientHandle client,
            Sequence tickTo,
            Sequence tickFrom,
            BitStreamReadOnly& source);

protected:
    // Don't delete via base class, use smart pointers or the derived pointer please.
    ~IStateManager();
    
private:
    virtual ClientHandle PrivateConnect(
            std::vector<uint8_t> connectData,
            bool& fail,
            std::string& failReason) = 0;

    virtual void PrivateDisconnect(ClientHandle playerToDisconnect) = 0;
    
    virtual void PrivateDeltaGet(
            ClientHandle client,
            Sequence& tickTo,
            Sequence& tickFrom,
            Sequence lastTickAcked,
            BitStream& result) const = 0;

    // may be ignored by the game state or not, we don't care.
    virtual void PrivateDeltaSet(
            ClientHandle client,
            Sequence tickTo,
            Sequence tickFrom,
            BitStreamReadOnly& source) = 0;
};

}} // namespace

#endif // ISTATEMANAGER_H
