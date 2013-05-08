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
#include <Common/Network/MockINetworkProvider.hpp>
#include <Common/MockIStateManager.hpp>

using namespace std;
using Bytes = std::vector<uint8_t>;

namespace GameInABox { namespace Common { namespace Network {

// Class definition!
class TestNetworkManagerClient : public ::testing::Test
{
public:
    TestNetworkManagerClient()
//        : ipMock1()
//        , ipMock2()
    {

    }

private:
//    MockINetworkProvider ipMock1;
//    MockINetworkProvider ipMock2;

};

TEST_F(TestNetworkManagerClient, TODO)
{
    // TODO!
}

}}} // namespace
