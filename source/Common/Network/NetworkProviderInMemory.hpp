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

#ifndef NETWORKPROVIDERINMEMORY_HPP
#define NETWORKPROVIDERINMEMORY_HPP


#ifndef USING_PRECOMPILED_HEADERS
#include <memory>
#include <boost/asio.hpp>
#include <unordered_map>
#include <chrono>
#endif

#include "Hash.hpp"
#include "INetworkProvider.hpp"

namespace GameInABox { namespace Common { namespace Network {

class NetworkProviderInMemory final : public INetworkProvider
{
public:
    using Clock = std::chrono::steady_clock;
    using TimeFunction = std::function<Clock::time_point()>;

    NetworkProviderInMemory() : NetworkProviderInMemory(Clock::now) {}
    NetworkProviderInMemory(TimeFunction timepiece);

    void RunAs(boost::asio::ip::udp::endpoint clientAddress) { myCurrentSource = clientAddress; }

private:
    struct TimePacket
    {
        Clock::time_point timeToRelease;
        NetworkPacket data;
    };

    std::unordered_map<boost::asio::ip::udp::endpoint, std::vector<TimePacket>> myAddressToPackets;
    boost::asio::ip::udp::endpoint myCurrentSource;
    bool myNetworkIsDisabled;
    TimeFunction myTimeNow;

    // INetworkProvider Methods.
    std::vector<NetworkPacket> PrivateReceive() override;
    void PrivateSend(std::vector<NetworkPacket> packets) override;
    void PrivateReset() override;
    void PrivateFlush() override;
    void PrivateDisable() override;
    bool PrivateIsDisabled() const override;
};
}}} // namespace

#endif // NETWORKPROVIDERINMEMORY_HPP
