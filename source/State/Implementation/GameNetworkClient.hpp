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
#include "DeltaCoder.hpp"

#include <Network/Sequence.hpp>
#include <Network/Delta.hpp>
#include <Network/ClientHandle.hpp>

#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

namespace GameInABox { namespace State { namespace Implementation {

class GameNetworkClient
{
public:
    GameNetworkClient(GameInABox::Network::ClientHandle client, unsigned bufferSize);

    void Tick(StatePlayerClient newState);

    GameInABox::Network::Delta DeltaCreate(
            GameInABox::Network::ClientHandle handle,
            boost::optional<GameInABox::Network::Sequence> lastAcked) const;

private:
    boost::circular_buffer<StatePlayerClient> myStates;
    GameInABox::Network::Sequence myCurrentSequence;
    GameInABox::Network::ClientHandle myHandle;

    DeltaCoder<StatePlayerClient>   myCoder;
    const StatePlayerClient               myIdentity;
};

}}} // namespace

#endif // GAMENETWORKCLIENT_HPP
