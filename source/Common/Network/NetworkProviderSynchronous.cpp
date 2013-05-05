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
using namespace GameInABox::Common::Logging;

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
        boost::system::error_code error;

        std::size_t available(mySocket->available(error));

        // don't check for packet type, as assume you cannot get ip4 on an ip6 socket.
        // for now loop one packet at a time until empty or max number of packets
        // in one receive. Bah, I wish I didn't need to suffer the cost of array
        // initilisation.
        while   (
                    (available > 0) &&
                    (mySocket->is_open()) &&
                    (!error)
                )
        {
            boost::asio::ip::udp::endpoint addressToUse;
            std::vector<uint8_t> dataToUse(available);

            // blocking (but shouldn't as the data is available).
            mySocket->receive_from(
                boost::asio::buffer(dataToUse),
                addressToUse,
                0,
                error);

            if (!error)
            {
                result.emplace_back(dataToUse, addressToUse);
                available = mySocket->available(error);
            }
        }

        if (error)
        {
            // report and give up.
            Logging::Log(
                Logging::LogLevel::Error,
                "Received Failed: (",
                error.value(),
                ") ",
                error.message().c_str());
        }
    }

    return result;
}

void NetworkProviderSynchronous::PrivateSend(std::vector<NetworkPacket> packets)
{
    if (mySocket->is_open() && !packets.empty())
    {
        boost::system::error_code error;

        for (auto& packet : packets)
        {
            // Suggested (possible) performance inprovements:
            // Batching to sender address:
            // group them to destination address
            // send the groups.

            // test to see if they are the same network type (ip4/ip6)
            if (packet.data.size() > 0)
            {
                if  (
                        (packet.address.address().is_v4() == myAddressIsIpv4) &&
                        (packet.address.address().is_v6() == myAddressIsIpv6)
                     )
                {
                    // Buffer lifetime > send_to lifetime. And since we're
                    // block that means for the life of the function.
                    // If we were using async sends, things would be more
                    // complicated.
                    mySocket->send_to(
                        boost::asio::buffer(packet.data),
                        packet.address,
                        0,
                        error);

                    if (error)
                    {
                        // report and quit.
                        Logging::Log(
                            Logging::LogLevel::Error,
                            "Send Failed: (",
                            error.value(),
                            ") ",
                            error.message().c_str());

                        break;
                    }
                }
            }
        }
    }
}

void NetworkProviderSynchronous::PrivateReset()
{
    PrivateDisable();

    try
    {
        using std::swap;

        auto tempSocket = make_unique<boost::asio::ip::udp::socket>(myIoService, myBindAddress);
        swap(mySocket, tempSocket);
    }
    catch (boost::system::system_error& socketError)
    {
        // is_open() will return false if this didn't work, so just report why to debug.
        Logging::Log(
            Logging::LogLevel::Error,
            "Reset Failed: (",
            socketError.code().value(),
            ") ",
            socketError.what());
    }
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
