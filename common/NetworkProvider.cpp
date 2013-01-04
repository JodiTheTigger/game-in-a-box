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

#include "NetworkProvider.h"

bool NetworkProvider::GetPacket(boost::asio::ip::udp::endpoint& , std::vector<uint8_t>& )
{
    // TODO!
    return false;
}

// returns the amount of bytes reported sent by the system
// blocking
uint32_t NetworkProvider::SendPacket(const boost::asio::ip::udp::endpoint&, const std::vector<uint8_t>& )
{
    // TODO!
    return 0;
}

