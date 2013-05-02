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

#include <Common/Network/NetworkProviderSynchronous.hpp>
#include <gmock/gmock.h>

using namespace std;
using namespace boost::asio::ip;
using Bytes = std::vector<uint8_t>;

// NOTE: Don't run this test with Administrator / root priviliages
// as it assumes it doesn't have permission to bind to ports < 1024.

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestNetworkProviderSynchronous : public ::testing::Test
{
public:
    TestNetworkProviderSynchronous()
        : myIpv4()
        , myIpv6(udp::endpoint(udp::v6(), 0))
    {
    }

    NetworkProviderSynchronous myIpv4;
    NetworkProviderSynchronous myIpv6;
};

TEST_F(TestNetworkProviderSynchronous, Ip4NoPermissionBind)
{
    // expect exception about permissions.
//    ASSERT_THROW(
//        NetworkProviderSynchronous notAllowed(udp::endpoint(udp::v4(), 1023)),
//        error::basic_errors::access_denied);
}

}}} // namespace
