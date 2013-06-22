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
#include <Common/MockIStateManager.hpp>
#include <Common/MotleyUniquePointer.hpp>

// NOTE:
// This only tests the class for valid state.
// I'm not testing the process, or the data the mocks
// are getting or setting.
// I'm not testing timeouts due to the general rule of:
// units test shouldn't take more than 250ms to complete.

using namespace std;
using Bytes = std::vector<uint8_t>;

using ::testing::Return;
using ::testing::AtLeast;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestClientServer : public ::testing::Test
{
public:
    TestClientServer()
        : ipMock1(make_unique_motley<INetworkProvider, MockINetworkProvider>())
        , ipMock2(make_unique_motley<INetworkProvider, MockINetworkProvider>())
        , stateMock()
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

    MotleyUniquePointer<INetworkProvider> ipMock1;
    MotleyUniquePointer<INetworkProvider> ipMock2;
    MockIStateManager stateMock;
    std::array<uint64_t, 256> frequencies;
    std::array<uint64_t, 256> frequenciesUseful;
};

TEST_F(TestClientServer, CreateClientNoNet)
{
    // right, what do we expect?
    EXPECT_CALL(stateMock, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequencies));

    NetworkManagerClient toTest({}, stateMock);

    EXPECT_FALSE(toTest.IsConnected());
    EXPECT_FALSE(toTest.HasFailed());
    EXPECT_EQ("", toTest.FailReason());
}

TEST_F(TestClientServer, CreateClientTwoNet)
{
    std::vector<MotleyUniquePointer<INetworkProvider>> networks;

    networks.push_back(std::move(ipMock1));
    networks.push_back(std::move(ipMock2));

    // right, what do we expect?
    EXPECT_CALL(stateMock, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequencies));

    NetworkManagerClient(std::move(networks), stateMock);
}

TEST_F(TestClientServer, CreateServer)
{
    // right, what do we expect?
    EXPECT_CALL(stateMock, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequencies));

    NetworkManagerServer(std::move(ipMock1), stateMock);
}

}}} // namespace


