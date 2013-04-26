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

#ifndef NETWORKPACKET_H
#define NETWORKPACKET_H

#include <cstdint>
#include <vector>
#include <memory>
#include <boost/asio/ip/udp.hpp>

#include "Common/BuildMacros.hpp"

namespace GameInABox { namespace Common { namespace Network {

class NetworkPacket
{
public:
    boost::asio::ip::udp::endpoint      address;
    std::vector<uint8_t>                data;

    NetworkPacket()
        : address()
        , data()
    {
    }

    NetworkPacket(
            boost::asio::ip::udp::endpoint addressToUse,
            std::vector<uint8_t> dataToUse)
        : address(addressToUse)
        , data(dataToUse)
    {
    }

    NetworkPacket(const NetworkPacket&) = default;
    NetworkPacket& operator= ( NetworkPacket const &) = default;
};

}}} // namespace

#endif // NETWORKPACKET_H
