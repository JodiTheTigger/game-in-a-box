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

#include <Common/Network/NetworkManagerClient.hpp>
#include <Common/Network/NetworkManagerServer.hpp>
#include <Common/Network/MockINetworkProvider.hpp>
#include <Common/Network/NetworkProviderInMemory.hpp>
#include <Common/MockIStateManager.hpp>

// RAM: TODO: Test CanSend and CanReceive been false.
// RAM: TODO: Test IStateManager: Not Connected.
// RAM: TODO: Test IStateManager: return failed connection (with reason).

using namespace std;
using namespace boost::asio::ip;
using Bytes = std::vector<uint8_t>;

using ::testing::Return;
using ::testing::AtLeast;
using ::testing::StrictMock;

namespace GameInABox { namespace Common { namespace Network {

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

    StrictMock<MockIStateManager> stateMockServer;
    StrictMock<MockIStateManager> stateMockClient;
    std::array<uint64_t, 256> frequencies;
    std::array<uint64_t, 256> frequenciesUseful;

    NetworkProviderInMemory theNetwork;
};


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
    // right, what do we expect?
    // Huffman frequencies, CanSend, CanReceive and Deltas
    // for both client and server.

    // Client
    // ======
    EXPECT_CALL(stateMockClient, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequenciesUseful));

    EXPECT_CALL(stateMockClient, PrivateConnect( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(boost::optional<ClientHandle>(42)));

    EXPECT_CALL(stateMockClient, PrivateStateInfo( ::testing::_ ))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(std::vector<uint8_t>()));

    EXPECT_CALL(stateMockClient, PrivateCanReceive( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(stateMockClient, PrivateCanSend( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(stateMockClient, PrivateIsConnected( ::testing::_ ))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    /* RAM: TODO
    MOCK_CONST_METHOD2(PrivateDeltaCreate, Delta (
                           ClientHandle,
                           boost::optional<Sequence>));
    MOCK_METHOD2(PrivateDeltaParse, Sequence (
                    ClientHandle,
                    const Delta&));*/

    // Server
    // ======
    EXPECT_CALL(stateMockServer, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequenciesUseful));

    EXPECT_CALL(stateMockServer, PrivateConnect( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(boost::optional<ClientHandle>(88)));

    EXPECT_CALL(stateMockServer, PrivateStateInfo( ::testing::_ ))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(std::vector<uint8_t>()));

    EXPECT_CALL(stateMockServer, PrivateCanReceive( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(stateMockServer, PrivateCanSend( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));

    EXPECT_CALL(stateMockServer, PrivateIsConnected( ::testing::_ ))
            .Times(AtLeast(1))
            .WillRepeatedly(Return(bool(true)));


    // Careful using the same state machine.
    NetworkManagerServer server{theNetwork, stateMockServer};
    NetworkManagerClient client{theNetwork, stateMockClient};

    auto addressServer = udp::endpoint{address_v4(1l), 13444};
    auto addressClient = udp::endpoint{address_v4(2l), 4444};

    // Should be connected after 100 tries!
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
            keepGoing = !(client.IsConnected() || client.HasFailed());
        }
    }

    EXPECT_TRUE(client.IsConnected());
    EXPECT_FALSE(client.HasFailed());
}


}}} // namespace


