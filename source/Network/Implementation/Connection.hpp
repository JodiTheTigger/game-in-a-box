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

#include <vector>
#include <string>
#include <chrono>
#include <functional>

#include <boost/optional.hpp>

#include "Units.hpp"
#include "ClientHandle.hpp"
#include "NetworkKey.hpp"
#include "PacketFragmentManager.hpp"

namespace GameInABox { namespace Network {
class IStateManager;
class NetworkPacket;

namespace Implementation {
enum class State;

// Handles the connection handshake and failures between a client and a server.
// Also decodes incomming DeltaPackets (but does not fragment outgoing ones).
class Connection
{
public:
    enum class Mode
    {
        Client,
        Server
    };

    explicit Connection(IStateManager& stateManager)
        : Connection(stateManager, Clock::now) {}

    Connection(
            IStateManager& stateManager,
            TimeFunction timepiece);

    Connection(const Connection&) = default;
    Connection(Connection&&) = default;
    Connection& operator=(const Connection&) = default;
    Connection& operator=(Connection&&) = default;
    ~Connection() = default;

    void Start(Mode mode);
    void Disconnect(std::string failReason);

    // Input, packets to process, output, packets to send.
    std::vector<uint8_t> Process(std::vector<uint8_t> packet);

    // Returns the most recent recieved delta packet
    // It auto drops old out of order packets, so
    // you can assume this is always the most recent
    // packet.
    PacketDelta GetDefragmentedPacket();
    boost::optional<Sequence> LastSequenceAck() const;

    bool IsConnected() const;
    bool HasFailed() const;

    boost::optional<ClientHandle> IdClient() const;
    boost::optional<uint16_t> IdConnection() const;
    NetworkKey Key() const;

    std::string FailReason() const
    {
        return myFailReason;
    }

private:
    static const int HandshakeRetries = 5;
    static const int FloodTrigger = 1 + HandshakeRetries * 2;
    static const uint8_t Version = 1;

    IStateManager*                          myStateManager;
    State                                   myState;
    std::string                             myFailReason;
    NetworkKey                              myKey;
    int                                     myPacketCount;
    OClock                                  myLastTimestamp;
    boost::optional<Sequence>               myLastSequenceAck;
    boost::optional<ClientHandle>           myStateHandle;
    boost::optional<uint16_t>               myIdConnection;
    PacketFragmentManager                   myFragments;
    PacketDelta                             myLastDelta;
    Sequence                                myLastSequenceRecieved;
    TimeFunction                            myTimeNow;

    static constexpr std::chrono::milliseconds HandshakeRetryPeriod()
    {
        return std::chrono::milliseconds{1000};
    }

    void Reset(State resetState);
    void Fail(std::string failReason);
    bool IsValidDeltaTestDisconnectIfNot(const PacketDelta& delta);
    bool Disconnected(const std::vector<uint8_t>& packet);
};

}}} // namespace

#endif // HANDSHAKE_HPP
