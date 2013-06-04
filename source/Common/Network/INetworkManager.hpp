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

#ifndef INETWORKMANAGER_H
#define INETWORKMANAGER_H

#ifndef USING_PRECOMPILED_HEADERS
#include <memory>
#include <vector>
#endif

#include "Common/No.hpp"

namespace GameInABox { namespace Common { namespace Network {

class INetworkManager : NoCopyMoveNorAssign
{
public:
    // Processes all waiting packets.
    // -Does connection challenges
    // -Does connectionless packets (challenge, info)
    // -Generates new clients (tells the state manager)
    // -Processes incoming client state
    void ProcessIncomming();

    // Sends the network state to all connected clients
    void SendState();

protected:
    INetworkManager() {};
    ~INetworkManager();

private:
    virtual void PrivateProcessIncomming() = 0;
    virtual void PrivateSendState() = 0;
};

}}} // namespace

#endif // INETWORKMANAGER_H
