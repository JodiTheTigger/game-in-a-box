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

#include <cstdint>
#include <vector>
#include <map>
#include <boost/asio/ip/udp.hpp>

#include "NetworkManagerBase.h"
#include "NetworkPacket.h"

// forward declarations
class NetworkPacket;

class NetworkManagerServer : public NetworkManagerBase
{
public:
    NetworkManagerServer();

    // scratch class
    void ParsePacketFromClient(NetworkPacket& packetData);
    void ParsePacketFromServer(NetworkPacket& packetData);

private:
    static const size_t MinimumPacketSize = 3;
    static const size_t MinimumPacketSizeFromClient = MinimumPacketSize + 2;
    static const size_t OffsetSequence = 0;
    static const size_t OffsetLinkId = 2;
    static const size_t OffsetDataFromServer = 2;
    static const size_t OffsetDataFromClient = 4;
    static const size_t OffsetFragmentId = 2;
    static const size_t OffsetFragmentData = 3;
    static const size_t OffsetCommandKey = 2;
    static const size_t OffsetCommand = 6;
    static const size_t OffsetCommandData = 7;

    static const size_t MtuIp4 = 576;
    static const size_t MtuIp6 = 1280;
    static const size_t MtuEthernetV2 = 1500;
    static const size_t MtuEthernetLlcSnapPppoe = 1492;

    static const size_t SizeMaxMtu = MtuEthernetLlcSnapPppoe;
    static const size_t SizeIpHeaderMinimum = 20;
    static const size_t SizeUdpHeader = 8;

    static const size_t SizeMaxPacketSize = SizeMaxMtu - (SizeIpHeaderMinimum + SizeUdpHeader);

    enum class Command : uint8_t
    {
        Invalid = 0,
        Challenge,
        ChallengeResponse,
        Info,
        InfoResponse,
        Connect,
        // ConnectResponse doesn't exist, I just start sending deltas.

        // Argh, no easy way to tell if a uint8_t is a valid enum
        // Have to make sure they are all sequential from 0, and test
        // against this first. That or a huge convert case statement. Argh!
        // RAM: using a switch statement white list, so I don't need this.
        //CommandCount
    };

    class Challenger
    {
        boost::asio::ip::udp::endpoint endpoint;
        uint32_t key;
        uint8_t challengeCount;
        // timestamp lastPacket;
    };

    class Client : public Challenger
    {
        uint16_t linkId;
    };

    class ClientKey
    {
        boost::asio::ip::address address;
        uint16_t linkId;
    };

    // make this as if it's a custom uint16_t with special comparisons.
    // Also move this class as it's needed by the IStateManager itself
    // So it should be its own class.
    class Sequence
    {
    public:
        uint16_t value;
        //bool fragmented;

        Sequence() : Sequence(0) {};
        Sequence(uint8_t newValue) : value(newValue) {};
        Sequence(const Sequence& other) : value(other.value) {};

        bool operator>(const Sequence& other)
        {
            // RAM: TODO PROPERLY! WRAPAROUND!
            return this->value > other.value;
        }
    };

    // pure functional is good.
    static NetworkPacket PacketDefragment(const std::vector<NetworkPacket>& fragments);
    static std::vector<NetworkPacket>PacketFragment(NetworkPacket &whole);

    void ParseCommand(NetworkPacket& packetData);
    void ParseDelta(NetworkPacket& packetData);

    Sequence SequenceFromPacket(const NetworkPacket& packetData)
    {
        return Sequence((uint16_t) 0x7FFF &
            (((uint16_t) (packetData.data[OffsetSequence]) << 8) ||
            (uint16_t) (packetData.data[OffsetSequence + 0])));
    }

    // RAM: TODO: Is this the best storage for the use?
    std::vector<NetworkPacket> myFragments;
    std::map<ClientKey, Challenger> myClients;
    uint8_t myFragmentCount;
    uint8_t myFragmentTotal;
    Sequence myFragmentSequence;
};

#endif // NETWORKMANAGERSERVER_H
