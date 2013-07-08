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

#include <cstdint>
#include <chrono>
#include <vector>
#include <array>
#include <gmock/gmock.h>

#include <Common/Network/Connection.hpp>
#include <Common/Network/PacketDelta.hpp>
#include <Common/MockIStateManager.hpp>

using ::testing::Return;
using ::testing::AtLeast;
using ::testing::NiceMock;
using ::testing::StrictMock;

using namespace std;
using Bytes = std::vector<uint8_t>;

namespace GameInABox { namespace Common { namespace Network {

// NOTE: Don't test what you don't know from the public interface.
class TestConnection : public ::testing::Test
{
public:
    NiceMock<MockIStateManager>     stateMockNice;
    StrictMock<MockIStateManager>   stateMockStrict;
};

int Cycle(Connection& client, Connection& server, OClock &testTime, int countMax);

TEST_F(TestConnection, CreateCustomTime)
{
    // woo, my first lambda with capture.
    OClock testTime;
    Connection toTest(stateMockStrict, [&testTime] () -> OClock { return testTime; });

    testTime = Clock::now();
    EXPECT_FALSE(toTest.HasFailed());
    EXPECT_FALSE(toTest.IsConnected());
    EXPECT_FALSE(toTest.IdClient());
    EXPECT_EQ("", toTest.FailReason());
    EXPECT_FALSE(toTest.LastSequenceAck());

    auto delta = toTest.GetDefragmentedPacket();
    EXPECT_FALSE(delta.IsValid());
}

TEST_F(TestConnection, NothingwhenNotStarted)
{
    Connection toTest(stateMockStrict);
    Bytes empty{};

    for (int i = 0; i < 1000; i++)
    {
        EXPECT_EQ(empty, toTest.Process({}));
    }
}

TEST_F(TestConnection, KeyNotStarted)
{
    Connection toTest(stateMockStrict);

    EXPECT_EQ(GetNetworkKeyNil(), toTest.Key());
}


TEST_F(TestConnection, KeyFreshClient)
{
    Connection toTest(stateMockStrict);

    toTest.Start(Connection::Mode::Client);

    EXPECT_EQ(GetNetworkKeyNil(), toTest.Key());
}

TEST_F(TestConnection, KeyFreshServer)
{
    Connection toTest(stateMockStrict);

    toTest.Start(Connection::Mode::Server);

    EXPECT_NE(GetNetworkKeyNil(), toTest.Key());
}

// Value-Parameterized Tests require too much setup.
TEST_F(TestConnection, TimeoutClient)
{
    // Give up after "1000 seconds", even though not technically a failure
    // as we don't know how many retires are allowed.
    OClock testTime{Clock::now()};
    Connection toTest{stateMockStrict, [&testTime] () -> OClock { return testTime; }};

    toTest.Start(Connection::Mode::Client);

    int count{0};
    for (;count < 1000; count++)
    {
        toTest.Process({});
        testTime += std::chrono::seconds{1};

        if (toTest.HasFailed())
        {
            break;
        }
    }

    // "No timeout after 1000 seconds, please check this is a valid use case."
    EXPECT_NE(1000, count);

    EXPECT_TRUE(toTest.HasFailed());
    EXPECT_FALSE(toTest.IsConnected());
    EXPECT_NE("", toTest.FailReason());
    EXPECT_FALSE(toTest.LastSequenceAck());
}

TEST_F(TestConnection, EmptyDisconnect)
{
    Connection toTest(stateMockStrict);
    Bytes empty{};

    toTest.Disconnect("");
    toTest.Process({});

    EXPECT_TRUE(toTest.HasFailed());
    EXPECT_NE("", toTest.FailReason());
}

TEST_F(TestConnection, EmptyDisconnectWithString)
{
    Connection toTest(stateMockStrict);
    Bytes empty{};
    auto reason = std::string{"Because!"};

    toTest.Disconnect(reason);
    toTest.Process({});

    EXPECT_TRUE(toTest.HasFailed());
    EXPECT_EQ(reason, toTest.FailReason());
}

TEST_F(TestConnection, ClientServerConnect)
{
    OClock testTime{Clock::now()};
    Connection toTestClient{stateMockNice, [&testTime] () -> OClock { return testTime; }};
    Connection toTestServer{stateMockNice, [&testTime] () -> OClock { return testTime; }};

    toTestClient.Start(Connection::Mode::Client);
    toTestServer.Start(Connection::Mode::Server);

    // Setup the state machine
    ON_CALL(stateMockNice, PrivateConnect( ::testing::_, ::testing::_))
            .WillByDefault(Return(boost::optional<ClientHandle>(42)));

    ON_CALL(stateMockNice, PrivateStateInfo( ::testing::_ ))
            .WillByDefault(Return(std::vector<uint8_t>()));

    // Need the same for Info() ?
    // .SetArgReferee<argument ordinal>(value)

    auto count = Cycle(toTestClient, toTestServer, testTime, 1000);

    // Client should have connected.
    EXPECT_NE(1000, count);
    EXPECT_TRUE(toTestClient.IsConnected());
    EXPECT_FALSE(toTestClient.HasFailed());
    EXPECT_EQ("", toTestClient.FailReason());

    // Without deltas server might not consider this connected, so don't test.
    EXPECT_FALSE(toTestServer.HasFailed());
    EXPECT_EQ("", toTestServer.FailReason());
}

TEST_F(TestConnection, ClientServerConnectWithDelta)
{
    OClock testTime{Clock::now()};
    Connection toTestClient{stateMockNice, [&testTime] () -> OClock { return testTime; }};
    Connection toTestServer{stateMockNice, [&testTime] () -> OClock { return testTime; }};

    toTestClient.Start(Connection::Mode::Client);
    toTestServer.Start(Connection::Mode::Server);

    // Setup the state machine
    ON_CALL(stateMockNice, PrivateConnect( ::testing::_, ::testing::_))
            .WillByDefault(Return(boost::optional<ClientHandle>(42)));

    ON_CALL(stateMockNice, PrivateStateInfo( ::testing::_ ))
            .WillByDefault(Return(std::vector<uint8_t>()));

    // Need the same for Info() ?
    // .SetArgReferee<argument ordinal>(value)

    Cycle(toTestClient, toTestServer, testTime, 1000);

    testTime += std::chrono::milliseconds(300);

    // No ack, connection id is the 1st two bytes of the payload
    auto delta = PacketDelta{Sequence{0}, {}, 0, Bytes(42,0x20)};
    toTestServer.Process(delta.data);

    auto deltaResult = toTestServer.GetDefragmentedPacket();
    auto id = Network::IdConnection(deltaResult);
    auto payload = Network::ClientPayload(deltaResult);

    // client and server should be connected.
    EXPECT_TRUE(toTestServer.IsConnected());
    EXPECT_EQ(deltaResult, delta);
    EXPECT_FALSE(toTestServer.LastSequenceAck());
    EXPECT_EQ(toTestServer.Key(), toTestClient.Key());

    ASSERT_TRUE(toTestServer.IdConnection());
    EXPECT_EQ(toTestServer.IdConnection().get(), 0x2020);

    // test a normal delta,
    toTestServer.Process(PacketDelta{Sequence{1}, Sequence{55}, 0, Bytes(42,0x20)}.data);
    auto deltaResult2 = toTestServer.GetDefragmentedPacket();
    ASSERT_TRUE(toTestServer.LastSequenceAck());
    EXPECT_EQ(55, toTestServer.LastSequenceAck()->Value());

    // test old packet.
    toTestServer.Process(PacketDelta{Sequence{0}, Sequence{66}, 0, Bytes(42,0x20)}.data);
    auto deltaResult3 = toTestServer.GetDefragmentedPacket();
    EXPECT_FALSE(deltaResult3.IsValid());
    EXPECT_EQ(55, toTestServer.LastSequenceAck()->Value());
}

TEST_F(TestConnection, ClientServerConnectDisconnectFromClient)
{
    OClock testTime{Clock::now()};
    Connection toTestClient{stateMockNice, [&testTime] () -> OClock { return testTime; }};
    Connection toTestServer{stateMockNice, [&testTime] () -> OClock { return testTime; }};

    toTestClient.Start(Connection::Mode::Client);
    toTestServer.Start(Connection::Mode::Server);

    // Setup the state machine
    ON_CALL(stateMockNice, PrivateConnect( ::testing::_, ::testing::_))
            .WillByDefault(Return(boost::optional<ClientHandle>(42)));

    ON_CALL(stateMockNice, PrivateStateInfo( ::testing::_ ))
            .WillByDefault(Return(std::vector<uint8_t>()));

    // Need the same for Info() ?
    // .SetArgReferee<argument ordinal>(value)

    Cycle(toTestClient, toTestServer, testTime, 1000);

    testTime += std::chrono::milliseconds(300);

    toTestServer.Process(PacketDelta{Bytes(42,20)}.data);

    // Right, everyone is connected now. Lets Disconnect via the client.
    auto reason = std::string{"Because I called Disconnect()."};

    toTestClient.Disconnect(reason);

    auto count = Cycle(toTestClient, toTestServer, testTime, 100);

    // Everyone should be disconnected
    EXPECT_NE(100, count);
    EXPECT_FALSE(toTestClient.IsConnected());
    EXPECT_TRUE(toTestClient.HasFailed());
    EXPECT_EQ(reason, toTestClient.FailReason());

    EXPECT_FALSE(toTestServer.IsConnected());
    EXPECT_TRUE(toTestServer.HasFailed());
    EXPECT_EQ(reason, toTestServer.FailReason());
}

TEST_F(TestConnection, ClientServerConnectDisconnectFromServer)
{
    OClock testTime{Clock::now()};
    Connection toTestClient{stateMockNice, [&testTime] () -> OClock { return testTime; }};
    Connection toTestServer{stateMockNice, [&testTime] () -> OClock { return testTime; }};

    toTestClient.Start(Connection::Mode::Client);
    toTestServer.Start(Connection::Mode::Server);

    // Setup the state machine
    ON_CALL(stateMockNice, PrivateConnect( ::testing::_, ::testing::_))
            .WillByDefault(Return(boost::optional<ClientHandle>(42)));

    ON_CALL(stateMockNice, PrivateStateInfo( ::testing::_ ))
            .WillByDefault(Return(std::vector<uint8_t>()));

    // Need the same for Info() ?
    // .SetArgReferee<argument ordinal>(value)

    Cycle(toTestClient, toTestServer, testTime, 1000);

    testTime += std::chrono::milliseconds(300);

    toTestServer.Process(PacketDelta{Bytes(42,20)}.data);

    // Right, everyone is connected now. Lets Disconnect via the client.
    auto reason = std::string{"Because Im the server!"};

    toTestServer.Disconnect(reason);

    auto count = Cycle(toTestClient, toTestServer, testTime, 100);

    // Everyone should be disconnected
    EXPECT_NE(100, count);
    EXPECT_FALSE(toTestClient.IsConnected());
    EXPECT_TRUE(toTestClient.HasFailed());
    EXPECT_EQ(reason, toTestClient.FailReason());

    EXPECT_FALSE(toTestServer.IsConnected());
    EXPECT_TRUE(toTestServer.HasFailed());
    EXPECT_EQ(reason, toTestServer.FailReason());
}

int Cycle(Connection& client, Connection& server, OClock &testTime, int countMax)
{
    int count{0};
    Bytes fromServer{};
    Bytes fromClient{};
    for (;count < countMax; count++)
    {
        fromClient = client.Process(fromServer);

        // simulate 400ms ping.
        testTime += std::chrono::milliseconds{200};

        fromServer = server.Process(fromClient);

        testTime += std::chrono::milliseconds{200};

        // Don't escape early if we're going though a
        // disconnect process.
        if ((client.HasFailed()) || server.HasFailed())
        {
            if ((client.HasFailed()) && server.HasFailed())
            {
                break;
            }
        }
        else
        {
            if ((client.IsConnected()))
            {
                break;
            }
        }
    }

    return count;
}

}}} // namespace
