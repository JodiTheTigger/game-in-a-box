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
#include <boost/asio/ip/udp.hpp>
#endif

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
            boost::asio::ip::udp::endpoint clientAddress);

    // Input, packets to process, output, packets to send.
    std::vector<NetworkPacket> Process(std::vector<NetworkPacket> packets);
    void Disconnect();

    bool IsConnected() const;
    bool HasFailed() const;

    std::string FailReason() const
    {
        return myFailReason;
    }

private:
    IStateManager&                  myStateManager;
    boost::asio::ip::udp::endpoint  myAddress;
    State                           myState;
    std::string                     myFailReason;
};

}}} // namespace

#endif // NETWORKSTATESERVER_HPP
