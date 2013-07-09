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

#include <memory>

#include "INetworkManager.hpp"

namespace GameInABox { namespace Network {
class IStateManager;
class INetworkProvider;

namespace Implementation {
class NetworkManagerServerGuts;
}

class NetworkManagerServer : public INetworkManager
{
public:
    NetworkManagerServer(
            INetworkProvider& network,
            IStateManager& stateManager);

    virtual ~NetworkManagerServer();

private:
    std::unique_ptr<Implementation::NetworkManagerServerGuts> myGuts;

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;
};

}} // namespace

#endif // NETWORKMANAGERSERVER_H
