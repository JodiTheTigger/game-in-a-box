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
#include <memory>

#include <Implementation/NetworkManagerClientGuts.hpp>
#include <Implementation/NetworkManagerServerGuts.hpp>
#include <Implementation/NetworkProviderInMemory.hpp>
#include "MockINetworkProvider.hpp"
#include "MockIStateManager.hpp"

using namespace std;
using namespace std::chrono;
using namespace boost::asio::ip;
using Bytes = std::vector<uint8_t>;

using ::testing::Return;
using ::testing::Invoke;
using ::testing::AtLeast;
using ::testing::NiceMock;

// see how far we got.
testing::AssertionResult GotToEnd(int n, int total)
{
    if (n < total)
    {
        return testing::AssertionFailure() << "got to " << n << " of " << total;
    }
    else
    {
        return testing::AssertionSuccess() << "got to " << n << " of " << total;
    }
}

namespace GameInABox { namespace Network { namespace Implementation {

// RAM: Each client needs a state mock, a random setting, and access to a data buffer to
// get its fake delta data from. I would make a fake array filled with RLE friendly random data
// that all clients reference, and they just take a random slice of that data for their packet.
// also don't forget to use a seed for the random number generator.

struct NodeState
{
    unsigned number;
    NiceMock<MockIStateManager> state;
    udp::endpoint address;
    NetworkProviderInMemory::RandomSettings random;

    std::normal_distribution<float> latency;
};

// Class definition!
class TestClientServerN : public ::testing::Test
{
public:
    static const int TotalClients = 8;

    TestClientServerN()
        : randomEngine(4)
        , random0To1(0,1)
        , stateClients()
        , stateServer()
        , frequenciesUseful()
        , theNetwork()
        , deltaData()
    {
    }

    virtual void SetUp() override
    {
        // setup random deltadata.
        for (std::size_t i = 0; i < deltaData.size(); ++i)
        {
            deltaData[i] = 255 * random0To1(randomEngine);

            // make it RLE friendly.
            if (random0To1(randomEngine) > 0.25)
            {
                auto bits = (1 << static_cast<unsigned>(8 * random0To1(randomEngine))) - 1;
                if (random0To1(randomEngine) > 0.5)
                {
                    deltaData[i] |= bits;
                }
                else
                {
                    deltaData[i] &= 0xff ^ bits;
                }
            }
        }

        // calculate the frequencies.
        // make sure we have all 256 items in the tree.
        frequenciesUseful.fill(1);

        for (const auto& value : deltaData)
        {
            frequenciesUseful[value]++;
        }

        // Setup all states
        int id = 0;
        SetupNodeState(stateServer, id++);
        for (auto& state : stateClients)
        {
            SetupNodeState(state, id++);
        }
    }

    // Helpers
    void SetupDefaultMock(NiceMock<MockIStateManager>& mock);
    void SetupNodeState(NodeState& state, int idNumber);
    Delta DeltaCreate(
            ClientHandle,
            boost::optional<Sequence> lastAcked);

    Sequence DeltaParse(
            ClientHandle,
            const Delta& payload);

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> random0To1;

    std::array<NodeState, TotalClients> stateClients;
    NodeState stateServer;

    std::array<uint64_t, 256> frequenciesUseful;

    NetworkProviderInMemory theNetwork;

