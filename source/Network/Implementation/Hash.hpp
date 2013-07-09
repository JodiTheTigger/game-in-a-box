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

#ifndef USING_PRECOMPILED_HEADERS
#include <boost/asio/ip/udp.hpp>
#endif

// Hash functions for boost::asio addresses.
// NOTE: Will produce mismatched-tags warnings because the standard uses
// both struct hash and class hash. Ugh.

// You are explicitly allowed to extend the std:: namespace
// for hash functions for custom types.
namespace std {

// /////////////////
// IPv6 Address
// /////////////////
template<>
struct hash<boost::asio::ip::address_v6>
{
    size_t operator()(const boost::asio::ip::address_v6& address) const
    {
        size_t result{};

        // http://stackoverflow.com/questions/12793653/how-to-calculate-a-hash-value-from-different-fields-of-an-ip-packet
        for (auto byte : address.to_bytes())
        {
            // Don't want 1234:5678 to hash to the same as
            // 5678:1234.
            result ^= (result << 3) | byte;
        }

        return result;
    }
};

// /////////////////
// IPv4 Address
// /////////////////
template<>
struct hash<boost::asio::ip::address_v4>
{
    size_t operator()(const boost::asio::ip::address_v4& address) const
    {
        return static_cast<size_t>(address.to_ulong());
    }
};


// /////////////////
// Endpoint
// /////////////////
template<typename T>
struct hash<boost::asio::ip::basic_endpoint<T>>
{
    size_t operator()(const boost::asio::ip::basic_endpoint<T>& endpoint) const
    {
        auto address = endpoint.address();
        size_t result{};

        if (address.is_v4())
        {
            result = hash<boost::asio::ip::address_v4>()(address.to_v4());
        }
        else
        {
            if (address.is_v6())
            {
                result = hash<boost::asio::ip::address_v6>()(address.to_v6());
            }
        }

        result ^= (endpoint.port() << 3);

        return result;
    }
};

} // namespace

#endif // HASH_HPP
