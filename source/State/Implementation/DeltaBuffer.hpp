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

#ifndef DELTABUFFER_HPP
#define DELTABUFFER_HPP

#include "Types.hpp"
#include "DeltaCoder.hpp"

#include <Network/Sequence.hpp>
#include <Network/Delta.hpp>
#include <Network/ClientHandle.hpp>

#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

namespace GameInABox { namespace State { namespace Implementation {

template<typename STATE>
class DeltaBuffer
{
public:
    typedef std::function<GameInABox::Network::Delta(const STATE&, const STATE&)> StateCoder;

    DeltaBuffer(STATE identity, StateCoder coder, unsigned stateBufferSize)
        : myStates(stateBufferSize)
        , myCurrentSequence()
        , myIdentity(identity)
    {
    }

    void Tick(STATE newState)
    {
        myStates.push_back(newState);
        ++myCurrentSequence;
    }

    GameInABox::Network::Delta DeltaCreate(
            GameInABox::Network::ClientHandle,
            boost::optional<GameInABox::Network::Sequence> lastSequenceAcked) const
    {
        auto base = &myIdentity;
        const auto& current = myStates.back();
        auto difference = unsigned{0};

        // Default is diff against identity (difference==0) unless otherwise.
        if (lastSequenceAcked)
        {
            difference = myCurrentSequence - lastSequenceAcked.get();
            auto bufferSize = myStates.size();

            if ((difference <= bufferSize) && (difference < 256))
            {
                base = &myStates[bufferSize - difference];
            }
            else
            {
                difference = 0;
            }
        }

        // Code
        return myStateCoder(*base, current);
    }

private:
    boost::circular_buffer<STATE> myStates;
    GameInABox::Network::Sequence myCurrentSequence;

    const StatePlayerClient     myIdentity;
    const StateCoder            myStateCoder;
};

}}} // namespace

#endif // DELTABUFFER_HPP
