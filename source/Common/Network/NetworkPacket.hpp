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

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <vector>
#include <memory>
#include <boost/asio/ip/udp.hpp>
#endif

#include "Common/BuildMacros.hpp"

namespace GameInABox { namespace Common { namespace Network {

class NetworkPacket
{
public:
    std::vector<uint8_t>                data;
    boost::asio::ip::udp::endpoint      address;

    NetworkPacket()
        : data()
        , address()
    {
    }

    NetworkPacket(
            std::vector<uint8_t> dataToUse,
            boost::asio::ip::udp::endpoint addressToUse)
        : data(dataToUse)
        , address(addressToUse)
    {
    }

    NetworkPacket(const NetworkPacket&) = default;
    NetworkPacket& operator= ( NetworkPacket const &) = default;
};

}}} // namespace

#endif // NETWORKPACKET_H
