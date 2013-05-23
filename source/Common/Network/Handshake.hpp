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

#ifndef HANDSHAKE_HPP
#define HANDSHAKE_HPP

#ifndef USING_PRECOMPILED_HEADERS
#include <vector>
#include <string>
#include <chrono>
#include <boost/optional.hpp>
#endif

#include "Common/ClientHandle.hpp"
#include "NetworkKey.hpp"

namespace GameInABox { namespace Common {
class IStateManager;

namespace Network {
class NetworkPacket;
enum class State;

// Handles the connection handshake and failures between a client and a server.
class Handshake
{
public:
    enum class Mode
    {
        Client,
        Server
    };

    Handshake(IStateManager& stateManager);

    Handshake(const Handshake&) = default;
    Handshake(Handshake&&) = default;
    Handshake& operator=(const Handshake&) = default;
    Handshake& operator=(Handshake&&) = default;
    ~Handshake() = default;

    void Start(Mode mode);
    void Disconnect();

    // Input, packets to process, output, packets to send.
    std::vector<uint8_t> Process(std::vector<uint8_t> packet);

    bool IsConnected() const;
    bool HasFailed() const;
    boost::optional<ClientHandle> Handle() const;

    std::string FailReason() const
    {
        return myFailReason;
    }

private:
    static const int HandshakeRetries = 5;
    static const int FloodTrigger = 1 + HandshakeRetries * 2;
    static const uint8_t Version = 1;

    IStateManager&                          myStateManager;
    State                                   myState;
    std::string                             myFailReason;
    NetworkKey                              myKey;
    int                                     myPacketCount;
    std::chrono::steady_clock::time_point   myLastTimestamp;
    boost::optional<ClientHandle>           myStateHandle;

    static constexpr std::chrono::milliseconds HandshakeRetryPeriod()
    {
        return std::chrono::milliseconds{1000};
    }

    void Reset(State resetState);
    void Fail(std::string failReason);
    bool Disconnected(const std::vector<uint8_t>& packet);

    // To enable a unit test class for the state machine
    // I need a way of controlling the time.
    // RAM: Use a function passed into the constructor instead
    // (dependency injection).
    virtual std::chrono::steady_clock::time_point GetTimeNow();
};

}}} // namespace

#endif // HANDSHAKE_HPP
