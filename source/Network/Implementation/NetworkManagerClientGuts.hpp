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

#ifndef NETWORKMANAGERCLIENTGUTS_H
#define NETWORKMANAGERCLIENTGUTS_H

#include "IStateManager.hpp"
#include "Huffman.hpp"
#include "WrappingCounter.hpp"
#include "INetworkManager.hpp"
#include "PacketFragmentManager.hpp"
#include "NetworkKey.hpp"
#include "Connection.hpp"

#include <boost/asio/ip/udp.hpp>

#include <vector>
#include <array>
#include <utility>

namespace GameInABox { namespace Network {
class INetworkProvider;

namespace Implementation {

class NetworkManagerClientGuts : public INetworkManager
{
public:
    NetworkManagerClientGuts(
            INetworkProvider& network,
            IStateManager& stateManager,
            TimeFunction timepiece);

    NetworkManagerClientGuts(
            INetworkProvider& network,
            IStateManager& stateManager);

    void Connect(boost::asio::ip::udp::endpoint serverAddress);
    void Disconnect();

    bool IsConnected() const
    {
        return myConnection.IsConnected();
    }

    bool HasFailed() const
    {
        return myConnection.HasFailed();
    }

    std::string FailReason() const
    {
        return myConnection.FailReason();
    }

    virtual ~NetworkManagerClientGuts();

private:
    static const uint64_t MaxPacketSizeInBytes{65535};
    static constexpr uint8_t HandshakeRetries{5};

    static constexpr std::chrono::milliseconds HandshakeRetryPeriod()
    {
        return std::chrono::milliseconds{1000};
    }

    INetworkProvider& myNetwork;
    Connection myConnection;
    IStateManager& myStateManager;

    boost::asio::ip::udp::endpoint myServerAddress;
    uint16_t myClientId;

    Huffman myCompressor;

    Sequence myLastSequenceProcessed;

    uint8_t myPacketSentCount;

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;

    void Fail(std::string failReason);
    void DeltaReceive();
    void DeltaSend();
};

}}} // namespace

#endif // NETWORKMANAGERCLIENTGUTS_H
