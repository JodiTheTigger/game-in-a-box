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
    auto packetToClient = NetworkPacket{payloads[1], addressClient};

    buffer.RunAs(addressServer);
    buffer.Send({packetToClient});

    buffer.RunAs(addressClient);
    auto result = buffer.Receive();

    ASSERT_EQ(1, result.size());
    EXPECT_EQ(payloads[1], result[0].data);
}

TEST_F(TestNetworkProviderInMemory, PacketLoss50Percent)
{
    NetworkProviderInMemory::OClock testTime{NetworkProviderInMemory::Clock::now()};
    std::vector<uint8_t> payload = {1,2,3,4};

    NetworkProviderInMemory buffer(
        NetworkProviderInMemory::Milliseconds{0},
        NetworkProviderInMemory::Milliseconds{0},
        NetworkProviderInMemory::Milliseconds{0},
        0.5f,
        0.75f,
        0,
        [&testTime] () -> NetworkProviderInMemory::OClock { return testTime; });


    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};
    auto packetToClient = NetworkPacket{payload, addressClient};

    buffer.RunAs(addressServer);
    buffer.Send({packetToClient, packetToClient, packetToClient, packetToClient, packetToClient, packetToClient, packetToClient, packetToClient});

    buffer.RunAs(addressClient);
    auto result = buffer.Receive();

    EXPECT_NE(result.size(), 8);
    EXPECT_NE(result.size(), 0);
}

TEST_F(TestNetworkProviderInMemory, LatencyInOrder)
{
    NetworkProviderInMemory::OClock testTime{NetworkProviderInMemory::Clock::now()};
    std::vector<std::vector<uint8_t>> payloads = {{1,2,3,4}, {4,4}, {3,5,6,7}, {8,8,8,8,8,8,8,8}};

    NetworkProviderInMemory buffer(
        NetworkProviderInMemory::Milliseconds{100},
        NetworkProviderInMemory::Milliseconds{150},
        NetworkProviderInMemory::Milliseconds{50},
        0,
        0,
        0,
        [&testTime] () -> NetworkProviderInMemory::OClock { return testTime; });


    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};
    auto packets = std::vector<NetworkPacket>{};
    for (auto payload : payloads)
    {
        packets.emplace_back(payload, addressClient);
    }

    buffer.RunAs(addressServer);
    for (int i = 0; i < 8; i++)
    {
        buffer.Send(packets);
        testTime += std::chrono::milliseconds(10);
    }

    testTime -= std::chrono::milliseconds(8 * 10);

    buffer.RunAs(addressClient);
    auto result = buffer.Receive();
    EXPECT_EQ(result.size(), 0);

    // Minimum latency is 100 ms.
    testTime -= std::chrono::milliseconds(99);
    result = buffer.Receive();
    EXPECT_EQ(result.size(), 0);

    // Should get all packets after 1 second.
    testTime += std::chrono::milliseconds(1001);
    result = buffer.Receive();
    EXPECT_EQ(result.size(), 8 * payloads.size());

    int i = 0;
    for (auto& packet : result)
    {
        EXPECT_EQ(payloads[i], packet.data);
        i = (i + 1) % payloads.size();
    }
}

TEST_F(TestNetworkProviderInMemory, LatencyOutOfOrder)
{
    NetworkProviderInMemory::OClock testTime{NetworkProviderInMemory::Clock::now()};
    std::vector<std::vector<uint8_t>> payloads = {{1,2,3,4}, {4,4}, {3,5,6,7}, {8,8,8,8,8,8,8,8}};

    NetworkProviderInMemory buffer(
        NetworkProviderInMemory::Milliseconds{100},
        NetworkProviderInMemory::Milliseconds{150},
        NetworkProviderInMemory::Milliseconds{50},
        0,
        0,
        1.0,
        [&testTime] () -> NetworkProviderInMemory::OClock { return testTime; });

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};
    auto packets = std::vector<NetworkPacket>{};
    for (auto payload : payloads)
    {
        packets.emplace_back(payload, addressClient);
    }

    buffer.RunAs(addressServer);
    buffer.Send(packets);

    testTime += std::chrono::milliseconds(1000);

    buffer.RunAs(addressClient);
    auto result = buffer.Receive();
    EXPECT_EQ(result.size(), payloads.size());

    bool inOrder = true;
    for (std::size_t i = 0; i < result.size(); ++i)
    {
        if (result[i].data != payloads[i])
        {
            inOrder = false;
        }
    }

    EXPECT_FALSE(inOrder);
}

}}} // namespace
