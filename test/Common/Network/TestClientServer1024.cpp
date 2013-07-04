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
using ::testing::NiceMock;

using Clock = std::chrono::steady_clock;
using Oclock = Clock::time_point;

namespace GameInABox { namespace Common { namespace Network {

// RAM: Each client needs a state mock, a random setting, and access to a data buffer to
// get its fake delta data from. I would make a fake array filled with RLE friendly random data
// that all clients reference, and they just take a random slice of that data for their packet.
// also don't forget to use a seed for the random number generator.

struct ClientState
{
    unsigned number;
    NiceMock<MockIStateManager> state;
    udp::endpoint address;
    NetworkProviderInMemory::RandomSettings random;

    std::normal_distribution<float> latency;
};

// Class definition!
class TestClientServer1024 : public ::testing::Test
{
public:
    TestClientServer1024()
        : randomEngine(4)
        , random0To1(0,1)
        , stateMock()
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
        unsigned id = 0;
        for (auto& state : stateMock)
        {
            state.number = id++;
            state.latency = std::normal_distribution<float>(random0To1(randomEngine) * 2000.0f, random0To1(randomEngine) * 400.0f);
            state.random.latency = std::bind(state.latency, randomEngine);
            state.random.random0To1 = std::bind(random0To1, randomEngine);
            state.random.latencyMinimum = NetworkProviderInMemory::Milliseconds(static_cast<NetworkProviderInMemory::MillisecondStorageType>(1 + random0To1(randomEngine) * 500));
            state.random.packetLossChancePerPacket0to1 = random0To1(randomEngine);
            state.random.packetLossChanceBurst0to1 = random0To1(randomEngine);
            state.random.packetOutOfOrderChance0to1 = random0To1(randomEngine);

            SetupDefaultMock(state.state);
        }
    }

    // Helpers
    void SetupDefaultMock(NiceMock<MockIStateManager>& mock);
    Delta DeltaCreate(
            ClientHandle,
            boost::optional<Sequence> lastAcked);

    Sequence DeltaParse(
            ClientHandle,
            const Delta& payload);

    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> random0To1;
    std::array<ClientState, 1024> stateMock;
    std::array<uint64_t, 256> frequenciesUseful;

    NetworkProviderInMemory theNetwork;

    std::array<uint8_t, 1024 * 16 * 8 * 32> deltaData;
};

// ///////////////////
// Helpers
// ///////////////////
void TestClientServer1024::SetupDefaultMock(NiceMock<MockIStateManager>& mock)
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
            .WillRepeatedly(Invoke(std::bind(&TestClientServer1024::DeltaCreate, this, std::placeholders::_1, std::placeholders::_2)));

    EXPECT_CALL(mock, PrivateDeltaParse( ::testing::_, ::testing::_))
            .Times(AtLeast(1))
            .WillRepeatedly(Invoke(std::bind(&TestClientServer1024::DeltaParse, this, std::placeholders::_1, std::placeholders::_2)));

    EXPECT_CALL(mock, PrivateDisconnect(::testing::_))
            .Times(AtLeast(1));
}

Delta TestClientServer1024::DeltaCreate(
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

Sequence TestClientServer1024::DeltaParse(
        ClientHandle,
        const Delta& payload)
{
    return payload.to;
}

// ///////////////////
// Tests
// ///////////////////
TEST_F(TestClientServer1024, CreateClient)
{
    Oclock testTime{Clock::now()};

    NetworkManagerServer theServer(theNetwork, stateMock[0].state, [&testTime] () -> Oclock { return testTime; });
    std::vector<std::unique_ptr<NetworkManagerClient>> theClients{};

    // setup
    bool first = true;
    for (auto& state: stateMock)
    {
        if (first)
        {
            first = false;
            continue;
        }

        theClients.emplace_back(new NetworkManagerClient(theNetwork, state.state, [&testTime] () -> Oclock { return testTime; }));
    }

    // simulate gameloop time, client send/receive at 100fps (10ms tick)
    // server send/receieve at 20fps (50ms tick).

    // connect all the clients to the server at once, that'll be fun.
    for (auto& client: theClients)
    {
        client->Connect(stateMock[0].address);
    }

    // Right, run the server for "2 minutes" which is 2*60*1000ms = 180,000ms,
    // split up into 10ms increments.

    // RAM: TODO: Think of the game loops - you have ti wrong somehow...
    int five = 0;
    for (auto i = 0; i < 18000; ++i)
    {
        if (five == 5)
        {
            five = 0;

            theNetwork.RunAs(stateMock[0].address, stateMock[0].random);
            theServer.ProcessIncomming();
            theServer.SendState();
        }
        else
        {
            ++five;
        }

        // do all the clients.
        int clientCount = 1;
        for (auto& client: theClients)
        {
            theNetwork.RunAs(stateMock[clientCount].address, stateMock[clientCount].random);
            client->ProcessIncomming();
            client->SendState();
        }

        testTime += std::chrono::milliseconds(10);
    }


    // start all the state.
    theNetwork.RunAs(stateMock[0].address, stateMock[0].random);

}


}}} // namespace
