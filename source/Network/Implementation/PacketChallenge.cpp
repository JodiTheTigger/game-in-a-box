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

#include "PacketChallenge.hpp"

#include <string>
#include <iterator>

using namespace std;
using namespace GameInABox::Network::Implementation;

const std::string PacketChallenge::ChallengeMessage = "Why, Hello there.";

PacketChallenge::PacketChallenge()
    : Packet({}, Command::Challenge, PacketChallenge::ChallengeMessage)
{
}

PacketChallenge::PacketChallenge(std::vector<uint8_t> fromBuffer)
    : Packet(fromBuffer)
{
}

PacketChallenge::~PacketChallenge()
{
}

bool PacketChallenge::IsValid() const
{
    if (Packet::GetCommand(data) == Command::Challenge)
    {
        if (GetPayloadString(*this) == PacketChallenge::ChallengeMessage)
        {
            return true;
        }
    }

    return false;
}
