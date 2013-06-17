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

#include "PacketDeltaClient.hpp"
#include "BufferSerialisation.hpp"

using namespace GameInABox::Common;
using namespace GameInABox::Common::Network;


template<class DELTA>
boost::optional<uint16_t> GetIdConnection(const DELTA& delta)
{
    if (delta.data.size() >= (delta.OffsetPayload() + 2))
    {
        if (delta.IsValid())
        {
            uint16_t result;

            Pull(begin(delta.data) + delta.OffsetPayload(), result);

            return {result};
        }
    }

    return {};
}

template<class DELTA>
std::vector<uint8_t> GetClientPayload(const DELTA& delta)
{
    if (delta.data.size() > (delta.OffsetPayload() + 2))
    {
        if (delta.IsValid())
        {
            uint16_t result;

            return {begin(delta.data) + delta.OffsetPayload() + 2, end(delta.data)};
        }
    }

    return {};
}
