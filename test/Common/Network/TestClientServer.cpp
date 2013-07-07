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

#include <gmock/gmock.h>
#include <chrono>

#include <Common/Network/NetworkManagerClient.hpp>
#include <Common/Network/NetworkManagerServer.hpp>
#include <Common/Network/MockINetworkProvider.hpp>
#include <Common/Network/NetworkProviderInMemory.hpp>
#include <Common/MockIStateManager.hpp>

using namespace std;
using namespace boost::asio::ip;
using Bytes = std::vector<uint8_t>;

using ::testing::Return;
using ::testing::Invoke;
using ::testing::AtLeast;
using ::testing::StrictMock;

using Clock = std::chrono::steady_clock;
using Oclock = Clock::time_point;

namespace GameInABox { namespace Common { namespace Network {

Delta DeltaCreate(
        ClientHandle,
        boost::optional<Sequence> lastAcked)
{
    if (lastAcked)
    {
        return {Sequence(lastAcked->Value() + 1), Sequence(lastAcked->Value() + 1), {}};
    }
    else
    {
        return {Sequence{0},Sequence{0}, {}};
    }
}

Sequence DeltaParse(
        ClientHandle,
        const Delta& payload)
{
    return payload.to;
}

// Class definition!
class TestClientServer : public ::testing::Test
{
public:
    TestClientServer()
        : stateMockServer()
        , stateMockClient()
        , frequencies()
        , frequenciesUseful()
        , theNetwork()
    {
        // all equally probable.
        frequencies.fill(1);

        // all set or all zero more probable.
        frequenciesUseful.fill(1);

        // 8 bits
        frequenciesUseful[0x00] = 10;
        frequenciesUseful[0xFF] = 10;

        // 7 bits
        frequenciesUseful[0x7F] = 9;
        frequenciesUseful[0xFE] = 9;

        // 6 bits
        frequenciesUseful[0x3F] = 8;
        frequenciesUseful[0x7E] = 8;
        frequenciesUseful[0xFC] = 8;
    }

    // Helpers
    void SetupDefaultMock(StrictMock<MockIStateManager>& mock);

    // RAM: TODO: Don't really care about the state handler.
    // Change to NiceMock, and setup ON_CALL macros
    // instead.
    StrictMock<MockIStateManager> stateMockServer;
    StrictMock<MockIStateManager> stateMockClient;
    std::array<uint64_t, 256> frequencies;
    std::array<uint64_t, 256> frequenciesUseful;

    NetworkProviderInMemory theNetwork;
};

// ///////////////////
// Helpers
// ///////////////////
void TestClientServer::SetupDefaultMock(StrictMock<MockIStateManager>& mock)
{
    // right, what do we expect?
    // Huffman frequencies, CanSend, CanReceive and Deltas.
    EXPECT_CALL(mock, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequenciesUseful));

    EXPECT_CALL(mock, PrivateConnect( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(boost::optional<ClientHandle>(42)));

    EXPECT_CALL(mock, PrivateStateInfo( ::testing::_ ))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(std::vector<uint8_t>()));

