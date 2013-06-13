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

#ifndef PACKETDELTAFRAGENTMANAGER_H
#define PACKETDELTAFRAGENTMANAGER_H

#ifndef USING_PRECOMPILED_HEADERS
#include <vector>
#endif

#include "PacketDelta.hpp"
#include "PacketFragment.hpp"

namespace GameInABox { namespace Common { namespace Network {

// Fragments and defragments packets of a fixed size
// of PacketDeltaFragmentManager::SizeMaxPacketSize.
// If we need to generalise this, turn it into a template
// with the packet size a template parameter so that the static
// function uses the generalised fragment size as well.
class PacketDeltaFragmentManager
{
public:
    // Returned array might not contain fragmented packets
    // if the passed PacketDelta is small enough.
    static std::vector<PacketDelta> FragmentPacket(PacketDelta toFragment);
    static std::vector<PacketFragment> FragmentPacket2(PacketDelta toFragment);

    PacketDeltaFragmentManager();

    // Takes both fragmented and non-fragmented packets
    // returns the most recent complete packet.
    void AddPacket(PacketDelta fragmentToAdd);
    void AddPacket(PacketFragment packet);
    PacketDelta GetDefragmentedPacket();
    PacketDelta GetDefragmentedPacket2();

private:    
    static const std::size_t MtuIp4 = 576;
    static const std::size_t MtuIp6 = 1280;
    static const std::size_t MtuEthernetV2 = 1500;
    static const std::size_t MtuEthernetLlcSnapPppoe = 1492;

    static const std::size_t SizeMaxMtu = MtuEthernetLlcSnapPppoe;
    static const std::size_t SizeIpHeaderMinimum = 20;
    static const std::size_t SizeUdpHeader = 8;

    static const std::size_t SizeMaxPacketSize = SizeMaxMtu - (SizeIpHeaderMinimum + SizeUdpHeader);

    std::vector<PacketDelta> myFragments;
    std::vector<PacketFragment> myFragments2;
    PacketDelta myComplete;
    Sequence myCurrentSequence;
};

}}} // namespace

#endif // PACKETDELTAFRAGENTMANAGER_H
