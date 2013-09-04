/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>

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

#ifndef MOCKISTATEMANAGER_HPP
#define MOCKISTATEMANAGER_HPP

#include <Network/IStateManager.hpp>

#include <gmock/gmock.h>

namespace GameInABox { namespace Network {

class MockIStateManager : public IStateManager
{
public:    
    typedef std::array<uint64_t, 256>       FrequencyArray;
    typedef boost::optional<ClientHandle>   OptionalClient;

    MOCK_CONST_METHOD1(PrivateStateInfo, std::vector<uint8_t> (const OptionalClient&));

    MOCK_METHOD2(PrivateConnect, OptionalClient (std::vector<uint8_t>,
                                               std::string&));

    MOCK_METHOD1(PrivateDisconnect, void (ClientHandle));

    MOCK_METHOD2(PrivateCanSend, bool (OptionalClient, std::size_t));
    MOCK_METHOD2(PrivateCanReceive, bool (OptionalClient, std::size_t));

    MOCK_CONST_METHOD2(PrivateDeltaCreate, Delta (
                           ClientHandle,
                           boost::optional<Sequence>));
    MOCK_METHOD2(PrivateDeltaParse, boost::optional<Sequence> (
                    ClientHandle,
                    const Delta&));
};

}} // namespace

#endif // MOCKISTATEMANAGER_HPP
