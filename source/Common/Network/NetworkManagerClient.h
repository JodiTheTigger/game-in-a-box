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

#ifndef NETWORKMANAGERCLIENT_H
#define NETWORKMANAGERCLIENT_H

#include <memory>
#include <vector>
#include <boost/asio/ip/udp.hpp>
#include <chrono>
#include <string>

#include "NetworkPacketParser.h"
#include "INetworkManager.h"
#include "Common/IStateManager.h"

namespace GameInABox { namespace Common { namespace Network {

// forward delcarations
class NetworkPacket;
class NetworkProvider;

class NetworkManagerClient : public NetworkPacketParser, INetworkManager
{
    CLASS_NOCOPY_ASSIGN_MOVE(NetworkManagerClient)

public:
    NetworkManagerClient(
            std::vector<std::unique_ptr<NetworkProvider>> networks,
            std::weak_ptr<IStateManager> stateManager);

    virtual ~NetworkManagerClient() {}

    void Connect(
            boost::asio::ip::udp::endpoint serverAddress,
            std::vector<uint8_t> connectData);

    bool IsConnected();
    bool IsFailed();

private:
    static const uint8_t HandshakeRetries = 5;
    static constexpr std::chrono::milliseconds HandshakeRetryPeriod{1000};

    enum class State
    {
        Idle,
        Challenging,
        Connecting,
        Connected,
        Failed
    };    

    std::weak_ptr<IStateManager> myStateManager;
    std::vector<std::unique_ptr<NetworkProvider>> myNetworks;

    State myState;

    // RAM: Todo, make into it's own type for type checking at compile time.
    uint32_t myKey;
    boost::asio::ip::udp::endpoint myServerAddress;
    size_t myServerInterface;
    std::chrono::steady_clock::time_point myLastPacketSent;
    std::vector<uint8_t> myConnectData; // RAM: TODO: turn to pointer, Free once connected, as nolonger needed.
    IStateManager::ClientHandle* myClientId;

    // RAM: TODO: replace type to Count so I don't need to use hungarian notation.
    uint8_t myPacketSentCount;

    void SendChallengePacket();
    void SendConnectPacket();
    void SendDisconnectPacket(std::string reason);

    void ParseCommand(NetworkPacket& packetData) override;
    void ParseDelta(NetworkPacket& packetData) override;

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;
};

}}} // namespace

#endif // NETWORKMANAGERCLIENT_H
