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

#include "NetworkManagerClient.h"
#include "NetworkPacket.h"

#include "NetworkProvider.h"
#include "IStateManager.h"

using namespace std;

NetworkManagerClient::NetworkManagerClient(
        std::vector<std::unique_ptr<NetworkProvider>> networks,
        std::weak_ptr<IStateManager> stateManager)
    : NetworkPacketParser(PacketEncoding::FromServer)
    , myStateManager(stateManager)
    , myState(State::Idle)
{
    for (auto& network : networks)
    {
        if (network)
        {
            myNetworks.push_back(move(network));
        }
    }
}

void Connect(boost::asio::ip::udp::endpoint)
{

}

bool NetworkManagerClient::IsConnected()
{
    return myState==State::Connected;
}

bool NetworkManagerClient::IsTimedOut()
{
    return myState==State::Timeout;
}

void NetworkManagerClient::ParseCommand(NetworkPacket &packetData)
{
    // The only command we care about is the ChallengeResponse.
    if (myState == State::Challenging)
    {
        Command command;

        command = Command(packetData.data[OffsetCommand]);

        if (command == Command::ChallengeResponse)
        {
            myKey = KeyGet(packetData);
            myState = State::Connecting;

            // RAM: TODO: Send connect packet!
        }
    }
}

void NetworkManagerClient::ParseDelta(NetworkPacket &)
{
    if (myState == State::Connecting)
    {
        myState = State::Connected;
    }

    // Ignore if not connected
    if (myState == State::Connected)
    {
        // RAM: TODO: Parse the packet please.
        // TODO: Decrypt
        // TODO: Expand
        // TODO: Pass to gamestate.
    }
}

void NetworkManagerClient::PrivateProcessIncomming()
{
    if ((myState != State::Idle) && (myState != State::Timeout))
    {
        // TODO!
    }
}

void NetworkManagerClient::PrivateSendState()
{
    if ((myState != State::Idle) && (myState != State::Timeout))
    {
        // TODO!
    }
}
