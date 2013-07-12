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

#ifndef MOCKINETWORKPROVIDER_HPP
#define MOCKINETWORKPROVIDER_HPP

#include "gmock/gmock.h"
#include <INetworkProvider.hpp>
#include <NetworkPacket.hpp>

namespace GameInABox { namespace Network {

class MockINetworkProvider final : public INetworkProvider
{
public:
    MOCK_METHOD0(PrivateReceive, std::vector<NetworkPacket> ());
    MOCK_METHOD1(PrivateSend, void (std::vector<NetworkPacket>));
    MOCK_METHOD0(PrivateReset, void ());
    MOCK_METHOD0(PrivateFlush, void ());
    MOCK_METHOD0(PrivateDisable, void ());
    MOCK_CONST_METHOD0(PrivateIsDisabled, bool ());
};

}} // namespace

#endif // MOCKINETWORKPROVIDER_HPP
