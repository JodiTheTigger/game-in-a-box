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
#include <memory>
#include <string>
#include <boost/asio/ip/udp.hpp>
#endif

#include "INetworkManager.hpp"

namespace GameInABox { namespace Network
class INetworkProvider;

namespace Implementation {
class NetworkManagerClientGuts;
} // namespace

class NetworkManagerClient : public INetworkManager
{
public:
    NetworkManagerClient(
            INetworkProvider& network,
            IStateManager& stateManager);

    void Connect(boost::asio::ip::udp::endpoint serverAddress);
    void Disconnect();

    bool IsConnected() const;
    bool HasFailed() const;
    std::string FailReason() const;
    virtual ~NetworkManagerClient();

private:
    std::unique_ptr<Implementation::NetworkManagerClientGuts> myGuts;

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;
};

}} // namespace

#endif // NETWORKMANAGERCLIENT_H
