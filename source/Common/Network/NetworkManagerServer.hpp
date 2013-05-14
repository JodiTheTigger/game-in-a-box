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

#include "INetworkManager.hpp"
#include "NetworkPacket.hpp"

namespace GameInABox { namespace Common { namespace Network {

// forward declarations
class NetworkPacket;

class NetworkManagerServer : public INetworkManager
{
public:
    NetworkManagerServer();
    virtual ~NetworkManagerServer();

private:
    // RAM: Note: For now not tracking
    // client state via an enum, instead
    // tracking state by putting various
    // clients into various arrays
    // (myChallengers, myClients, myDisconnected)

    class Challenger
    {
    public:
        boost::asio::ip::udp::endpoint endpoint{};
        uint32_t key{};
        uint8_t challengeCount{};
        // timestamp lastPacket;

        virtual ~Challenger() {};
    };

    class Client : public Challenger
    {
        uint16_t linkId{};
    };

    class ClientKey
    {
        boost::asio::ip::address address{};
        uint16_t linkId{};
    };

    void PrivateProcessIncomming() override;
    void PrivateSendState() override;

    // RAM: TODO: Is this the best storage for the use?
    std::map<ClientKey, Challenger> myClients;
};

}}} // namespace

#endif // NETWORKMANAGERSERVER_H
