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

using Packets = std::vector<NetworkPacket>;

// Class definition!
class TestNetworkProviderSynchronous : public ::testing::Test
{
public:
    TestNetworkProviderSynchronous()
        : myIpv4()
        , myIpv6(udp::endpoint(udp::v6(), 0))
        // boost has no way of getting local or loopback addresses.
        // using hard coded general defaults.
        , myIpv4loopback(address::from_string("127.0.0.1"))
        , myIpv4blackhole(address::from_string("10.24.76.43"))
    {
    }

    NetworkProviderSynchronous myIpv4;
    NetworkProviderSynchronous myIpv6;
    address myIpv4loopback;
    address myIpv4blackhole;
};

//std::vector<NetworkPacket> PrivateReceive() override;
//void PrivateSend(std::vector<NetworkPacket> packets) override;
//void PrivateReset() override;
//void PrivateFlush() override;
//void PrivateDisable() override;

TEST_F(TestNetworkProviderSynchronous, Ip4NoPermissionBind)
{
    // expect exception about permissions.
    ASSERT_THROW(
        NetworkProviderSynchronous notAllowed(udp::endpoint(udp::v4(), 1023)),
        boost::system::system_error);
}

TEST_F(TestNetworkProviderSynchronous, Ip4IsValid)
{
    EXPECT_FALSE(myIpv4.IsDisabled());
}

TEST_F(TestNetworkProviderSynchronous, Ip4Disable)
{
    EXPECT_FALSE(myIpv4.IsDisabled());
    myIpv4.Disable();
    EXPECT_TRUE(myIpv4.IsDisabled());
}

TEST_F(TestNetworkProviderSynchronous, Ip4DisableThenReset)
{
    EXPECT_FALSE(myIpv4.IsDisabled());
    myIpv4.Disable();
    EXPECT_TRUE(myIpv4.IsDisabled());
    myIpv4.Reset();
    EXPECT_FALSE(myIpv4.IsDisabled());
}

TEST_F(TestNetworkProviderSynchronous, Ip4Reset)
{
    EXPECT_FALSE(myIpv4.IsDisabled());
    myIpv4.Reset();
    EXPECT_FALSE(myIpv4.IsDisabled());
}

TEST_F(TestNetworkProviderSynchronous, Ip4Flush)
{
    myIpv4.Flush();
}

TEST_F(TestNetworkProviderSynchronous, Ip4FlushWhenDisabled)
{
    myIpv4.Disable();
    myIpv4.Flush();
}

TEST_F(TestNetworkProviderSynchronous, Ip4NotExpectingData)
{
    Packets data(myIpv4.Receive());

    EXPECT_EQ(0, data.size());
}

TEST_F(TestNetworkProviderSynchronous, Ip4SendEmptyArray)
{
    myIpv4.Send({{}});
}


TEST_F(TestNetworkProviderSynchronous, Ip4SendEmptyPacket)
{
    Packets toSend;

    toSend.emplace_back();

    myIpv4.Send(toSend);
}

TEST_F(TestNetworkProviderSynchronous, Ip4SendPacketNoAddress)
{
    Packets toSend;

    // should ignore the invalid address.
    toSend.emplace_back(Bytes(4,42), udp::endpoint());

    myIpv4.Send(toSend);
}

TEST_F(TestNetworkProviderSynchronous, Ip4SendPacketLoopback)
{
    Packets toSend;

    toSend.emplace_back(Bytes(4,42), udp::endpoint(myIpv4loopback, 4444));
    myIpv4.Send(toSend);
}

TEST_F(TestNetworkProviderSynchronous, Ip4SendPacketBlackHole)
{
    Packets toSend;

    toSend.emplace_back(Bytes(4,42), udp::endpoint(myIpv4blackhole, 4444));
    myIpv4.Send(toSend);
}

TEST_F(TestNetworkProviderSynchronous, Ip4SendPacketLoopbackAndReceive)
{
    Packets toSend;
    NetworkProviderSynchronous listen(udp::endpoint(myIpv4loopback, 4444));

    toSend.emplace_back(Bytes(4,42), udp::endpoint(myIpv4loopback, 4444));
    myIpv4.Send(toSend);
    auto result(listen.Receive());

    ASSERT_EQ(1, result.size());
    EXPECT_EQ(udp::endpoint(myIpv4loopback, 4444), result[0].address);
    EXPECT_EQ(Bytes(4,42), result[0].data);
}

}}} // namespace
