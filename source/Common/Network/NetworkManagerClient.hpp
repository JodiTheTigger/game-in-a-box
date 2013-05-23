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

#ifndef USING_PRECOMPILED_HEADERS
#include <vector>
#include <array>
#include <chrono>
#include <utility>
#include <boost/asio/ip/udp.hpp>
#endif

#include "Common/IStateManager.hpp"
#include "Common/Huffman.hpp"
#include "Common/WrappingCounter.hpp"
#include "Common/MotleyUniquePointer.hpp"

#include "INetworkManager.hpp"
#include "PacketDeltaFragmentManager.hpp"
#include "NetworkKey.hpp"

namespace GameInABox { namespace Common { namespace Network {
class INetworkProvider;
class Handshake;

class NetworkManagerClient : public INetworkManager
{
    CLASS_NOCOPY_ASSIGN_MOVE(NetworkManagerClient)

public:
    NetworkManagerClient(
            std::vector<MotleyUniquePointer<INetworkProvider>> networks,
            IStateManager& stateManager);

    void Connect(boost::asio::ip::udp::endpoint serverAddress);
    void Disconnect();

    bool IsConnected() const
    {
        return myState == State::Connected;
    }

    bool HasFailed() const
    {
        return myState == State::FailedConnection;
    }

    std::string FailReason() const
    {
        return myFailReason;
    }

    virtual ~NetworkManagerClient();

private:
    static const uint64_t MaxPacketSizeInBytes{65535};
    static constexpr uint8_t HandshakeRetries{5};

    static constexpr std::chrono::milliseconds HandshakeRetryPeriod()
    {
        return std::chrono::milliseconds{1000};
    }

    enum class State
    {
        Idle,
        Challenging,
        Connecting,
        Connected,
        FailedConnection,
    };

    struct Connection;

    std::vector<Connection> myNetworks;
    Connection* myConnectedNetwork;
    IStateManager& myStateManager;

    State myState;
    NetworkKey myServerKey;
    boost::asio::ip::udp::endpoint myServerAddress;
    boost::optional<ClientHandle> myStateHandle;
    std::string myFailReason;
    uint16_t myClientId;

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
