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

#include "DeltaCoder.hpp"
#include "Sequence.hpp"
#include "Delta.hpp"
#include "ClientHandle.hpp"

#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

namespace GameInABox { namespace Network {

template<typename STATE>
class DeltaBuffer
{
public:
    typedef std::function<std::vector<uint8_t>(const STATE&, const STATE&)> StateCoder;

    DeltaBuffer(STATE identity, unsigned stateBufferSize, StateCoder coder)
        : myStates(stateBufferSize)
        , myCurrentSequence()
        , myIdentity(identity)
        , myStateCoder(coder)
    {
    }

    void Tick(STATE newState)
    {
        myStates.push_back(newState);
        ++myCurrentSequence;
    }

    Delta DeltaCreate(boost::optional<Sequence> lastSequenceAcked) const
    {
        auto base = &myIdentity;
        const auto& current = myStates.back();
        auto difference = unsigned{0};

        // ClientHandle is ignored as the network layer
        // verifies we're talking to the correct person.

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

        return Delta
        {
            static_cast<uint8_t>(difference),
            myCurrentSequence,
            myStateCoder(*base, current)
        };
    }

private:
    boost::circular_buffer<STATE> myStates;
    GameInABox::Network::Sequence myCurrentSequence;

    const STATE         myIdentity;
    const StateCoder    myStateCoder;
};

}} // namespace

#endif // DELTABUFFER_HPP
