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

#ifndef DELTAPARSER_HPP
#define DELTAPARSER_HPP

#include "DeltaCoder.hpp"
#include "Sequence.hpp"
#include "Delta.hpp"
#include "ClientHandle.hpp"

#include <boost/circular_buffer.hpp>
#include <boost/optional.hpp>

namespace GameInABox { namespace Network {

template<typename STATE>
class DeltaParser
{
public:
    typedef std::function<STATE(const STATE&, const std::vector<uint8_t>&)> StateCoder;

    DeltaParser(STATE identity, StateCoder coder)
        : myBase(identity)
        , myCurrentSequence()
        , myIdentity(identity)
        , myStateCoder(coder)
    {
    }

    STATE CurrentState()
    {
        // I don't like the extra copy here. But then again
        // I don't like returning a reference to my internals more.
        // And having myBase publically accessable is even worse.
        // So, it's a copy, and if we can prove its slow here because of that
        // then think about a better way.
        return myBase;
    }

    boost::optional<Sequence> DeltaParse(const Delta& payload) const
    {
        STATE* base{&myIdentity};

        if (payload.difference != 0)
        {
            if (myCurrentSequence)
            {
                auto from = Sequence(payload.to - Sequence{payload.difference});

                if (from == *myCurrentSequence)
                {
                    base = &myBase;
                }
                else
                {
                    // Ignore it if it's too old.
                    // Otherwise WTF?
                    if (from > *myCurrentSequence)
                    {
                        // Too new, WTF? resync please.
                        // (This shouldn't happen).
                        // RAM: TODO: Log this!
                        myCurrentSequence = {};
                    }

                    return myCurrentSequence;
                }
            }
            else
            {
                // wait, I don't even have a base yet.
                return {};
            }
        }

        // right, we have a new base!
        myCurrentSequence = payload.to;
        myBase = myStateCoder(base, payload.deltaPayload);

        return myCurrentSequence;
    }

private:
    STATE myBase;
    boost::optional<GameInABox::Network::Sequence> myCurrentSequence;

    const STATE         myIdentity;
    const StateCoder    myStateCoder;
};

}} // namespace

#endif // DELTAPARSER_HPP
