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

#include "PacketChallengeResponse.hpp"

using namespace GameInABox::Network::Implementation;

PacketChallengeResponse::PacketChallengeResponse(uint8_t version, NetworkKey key, std::vector<uint8_t> payload)
    : PacketCommandWithKey(key)
{
    data.reserve(payload.size() + OffsetPayload());
    data.push_back(version);
    data.insert(end(data), begin(payload), end(payload));
}

PacketChallengeResponse::PacketChallengeResponse(uint8_t version, NetworkKey key)
    : PacketChallengeResponse(version, key, {})
{
}

PacketChallengeResponse::~PacketChallengeResponse()
{
}

bool PacketChallengeResponse::IsValid() const
{
    if (PacketCommandWithKey::IsValid())
    {
        if (data.size() >= PacketSize)
        {
            if (Version() != 0)
            {
                return true;
            }
        }
    }

    return false;
}

uint8_t PacketChallengeResponse::Version() const
{
    return data[OffsetVersion];
}
