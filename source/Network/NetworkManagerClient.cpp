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
#include "Implementation/NetworkManagerClientGuts.hpp"
#include "NetworkManagerClient.hpp"

using namespace GameInABox::Network;
using namespace GameInABox::Network::Implementation;

NetworkManagerClient::NetworkManagerClient(
        INetworkProvider &network,
        IStateManager& stateManager)
    : INetworkManager()
    , myGuts(make_unique<NetworkManagerClientGuts>(network, stateManager, Clock::now))
{
}

NetworkManagerClient::~NetworkManagerClient()
{
}

void NetworkManagerClient::Connect(boost::asio::ip::udp::endpoint serverAddress)
{
    myGuts->Connect(serverAddress);
}

void NetworkManagerClient::Disconnect()
{
    myGuts->Disconnect();
}

bool NetworkManagerClient::IsConnected() const
{
    return myGuts->IsConnected();
}

bool NetworkManagerClient::HasFailed() const
{
    return myGuts->HasFailed();
}

std::string NetworkManagerClient::FailReason() const
{
    return myGuts->FailReason();
}

void NetworkManagerClient::PrivateProcessIncomming()
{    
    myGuts->ProcessIncomming();
}

void NetworkManagerClient::PrivateSendState()
{
    myGuts->SendState();
}
