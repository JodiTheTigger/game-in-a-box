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

#include <vector>
#include <array>
#include <memory>
#include <chrono>
#include <utility>
#include <boost/asio/ip/udp.hpp>
#include "INetworkManager.h"
#include "Common/IStateManager.h"
#include "Common/Huffman.h"
#include "Common/WrappingCounter.h"
#include "PacketDeltaFragmentManager.h"

namespace GameInABox { namespace Common {
class IStateManager;

namespace Network {
class NetworkProvider;

class NetworkManagerClient : public INetworkManager
{
    CLASS_NOCOPY_ASSIGN_MOVE(NetworkManagerClient)

public:
    NetworkManagerClient(
            std::vector<std::unique_ptr<NetworkProvider>> networks,
            IStateManager& stateManager);

    void Connect(boost::asio::ip::udp::endpoint serverAddress);
    void Disconnect();

    virtual ~NetworkManagerClient();

private:
    static constexpr uint8_t HandshakeRetries{5};
    static constexpr std::chrono::milliseconds HandshakeRetryPeriod{1000};

    enum class State
    {
        Idle,
        Challenging,
        Connecting,
        Connected,
        FailedConnection,
    };

    std::vector<std::unique_ptr<NetworkProvider>> myNetworks;
    IStateManager& myStateManager;
    NetworkProvider* myConnectedNetwork;

    State myState;
    uint32_t myServerKey;
    std::array<uint8_t,4> myServerKeyAsABuffer;
    boost::asio::ip::udp::endpoint myServerAddress;
    IStateManager::ClientHandle* myStateHandle;
    std::string myFailReason;
    uint16_t myClientId;

    PacketDeltaFragmentManager myDeltaHelper;
    Huffman myCompressor;

    Sequence myLastSequenceProcessed;
    Sequence myLastSequenceAcked;

    uint8_t myPacketSentCount;
    std::chrono::steady_clock::time_point myLastPacketSent;

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;

    void Fail(std::string failReason);
    void DeltaReceive();
    void DeltaSend();
};

}}} // namespace

#endif // NETWORKMANAGERCLIENT_H
