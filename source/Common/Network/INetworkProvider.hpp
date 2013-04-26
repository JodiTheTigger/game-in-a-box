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

#ifndef NETWORKPROVIDER_H
#define NETWORKPROVIDER_H

#include <vector>
#include "Common/BuildMacros.hpp"

namespace GameInABox { namespace Common { namespace Network {

// forward references
class NetworkPacket;

// RAM: TODO! Add network metrics (bytes per second, send, recieve, rolling stats)
// RAM: TODO! Add Network throttling here
// RAM: TODO! Add Network throttling per destination as well (so global throttle, and per connection)
class INetworkProvider
{
    CLASS_NOCOPY_ASSIGN_MOVE(INetworkProvider);

public:
    // I need to delete classes via the interface, so make the destructor virtual.
    // (Passing ownership via unique_ptr).
    // NOTE: I would prefer all NVI to have protected desctructors so delete cannot
    // be called by the user of the interface. But then doing so means I cannot use
    // unique_ptr<interface> to transfer ownership either. Bah.
    INetworkProvider() = default;
    virtual ~INetworkProvider() = default;

    // Non blocking, can return empty array.
    std::vector<NetworkPacket> Receive();
    
    // Adds the packets to the send queue. consumes the data (move()), non-blocking.
    // Will ignore packets that are the wrong type (ipv4 send for ip6 provider).
    void Send(std::vector<NetworkPacket> packets);

    // Disable and release all sockets and resources, clear all buffers. Reopens the socket.
    void Reset();

    // Blocks until all packets have been sent.
    void Flush();

    // Disable and release all sockets and resources, clear all buffers.
    // Sends are silently ignored, and Recieve will return nothing.
    void Disable();
    bool IsDisabled() const;

private:
    virtual std::vector<NetworkPacket> PrivateReceive() = 0;
    virtual void PrivateSend(std::vector<NetworkPacket> packets) = 0;
    virtual void PrivateReset() = 0;
    virtual void PrivateFlush() = 0;
    virtual void PrivateDisable() = 0;
    virtual bool PrivateIsDisabled() const= 0;
};

}}} // namespace

#endif // NETWORKPROVIDER_H
