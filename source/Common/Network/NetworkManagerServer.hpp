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

#ifndef NETWORKMANAGERSERVER_H
#define NETWORKMANAGERSERVER_H

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <vector>
#include <map>
#include <boost/asio/ip/udp.hpp>
#endif

#include "Common/MotleyUniquePointer.hpp"

#include "INetworkManager.hpp"
//#include "NetworkPacket.hpp"

namespace GameInABox { namespace Common {
class IStateManager;

namespace Network {
class INetworkProvider;
class NetworkPacket;

class NetworkManagerServer : public INetworkManager
{
    CLASS_NOCOPY_ASSIGN_MOVE(NetworkManagerServer)

public:
    NetworkManagerServer(
            std::vector<MotleyUniquePointer<INetworkProvider>> networks,
            IStateManager& stateManager);

    virtual ~NetworkManagerServer();

private:
    std::vector<MotleyUniquePointer<INetworkProvider>> myNetworks;
    IStateManager& myStateManager;

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;
};

}}} // namespace

#endif // NETWORKMANAGERSERVER_H
