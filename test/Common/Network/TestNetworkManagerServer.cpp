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

#include <Common/Network/NetworkManagerServer.hpp>
#include <Common/Network/MockINetworkProvider.hpp>
#include <Common/MockIStateManager.hpp>
#include <Common/MotleyUniquePointer.hpp>

using namespace std;
using Bytes = std::vector<uint8_t>;

using ::testing::Return;
using ::testing::AtLeast;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestNetworkManagerServer : public ::testing::Test
{
public:
    TestNetworkManagerServer()
        : ipMock1(make_unique_motley<INetworkProvider, MockINetworkProvider>())
        , stateMock()
    {
        // all equally probable.
        frequencies.fill(1);
    }

    MotleyUniquePointer<INetworkProvider> ipMock1;
    MockIStateManager stateMock;
    std::array<uint64_t, 256> frequencies;
};


TEST_F(TestNetworkManagerServer, CreateNet)
{
    // right, what do we expect?
    EXPECT_CALL(stateMock, PrivateGetHuffmanFrequencies())
            .Times(AtLeast(1))
            .WillRepeatedly(Return(frequencies));

    NetworkManagerServer(std::move(ipMock1), stateMock);
}



}}} // namespace