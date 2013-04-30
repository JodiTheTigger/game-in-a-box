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

#include "NetworkProviderSynchronousIp4.hpp"
#include "NetworkPacket.hpp"

using boost::asio::ip::udp;
using namespace GameInABox::Common::Network;

NetworkProviderSynchronousIp4::NetworkProviderSynchronousIp4(boost::asio::ip::udp::endpoint)
    : INetworkProvider()
    , myIoService()
{
}

NetworkProviderSynchronousIp4::~NetworkProviderSynchronousIp4() noexcept(true)
{
}

std::vector<NetworkPacket> NetworkProviderSynchronousIp4::PrivateReceive()
{
    return std::vector<NetworkPacket>();
}

void NetworkProviderSynchronousIp4::PrivateSend(std::vector<NetworkPacket>)
{

}

void NetworkProviderSynchronousIp4::PrivateReset()
{

}

void NetworkProviderSynchronousIp4::PrivateFlush()
{

}

void NetworkProviderSynchronousIp4::PrivateDisable()
{

}

bool NetworkProviderSynchronousIp4::PrivateIsDisabled() const
{
    return true;
}
