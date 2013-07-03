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
using ::testing::NiceMock;

using Clock = std::chrono::steady_clock;
using Oclock = Clock::time_point;

namespace GameInABox { namespace Common { namespace Network {

Delta DeltaCreate1024(
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

Sequence DeltaParse1024(
        ClientHandle,
        const Delta& payload)
{
    return payload.to;
}

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
    {
    }

    virtual void SetUpOverride()
    {
        // Setup all random settings
        for (auto& state : stateMock)
        {
            state.latency = std::normal_distribution<float>(random0To1(randomEngine) * 2000.0f, random0To1(randomEngine) * 400.0f);
            state.random.latency = std::bind(state.latency, randomEngine);
            state.random.random0To1 = std::bind(random0To1, randomEngine);
            state.random.latencyMinimum = NetworkProviderInMemory::Milliseconds(static_cast<NetworkProviderInMemory::MillisecondStorageType>(1 + random0To1(randomEngine) * 500));
            state.random.packetLossChancePerPacket0to1 = random0To1(randomEngine);
            state.random.packetLossChanceBurst0to1 = random0To1(randomEngine);
            state.random.packetOutOfOrderChance0to1 = random0To1(randomEngine);
        }

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
    //void SetupDefaultMock(StrictMock<MockIStateManager>& mock);
    std::default_random_engine randomEngine;
    std::uniform_real_distribution<float> random0To1;
    std::array<ClientState, 1024> stateMock;
    std::array<uint64_t, 256> frequenciesUseful;

    NetworkProviderInMemory theNetwork;
};

}}} // namespace
