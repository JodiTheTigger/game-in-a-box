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

#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#ifndef USING_PRECOMPILED_HEADERS
#include <string>
#include <boost/asio/ip/udp.hpp>
#endif

#include "PacketDelta.hpp"
#include "Handshake.hpp"
#include "PacketDeltaFragmentManager.hpp"

namespace GameInABox { namespace Common {
class IStateManager;

namespace Network {
class INetworkProvider;

class Connection
{
public:
    enum class Mode
    {
        Solo,
        Shared
    };

    Connection(
        INetworkProvider& network,
        IStateManager& state,
        Mode mode);

    // Rule of 5.
    Connection(const Connection&) = default;
    Connection(Connection&&) = default;
    Connection& operator=(const Connection&) = default;
    Connection& operator=(Connection&&) = default;
    ~Connection() = default;

    void Connect(boost::asio::ip::udp::endpoint address, Handshake::Mode mode);
    void Disconnect(std::string failReason);

    bool IsConnected();
    bool HasFailed();
    std::string FailReason();

    void Process();
    void DeltaSend(PacketDelta toSend);

    // Returns an invalid packet if there is nothing to get.
    PacketDelta DeltaReceive();

private:
    INetworkProvider& myNetwork;
    IStateManager& myState;
    Mode myMode;
    Handshake myHandshake;
    std::string myFailReason;
    boost::asio::ip::udp::endpoint myAddress;
    PacketDeltaFragmentManager myFragments;

    void Fail(std::string failReason);
};

}}} // namespace

#endif // CONNECTION_HPP
