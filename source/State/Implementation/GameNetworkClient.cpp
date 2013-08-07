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

#include "GameNetworkClient.hpp"

#include "DeltaMapItem.hpp"

#include <Common/BitStream.hpp>
#include <Common/BitStreamReadOnly.hpp>

namespace GameInABox { namespace State { namespace Implementation {

using namespace GameInABox::Network;
using namespace GameInABox::Common;

GameNetworkClient::GameNetworkClient(
        StatePlayerClient identity,
        unsigned bufferSize)
    : myStates(bufferSize)
    , myCurrentSequence()
    , myCoder({
                std::vector<DeltaMapItem>
                {
                    {MAKE_OFFSET(StatePlayerClient, orientation.x), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, orientation.y), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, orientation.z), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, flags), MapUnsigned{7_bits}}
                },
                Research{true, true}})
    , myIdentity(identity)
{
}


void GameNetworkClient::Tick(StatePlayerClient newState)
{
    myStates.push_back(newState);
    ++myCurrentSequence;
}

// ClientHandle is ignored as the network layer
// verifies we're talking to the correct person.
Delta GameNetworkClient::DeltaCreate(
        ClientHandle,
        boost::optional<Sequence> lastSequenceAcked) const
{
    auto base = &myIdentity;
    const auto& current = myStates.back();
    auto difference = unsigned{0};

    // buffer size should be less than the player state size.
    BitStream bits(sizeof(StatePlayerClient));

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

    // Don't just use == overator overloading as for delta difference.
    // We don't test against everything.
    // RAM: TODO: DO use == overloading. Change the struct strcuture so we
    // can compare stuff to make readability easier.
    if (
            (current.orientation.x == base->orientation.x) &&
            (current.orientation.y == base->orientation.y) &&
            (current.orientation.z == base->orientation.z) &&
            (current.flags == base->flags)
        )
    {
        // Same == 1 bit.
        bits.Push(true);
    }
    else
    {
        // Not same = 1 bit + encoding.
        bits.Push(false);

        // Delta code between last and current.
        myCoder.DeltaEncode(*base, current, bits);
    }

    return Delta{
        static_cast<uint8_t>(difference),
        myCurrentSequence,
        bits.TakeBuffer()};
}

}}} // namespace
