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

#include "NetworkPacketParser.h"
#include "INetworkManager.h"

// forward delcarations
class NetworkPacket;

class NetworkManagerClient : public NetworkPacketParser, INetworkManager
{
public:
    NetworkManagerClient();

private:
    enum class State
    {
        Idle,
        Challenging,
        Connecting,
        Connected,
        Dying,
    };

    void ParseCommand(NetworkPacket& packetData) override;
    void ParseDelta(NetworkPacket& packetData) override;

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;

    State myState;

    // RAM: Todo, make into it's own type for type checking at compile time.
    uint32_t myKey;
};

#endif // NETWORKMANAGERCLIENT_H
