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

#ifndef NETWORKPROVIDER_H
#define NETWORKPROVIDER_H

#include <cstdint>
#include <vector>
#include <memory>
#include <boost/asio/ip/udp.hpp>

// RAM: TODO! Turn this into an interface please! support ip4 ip6 and whatever else.
class NetworkProvider
{
public:
    // returns true if a packet was received (passed values are touched)
    // otherwise false if there is nothing to get (passed values are not touched)
    bool GetPacket(boost::asio::ip::udp::endpoint& remoteAddress, std::unique_ptr<std::vector<uint8_t>>& data);
    
    // returns the amount of bytes reported sent by the system
    // blocking
    uint32_t SendPacket(const boost::asio::ip::udp::endpoint& targetAddress, const std::unique_ptr<std::vector<uint8_t>>& data);
};

#endif // NETWORKPROVIDER_H
