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

#include "PacketChallengeResponse.h"

PacketChallengeResponse::PacketChallengeResponse(uint8_t version, uint32_t key)
    : PacketCommand(Command::ChallengeResponse)
    , myKey(key)
{
    // High byte first.
    myBuffer.push_back(version);
    myBuffer.push_back(uint8_t(key >> 24));
    myBuffer.push_back(uint8_t(key >> 16));
    myBuffer.push_back(uint8_t(key >> 8));
    myBuffer.push_back(uint8_t(key >> 0));
}

PacketChallengeResponse::~PacketChallengeResponse()
{
}

bool PacketChallengeResponse::IsValid()
{
    if (myBuffer.size() == (PayloadSize + MinimumPacketSize))
    {
        if (Command() == Command::ChallengeResponse)
        {
            if (Version() != 0)
            {
                if (Key() != 0)
                {
                    return true;
                }
            }
        }
    }

    return false;
}

uint8_t PacketChallengeResponse::Version() const
{
    return myBuffer[OffsetVersion];
}

uint32_t PacketChallengeResponse::Key() const
{
    return  uint32_t(myBuffer[OffsetKey + 0] << 24) |
            uint32_t(myBuffer[OffsetKey + 2] << 16) |
            uint32_t(myBuffer[OffsetKey + 3] << 8)  |
            uint32_t(myBuffer[OffsetKey + 4]);
}
