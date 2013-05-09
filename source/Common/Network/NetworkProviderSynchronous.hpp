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

#ifndef NetworkProviderSynchronous_H
#define NetworkProviderSynchronous_H

#include <memory>
#include <boost/asio.hpp>
#include "INetworkProvider.hpp"

namespace GameInABox { namespace Common { namespace Network {
class NetworkPacket;

class NetworkProviderSynchronous final: public INetworkProvider
{
public:
    NetworkProviderSynchronous(boost::asio::ip::udp::endpoint bindAddress);
    NetworkProviderSynchronous()
        : NetworkProviderSynchronous(boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0))
    {
    }

private:
    boost::asio::ip::udp::endpoint myBindAddress;
    boost::asio::io_service myIoService;

    // udp::socket can't be assigned, so I can't use it on the stack.
    std::unique_ptr<boost::asio::ip::udp::socket> mySocket;
    bool myAddressIsIpv4;
    bool myAddressIsIpv6;

    std::vector<NetworkPacket> PrivateReceive() override;
    void PrivateSend(std::vector<NetworkPacket> packets) override;
    void PrivateReset() override;
    void PrivateFlush() override;
    void PrivateDisable() override;
    bool PrivateIsDisabled() const override;
};

}}} // namespace

#endif // NetworkProviderSynchronous_H
