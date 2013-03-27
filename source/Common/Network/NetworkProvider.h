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

// forward references
class NetworkPacket;

// RAM: TODO! Turn this into an interface please! support ip4 ip6 and whatever else.
// RAM: Note: will silently ignore packets that are the wrong type (ipv4 send for ip6 provider)
// RAM: TODO! Add network metrics (bytes per second, send, recieve, rolling stats)
// RAM: TODO! Add Network throttling here
// RAM: TODO! Add Network throttling per destination as well (so global throttle, and per connection)
class NetworkProvider
{
public:
    // Non blocking, can return empty array.
    std::vector<NetworkPacket> Receive();
    
    // Adds the packets to the send queue. consumes the data (move()), non-blocking.
    void Send(std::vector<NetworkPacket> packets);

    // Disable and release all sockets and resources, clear all buffers. Reopens the socket.
    void Reset();

    // Blocks until all packets have been sent.
    void Flush();

    // Disable and release all sockets and resources, clear all buffers.
    // Sends are silently ignored, and Recieve will return nothing.
    void Disable();
    bool IsDisabled();
};

#endif // NETWORKPROVIDER_H