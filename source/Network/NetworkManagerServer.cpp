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

#ifndef USING_PRECOMPILED_HEADERS
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "Implementation/MakeUnique.hpp"
#include "Implementation/NetworkManagerServerGuts.hpp"
#include "NetworkManagerServer.hpp"

using namespace GameInABox::Network;
using namespace GameInABox::Network::Implementation;

NetworkManagerServer::NetworkManagerServer(
        INetworkProvider& network,
        IStateManager& stateManager)
    : INetworkManager()
    , myGuts(make_unique<NetworkManagerServerGuts>(network, stateManager, Clock::now))
{
}

NetworkManagerServer::~NetworkManagerServer()
{
}

void NetworkManagerServer::PrivateProcessIncomming()
{
    myGuts->ProcessIncomming();
}

void NetworkManagerServer::PrivateSendState()
{
    myGuts->SendState();
}
