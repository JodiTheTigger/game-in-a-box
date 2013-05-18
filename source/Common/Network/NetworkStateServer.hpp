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

#ifndef NETWORKSTATESERVER_HPP
#define NETWORKSTATESERVER_HPP

#ifndef USING_PRECOMPILED_HEADERS
#include <vector>
#include <string>
#include <chrono>
#include <boost/asio/ip/udp.hpp>
#endif

#include "Common/ClientHandle.hpp"
#include "NetworkKey.hpp"

namespace GameInABox { namespace Common {
class IStateManager;

namespace Network {
class NetworkPacket;
enum class State;

// Handles the connection handshake and failures for a server.
class NetworkStateServer
{
public:
    NetworkStateServer(
            IStateManager& stateManager,
            boost::asio::ip::udp::endpoint address);

    void StartClient();
    void StartServer();
    void Disconnect();

    // Input, packets to process, output, packets to send.
    std::vector<NetworkPacket> Process(NetworkPacket packets);

    bool IsConnected() const;
    bool HasFailed() const;
    ClientHandle Handle() const;

    std::string FailReason() const
    {
        return myFailReason;
    }

private:
    static const int HandshakeRetries = 5;

    IStateManager&                          myStateManager;
    boost::asio::ip::udp::endpoint          myAddress;
    State                                   myState;
    std::string                             myFailReason;
    NetworkKey                              myKey;
    int                                     myPacketCount;
    std::chrono::steady_clock::time_point   myLastTimestamp;
    ClientHandle                            myStateHandle;

    static constexpr std::chrono::milliseconds HandshakeRetryPeriod()
    {
        return std::chrono::milliseconds{1000};
    }

    void Reset(State resetState);
    void Fail(std::string failReason);
    bool Disconnected(const NetworkPacket& packet);

    // To enable a unit test class for the state machine
    // I need a way of controlling the time.
    virtual std::chrono::steady_clock::time_point GetTimeNow() = 0;
};

}}} // namespace

#endif // NETWORKSTATESERVER_HPP