    std::array<uint8_t, TotalClients * 16 * 8 * 32> deltaData;
};

// ///////////////////
// Helpers
// ///////////////////
void TestClientServerN::SetupDefaultMock(NiceMock<MockIStateManager>& mock)
{
    // right, what do we expect?
    // Huffman frequencies, CanSend, CanReceive and Deltas.
    ON_CALL(mock, PrivateGetHuffmanFrequencies())
            .WillByDefault(Return(frequenciesUseful));

    ON_CALL(mock, PrivateConnect( ::testing::_, ::testing::_))
            .WillByDefault(Return(boost::optional<ClientHandle>(42)));

    ON_CALL(mock, PrivateStateInfo( ::testing::_ ))
            .WillByDefault(Return(std::vector<uint8_t>()));

    ON_CALL(mock, PrivateCanReceive( ::testing::_, ::testing::_))
            .WillByDefault(Return(bool(true)));

    ON_CALL(mock, PrivateCanSend( ::testing::_, ::testing::_))
            .WillByDefault(Return(bool(true)));

    ON_CALL(mock, PrivateIsConnected( ::testing::_ ))
            .WillByDefault(Return(bool(true)));

    ON_CALL(mock, PrivateDeltaCreate( ::testing::_, ::testing::_))
            .WillByDefault(Invoke(std::bind(&TestClientServerN::DeltaCreate, this, std::placeholders::_1, std::placeholders::_2)));

    ON_CALL(mock, PrivateDeltaParse( ::testing::_, ::testing::_))
            .WillByDefault(Invoke(std::bind(&TestClientServerN::DeltaParse, this, std::placeholders::_1, std::placeholders::_2)));
}


void TestClientServerN::SetupNodeState(NodeState& state, int idNumber)
{
    state.number = idNumber;
    state.latency = std::normal_distribution<float>(random0To1(randomEngine) * 2000.0f, random0To1(randomEngine) * 400.0f);
    state.random.latency = std::bind(state.latency, randomEngine);
    state.random.random0To1 = std::bind(random0To1, randomEngine);
    state.random.latencyMinimum = Milliseconds(static_cast<MillisecondStorageType>(1 + random0To1(randomEngine) * 500));
    state.random.packetLossChancePerPacket0to1 = random0To1(randomEngine);
    state.random.packetLossChanceBurst0to1 = random0To1(randomEngine);
    state.random.packetOutOfOrderChance0to1 = random0To1(randomEngine);

    SetupDefaultMock(state.state);
}

Delta TestClientServerN::DeltaCreate(
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

Sequence TestClientServerN::DeltaParse(
        ClientHandle,
        const Delta& payload)
{
    return payload.to;
}

// ///////////////////
// Tests
// ///////////////////
TEST_F(TestClientServerN, CreateClient)
{
    // Right, run the server for "18 seconds" which is 30*1000ms = 30,000ms.
    // But remember that each tick is 10ms.
    const int maxTicks = 18 * 1000 / 10;

    OClock testTime{Clock::now()};
    OClock testStart{Clock::now()};

    NetworkManagerServerGuts theServer(theNetwork, stateServer.state, [&testTime] () -> OClock { return testTime; });
    std::vector<std::unique_ptr<NetworkManagerClientGuts>> theClients{};

    // setup
    for (auto& state: stateClients)
    {
        theClients.emplace_back(new NetworkManagerClientGuts(theNetwork, state.state, [&testTime] () -> OClock { return testTime; }));
    }

    // connect all the clients to the server at once, that'll be fun.
    for (auto& client: theClients)
    {
        client->Connect(stateServer.address);
    }

    // Simulate gameloop time, client send/receive at 100fps (10ms tick)
    // server send/receieve at 20fps (50ms tick).
    // split up into 10ms increments.
    int five = 0;
    int i;
    for (i = 0; i < maxTicks; ++i)
    {
        if (five == 5)
        {
            five = 0;

            // Server
            theNetwork.RunAs(stateServer.address, stateServer.random);
            theServer.ProcessIncomming();
            theServer.SendState();

            // Clients
            int clientCount = 0;
            for (auto& client: theClients)
            {
                theNetwork.RunAs(stateClients[clientCount].address, stateClients[clientCount].random);
                client->ProcessIncomming();
                client->SendState();
            }
        }
        else
        {            
            theServer.ProcessIncomming();

            // Clients
            int clientCount = 0;
            for (auto& client: theClients)
            {
                theNetwork.RunAs(stateClients[clientCount].address, stateClients[clientCount].random);
                client->SendState();
            }

            ++five;
        }

        testTime += std::chrono::milliseconds(10);

        // quit after 1 second.
        auto duration = (Clock::now() - testStart);
        if (duration_cast<milliseconds>(duration).count() > 1000)
        {
            break;
        }
    }

    // All clients should either be connected, or failed, not still trying to connect!
    for (auto& client: theClients)
    {
        EXPECT_TRUE(client->IsConnected() | client->HasFailed());
    }

    // Don't bother with setting a time limit as I don't want tests that are valgrinded to fail
    // because they are too slow.
    //EXPECT_TRUE(GotToEnd(i, maxTicks));
}


}}} // namespace