    EXPECT_CALL(mock, PrivateCanReceive( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(mock, PrivateCanSend( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(mock, PrivateIsConnected( ::testing::_ ))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(mock, PrivateDeltaCreate( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Invoke(DeltaCreate));

    EXPECT_CALL(mock, PrivateDeltaParse( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Invoke(DeltaParse));

    EXPECT_CALL(mock, PrivateDisconnect(::testing::_))
            .Times(AtLeast(1));
}

// ///////////////////
// Simple Tests
// ///////////////////
TEST_F(TestClientServer, CreateClient)
{
    // right, what do we expect?
    EXPECT_CALL(stateMockClient, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequencies));

    NetworkManagerClient toTest(theNetwork, stateMockClient);

    EXPECT_FALSE(toTest.IsConnected());
    EXPECT_FALSE(toTest.HasFailed());
    EXPECT_EQ("", toTest.FailReason());
}

TEST_F(TestClientServer, CreateServer)
{
    // right, what do we expect?
    EXPECT_CALL(stateMockServer, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequencies));

    NetworkManagerServer(theNetwork, stateMockServer);
}

// ///////////////////
// Perfect conection
// ///////////////////
TEST_F(TestClientServer, OneConnection)
{
    for (auto mock : {&stateMockClient, &stateMockServer})
    {
        SetupDefaultMock(*mock);
    }

    NetworkManagerServer server{theNetwork, stateMockServer};
    NetworkManagerClient client{theNetwork, stateMockClient};

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};

    theNetwork.RunAs(addressClient);
    client.Connect(addressServer);
    int count = 0;
    bool keepGoing = true;
    while (keepGoing)
    {
        theNetwork.RunAs(addressServer);
        server.ProcessIncomming();
        server.SendState();

        theNetwork.RunAs(addressClient);
        client.ProcessIncomming();
        client.SendState();

        if (++count > 100)
        {
            keepGoing = false;
        }
        else
        {
            keepGoing = !client.HasFailed();
        }
    }

    // Should be connected, and have sent deltas after 100 tries!
    EXPECT_TRUE(client.IsConnected());
    EXPECT_FALSE(client.HasFailed());
}

TEST_F(TestClientServer, StateDisconnect)
{
    for (auto mock : {&stateMockClient, &stateMockServer})
    {
        SetupDefaultMock(*mock);
    }

    NetworkManagerServer server{theNetwork, stateMockServer};
    NetworkManagerClient client{theNetwork, stateMockClient};

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};

    theNetwork.RunAs(addressClient);
    client.Connect(addressServer);
    int count = 0;
    bool keepGoing = true;

    while (keepGoing)
    {
        theNetwork.RunAs(addressServer);
        server.ProcessIncomming();
        server.SendState();

        theNetwork.RunAs(addressClient);
        client.ProcessIncomming();
        client.SendState();

        if (++count > 100)
        {
            keepGoing = false;
        }
        else
        {
            keepGoing = !client.HasFailed();
        }

        // don't like the client sending after 50 ticks.
        if (count == 50)
        {
            EXPECT_CALL(stateMockServer, PrivateCanSend( ::testing::_, ::testing::_))
                    .Times(AtLeast(1))
                    .WillRepeatedly(Return(bool(false)));
        }
    }

    // Should be disconnected.
    EXPECT_FALSE(client.IsConnected());
    EXPECT_TRUE(client.HasFailed());

    // RAM: TODO: Make sure the reason is not a timeout!
    EXPECT_NE("", client.FailReason());
}

TEST_F(TestClientServer, NoReceive)
{
    for (auto mock : {&stateMockClient, &stateMockServer})
    {
        SetupDefaultMock(*mock);
    }

    NetworkManagerServer server{theNetwork, stateMockServer};
    NetworkManagerClient client{theNetwork, stateMockClient};

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};

    theNetwork.RunAs(addressClient);
    client.Connect(addressServer);
    int count = 0;
    bool keepGoing = true;

    // Server isn't listening
    EXPECT_CALL(stateMockServer, PrivateCanReceive( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(false)));

    while (keepGoing)
    {
        theNetwork.RunAs(addressServer);
        server.ProcessIncomming();
        server.SendState();

        theNetwork.RunAs(addressClient);
        client.ProcessIncomming();
        client.SendState();

        if (++count > 100)
        {
            keepGoing = false;
        }
        else
        {
            keepGoing = !client.HasFailed();
        }
    }

    // Should never have connected
    EXPECT_FALSE(client.IsConnected());
    EXPECT_TRUE(client.HasFailed());

    // RAM: TODO: Make sure the reason is a timeout!
    EXPECT_NE("", client.FailReason());
}

TEST_F(TestClientServer, NoSend)
{
    for (auto mock : {&stateMockClient, &stateMockServer})
    {
        SetupDefaultMock(*mock);
    }

    NetworkManagerServer server{theNetwork, stateMockServer};
    NetworkManagerClient client{theNetwork, stateMockClient};

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};

    theNetwork.RunAs(addressClient);
    client.Connect(addressServer);
    int count = 0;
    bool keepGoing = true;

    while (keepGoing)
    {
        theNetwork.RunAs(addressServer);
        server.ProcessIncomming();
        server.SendState();

        theNetwork.RunAs(addressClient);
        client.ProcessIncomming();
        client.SendState();

        if (++count > 100)
        {
            keepGoing = false;
        }
        else
        {
            keepGoing = !client.HasFailed();
        }

        // Timeouts on this layer only work with the handshake part.
        // Once connected it never times out.
        if (count == 1)
        {
            EXPECT_CALL(stateMockClient, PrivateIsConnected( ::testing::_ ))
                    .Times(AtLeast(1))
                    .WillRepeatedly(Return(bool(false)));
        }
    }

    // Should be disconnected.
    EXPECT_FALSE(client.IsConnected());
    EXPECT_TRUE(client.HasFailed());

    // RAM: TODO: Make sure the reason is a timeout!
    EXPECT_NE("", client.FailReason());
}


// ///////////////////
// Simulated Time
// ///////////////////
TEST_F(TestClientServer, ClientTimeout)
{
    Oclock testTime{Clock::now()};

    // Client
    EXPECT_CALL(stateMockClient, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequenciesUseful));

    EXPECT_CALL(stateMockClient, PrivateCanReceive( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(stateMockClient, PrivateCanSend( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(stateMockClient, PrivateStateInfo( ::testing::_ ))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(std::vector<uint8_t>()));

    // Server
    EXPECT_CALL(stateMockServer, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequenciesUseful));

    EXPECT_CALL(stateMockServer, PrivateCanReceive( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(stateMockServer, PrivateCanSend( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    NetworkManagerServer server{theNetwork, stateMockServer, [&testTime] () -> Oclock { return testTime; }};
    NetworkManagerClient client{theNetwork, stateMockClient, [&testTime] () -> Oclock { return testTime; }};

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};

    theNetwork.RunAs(addressClient);
    client.Connect(addressServer);
    int count = 0;
    bool keepGoing = true;
    while (keepGoing)
    {
        // simulate 80+80 = 160ms latency.
        testTime += std::chrono::milliseconds(80);

        theNetwork.RunAs(addressServer);

        // simulate timeout on the client by dropping
        // packets sent to the server.
        // At this point the server has got the connection
        // request but will not get anything more.
        if (count > 0)
        {
            theNetwork.Receive();
        }

        server.ProcessIncomming();
        server.SendState();

        testTime += std::chrono::milliseconds(80);

        theNetwork.RunAs(addressClient);
        client.ProcessIncomming();
        client.SendState();

        if (++count > 100)
        {
            keepGoing = false;
        }
        else
        {
            keepGoing = !client.HasFailed();
        }
    }

    // Should be connected, and have sent deltas after 100 tries!
    EXPECT_FALSE(client.IsConnected());
    EXPECT_TRUE(client.HasFailed());
    EXPECT_NE("", client.FailReason());
}


}}} // namespace


