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

#include <Common/Network/NetworkProviderInMemory.hpp>
#include <gmock/gmock.h>

using namespace std;
using namespace boost::asio::ip;
using Bytes = std::vector<uint8_t>;

namespace GameInABox { namespace Common { namespace Network {

using Packets = std::vector<NetworkPacket>;

// Class definition!
class TestNetworkProviderInMemory : public ::testing::Test
{
public:
    TestNetworkProviderInMemory() {}
};

TEST_F(TestNetworkProviderInMemory, NoLatency)
{
    NetworkProviderInMemory buffer{};
    std::vector<uint8_t> payloads[2] = {{1,2,3,4}, {6,7,8,9,10}};

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};
    auto packetToServer = NetworkPacket{payloads[0], addressServer};
    auto packetToClient = NetworkPacket{payloads[1], addressClient};

    buffer.RunAs(addressServer);
    buffer.Send({packetToClient});

    buffer.RunAs(addressClient);
    auto result = buffer.Receive();

    ASSERT_EQ(1, result.size());
    EXPECT_EQ(payloads[1], result[0].data);
}

}}} // namespace
