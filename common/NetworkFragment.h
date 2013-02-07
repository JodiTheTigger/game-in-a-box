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

#ifndef NETWORKFRAGMENT_H
#define NETWORKFRAGMENT_H

#include <cstdint>
#include <memory>
#include <vector>

#include "NetworkPacket.h"

class NetworkFragment
{
public:
    // RAM: Is there a better way than two functions to figure out if linkId is there or not?
    static std::unique_ptr<NetworkFragment> GetFragmentFromDataClient(NetworkPacket& packetData);
    static std::unique_ptr<NetworkFragment> GetFragmentFromDataServer(NetworkPacket& packetData);

    uint16_t sequence;
    uint16_t linkId;
    uint8_t fragmentId;
    
    bool isFragmented;
    
    NetworkPacket packet;   
    size_t dataOffset;  
    
private:
    static const size_t MinimumSizeClient = 2 + 2;
    static const size_t MinimumSizeServer = 2;
           
    // It's assumed this is a valid fragment by the time this
    // is used.
    NetworkFragment(NetworkPacket& packetData);
    
    uint16_t U16Get(size_t offsetInBytes) const;
};

#endif // NETWORKFRAGMENT_H
