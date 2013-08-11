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

#ifndef GAMENETWORKCLIENT_HPP
#define GAMENETWORKCLIENT_HPP

// scratch class for now.
// used to keep snapshots for the
// network subsystem.

#include "Types.hpp"
#include "DeltaBuffer.hpp"

#include <Network/Sequence.hpp>
#include <Network/Delta.hpp>
#include <Network/ClientHandle.hpp>

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Make this an implementation of IStateManager please.
class GameNetworkClient
{
public:
    GameNetworkClient(
            StatePlayerClient identity,
            unsigned bufferSize);

    void Tick(StatePlayerClient newState);

    GameInABox::Network::Delta DeltaCreate(
            GameInABox::Network::ClientHandle,
            boost::optional<GameInABox::Network::Sequence> lastAcked) const;

private:
    DeltaBuffer<StatePlayerClient>  myBuffer;
};

}}} // namespace

#endif // GAMENETWORKCLIENT_HPP
