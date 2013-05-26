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

using Clock = std::chrono::steady_clock;
using Oclock = Clock::time_point;

// NOTE: Don't test what you don't know from the public interface.
class TestConnection : public ::testing::Test
{
public:
    NiceMock<MockIStateManager>     stateMockNice;
    StrictMock<MockIStateManager>   stateMockStrict;
};

TEST_F(TestConnection, CreateCustomTime)
{
    // woo, my first lambda with capture.
    Oclock testTime;
    Connection toTest(stateMockStrict, [&testTime] () -> Oclock { return testTime; });

    testTime = Clock::now();
    EXPECT_FALSE(toTest.HasFailed());
    EXPECT_FALSE(toTest.IsConnected());
    EXPECT_FALSE(toTest.Handle());
    EXPECT_EQ("", toTest.FailReason());

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

// Value-Parameterized Tests require too much setup.
TEST_F(TestConnection, TimeoutClient)
{
    // Give up after "1000 seconds", even though not technically a failure
    // as we don't know how many retires are allowed.
    Oclock testTime{Clock::now()};
    Connection toTest{stateMockStrict, [&testTime] () -> Oclock { return testTime; }};

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
}

TEST_F(TestConnection, ClientServerConnect)
{
    Oclock testTime{Clock::now()};
    Connection toTestClient{stateMockNice, [&testTime] () -> Oclock { return testTime; }};
    Connection toTestServer{stateMockNice, [&testTime] () -> Oclock { return testTime; }};

    toTestClient.Start(Connection::Mode::Client);
    toTestServer.Start(Connection::Mode::Server);

    // Setup the state machine
    ON_CALL(stateMockNice, PrivateConnect( ::testing::_, ::testing::_))
            .WillByDefault(Return(boost::optional<ClientHandle>(42)));

    ON_CALL(stateMockNice, PrivateStateInfo( ::testing::_ ))
            .WillByDefault(Return(std::vector<uint8_t>()));

    // Need the same for Info() ?
    // .SetArgReferee<argument ordinal>(value)

    int count{0};
    Bytes fromServer{};
    Bytes fromClient{};
    for (;count < 1000; count++)
    {
        fromClient = toTestClient.Process(fromServer);

        // simulate 400ms ping.
        testTime += std::chrono::milliseconds{200};

        fromServer = toTestServer.Process(fromClient);

        testTime += std::chrono::milliseconds{200};

        if ((toTestClient.HasFailed()) || toTestServer.HasFailed())
        {
            break;
        }

        if ((toTestClient.IsConnected()))
        {
            break;
        }
    }

    // Client should have connected.
    EXPECT_NE(1000, count);
    EXPECT_TRUE(toTestClient.IsConnected());
    EXPECT_FALSE(toTestClient.HasFailed());
    EXPECT_EQ("", toTestClient.FailReason());

    // Without deltas server might not consider this connected, so don't test.
    EXPECT_FALSE(toTestServer.HasFailed());
    EXPECT_EQ("", toTestServer.FailReason());
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

}}} // namespace
