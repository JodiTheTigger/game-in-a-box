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

GameNetworkClient::GameNetworkClient(ClientHandle client, unsigned bufferSize)
    : myStates(bufferSize)
    , myCurrentSequence()
    , myHandle(client)
    , myCoder({
                std::vector<DeltaMapItem>
                {
                    {MAKE_OFFSET(StatePlayerClient, orientation.x), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, orientation.y), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, orientation.z), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, flags), MapUnsigned{7_bits}}
                },
                Research{true, true}})
    , myIdentity{Vec3{0,0,0},FlagsPlayer::NoFlags}
{
}


void GameNetworkClient::Tick(StatePlayerClient)
{

}

Delta GameNetworkClient::DeltaCreate(
        ClientHandle handle,
        boost::optional<Sequence> lastSequenceAcked) const
{
    if (handle != myHandle)
    {
        // RAM: TODO: Log: Shouldn't happen.
        return Delta();
    }

    // ////
    // First, perfect state, then corner cases.
    // ////

    // buffer size should be less than the player state size.
    BitStream bits(sizeof(StatePlayerClient));

    if (lastSequenceAcked)
    {
        unsigned indexBase = lastSequenceAcked.get().Value(); // RAM: TODO: FIX

        myCoder.DeltaEncode(myStates[indexBase], myStates.back(), bits);
    }
    else
    {
        // diff against identity.
        myCoder.DeltaEncode(myIdentity, myStates.back(), bits);
    }

    return Delta{
        Sequence{0}, // RAM: TODO: FIX
        myCurrentSequence,
        bits.TakeBuffer()};
}


}}} // namespace
