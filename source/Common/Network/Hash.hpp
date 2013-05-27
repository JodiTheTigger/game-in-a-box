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

#ifndef HASH_HPP
#define HASH_HPP

/*
#ifndef USING_PRECOMPILED_HEADERS
#include <boost/asio/ip/udp.hpp>
#endif

// boost::asio::ip::basic_endpoint<typename P>

// TODO: Make a hash for ip4 address and ip6 address.
boost::asio::ip::address_v6;
boost::asio::ip::address_v4;

template<>
struct hash<boost::asio::ip::address_v6>
{
    size_t operator()(const boost::asio::ip::address_v6 &address) const
    {
        size_t result{}
        auto byteHasher = std::hash<uint8_t>();

        for (auto byte : address.to_bytes())
        {
            // Don't want 1234:5678 to hash to the same as
            // 5678:1234.
            result ^= byteHasher(byte);
            result *= 53;
        }

        return result;
    }
}

template<>
struct hash<boost::asio::ip::udp::endpoint>
{
    size_t operator()(const boost::asio::ip::udp::endpoint &endpoint) const
    {
        auto address = endpoint.address();
        size_t addressHash{};

        if (address.is_v4())
        {
            // treat the ip address as a 32bit number.
            addressHash = std::hash<uint32_t>(){address.to_v4().to_ulong()};
        }
        else
        {
            auto byteHasher = std::hash<uint8_t>();

            for (auto byte : address.to_v6().to_bytes())
            {
                // Don't want 1234:5678 to hash to the same as
                // 5678:1234.
                addressHash ^= byteHasher(byte);
                addressHash *= 53;
            }
        }

        auto hashPort = std::hash<uint16_t>(){endpoint.port()};

        return theHash ^ (hashPort << 1);
    }
};

*/

#endif // HASH_HPP
