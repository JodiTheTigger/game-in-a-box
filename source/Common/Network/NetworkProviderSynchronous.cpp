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

#include "Common/MakeUnique.hpp"
#include "Common/Logging/Logging.hpp"

#include "NetworkProviderSynchronous.hpp"
#include "NetworkPacket.hpp"

using boost::asio::ip::udp;
using namespace GameInABox::Common::Network;

NetworkProviderSynchronous::NetworkProviderSynchronous(boost::asio::ip::udp::endpoint bindAddress)
    : INetworkProvider()
    , myBindAddress(bindAddress)
    , myIoService()
    , mySocket(make_unique<boost::asio::ip::udp::socket>(myIoService, myBindAddress))
    , myAddressIsIpv4(myBindAddress.address().is_v4())
    , myAddressIsIpv6(myBindAddress.address().is_v6())
{
}

NetworkProviderSynchronous::~NetworkProviderSynchronous() noexcept(true)
{
}

std::vector<NetworkPacket> NetworkProviderSynchronous::PrivateReceive()
{
    std::vector<NetworkPacket> result;

    if (mySocket->is_open())
    {
        std::size_t available(mySocket->available());

        // don't check for packet type, as assume you cannot get ip4 on an ip6 socket.
        // for now loop one packet at a time until empty or max number of packets
        // in one receive. Bah, I wish I didn't need to suffer the cost of array
        // initilisation.
        while ((available > 0) && (mySocket->is_open()))
        {
            boost::asio::ip::udp::endpoint addressToUse;
            std::vector<uint8_t> dataToUse(available);

            // blocking (but shouldn't as the data is available).
            // RAM: TODO! Deal with the errors!
            mySocket->receive_from(
                boost::asio::buffer(dataToUse),
                addressToUse);

            result.emplace_back(dataToUse, addressToUse);
            available = mySocket->available();
        }
    }

    return result;
}

void NetworkProviderSynchronous::PrivateSend(std::vector<NetworkPacket> packets)
{
    if (mySocket->is_open() && !packets.empty())
    {
        // RAM: who owns the packet buffers? figure out lifetime!
        for (auto& packet : packets)
        {
            // Suggested (possible) performance inprovements:
            // Batching to sender address:
            // group them to destination address
            // send the groups.
            // check for errors, fail gracfully.

            // test to see if they are the same network type (ip4/ip6)
            if (packet.data.size() > 0)
            {
                if  (
                        (packet.address.address().is_v4() == myAddressIsIpv4) &&
                        (packet.address.address().is_v6() == myAddressIsIpv6)
                     )
                {
                    // RAM: TODO make sure the address is not IPADDRESS_ANY!
                    // RAM: TODO: deal with errors!
                    mySocket->send_to(
                        boost::asio::buffer(packet.data),
                        packet.address);
                }
            }
        }
    }
}

void NetworkProviderSynchronous::PrivateReset()
{
    PrivateDisable();

    using std::swap;

    auto tempSocket = make_unique<boost::asio::ip::udp::socket>(myIoService, myBindAddress);
    swap(mySocket, tempSocket);
}

void NetworkProviderSynchronous::PrivateFlush()
{
    // Sends and Receives are blocking, nothing to flush.
}

void NetworkProviderSynchronous::PrivateDisable()
{
    mySocket->close();
}

bool NetworkProviderSynchronous::PrivateIsDisabled() const
{
    return !(mySocket->is_open());
}
