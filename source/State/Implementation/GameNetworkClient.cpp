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

// RAM: Oh wow, functors FTW.
// RAM: TODO Think of a name (DeltaStatePlayerClient?)
// RAM: TODO Move to its own class
// RAM: TODO Is this even done right? what are the couplings?
class DeltaThinkOfANameLater
{
public:
    DeltaThinkOfANameLater(DeltaCoder<StatePlayerClient> coder) : myCoder(coder) {}

    std::vector<uint8_t> operator()(const StatePlayerClient& base, const StatePlayerClient& target)
    {
        // buffer size should be less than the player state size.
        BitStream bits(sizeof(StatePlayerClient));

        if  (
                (base.orientation.x == target.orientation.x) &&
                (base.orientation.y == target.orientation.y) &&
                (base.orientation.z == target.orientation.z) &&
                (base.flags == target.flags)
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
            myCoder.DeltaEncode(base, target, bits);
        }

        return bits.TakeBuffer();
    }

private:
    DeltaCoder<StatePlayerClient> myCoder;
};

GameNetworkClient::GameNetworkClient(
        StatePlayerClient identity,
        unsigned bufferSize)
    : myCoder({
                std::vector<DeltaMapItem>
                {
                    {MAKE_OFFSET(StatePlayerClient, orientation.x), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, orientation.y), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, orientation.z), MapFloatRangeStrict{0, 71071, 17_bits}},
                    {MAKE_OFFSET(StatePlayerClient, flags), MapUnsigned{7_bits}}
                },
                Research{true, true}})
    , myBuffer(identity, bufferSize, DeltaThinkOfANameLater(myCoder))
{    
}

void GameNetworkClient::Tick(StatePlayerClient newState)
{
    myBuffer.Tick(newState);
}

Delta GameNetworkClient::DeltaCreate(
        ClientHandle,
        boost::optional<Sequence> lastSequenceAcked) const
{
    return myBuffer.DeltaCreate(lastSequenceAcked);
}

}}} // namespace
