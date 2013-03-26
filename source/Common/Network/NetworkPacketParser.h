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

#ifndef NETWORKMANAGERBASE_H
#define NETWORKMANAGERBASE_H

#include <cstdint>
#include <vector>

#include "WrappingCounter.h"

// forward delcarations
class NetworkPacket;

// RAM: TODO: Rename this to PacketParseHelper or something.
// As it's got nothing to do with network management, but
// everything to do with parsing packets. Oh, and it also
// contains state an info shared by both clients and servers
// such as commands.
class NetworkPacketParser
{    
    void ParsePacket(NetworkPacket& packetData);

protected:
    // RAM: Remove: Most are now contained within PacketCommand and
    // It's derivitives.
    static const std::size_t MinimumPacketSize = 3;
    static const std::size_t MinimumPacketSizeFromClient = MinimumPacketSize + 2;
    static const std::size_t OffsetSequence = 0;
    static const std::size_t OffsetLinkId = 2;
    static const std::size_t OffsetDataFromServer = 2;
    static const std::size_t OffsetDataFromClient = 4;
    static const std::size_t OffsetFragmentId = 2;
    static const std::size_t OffsetFragmentData = 3;
    // RAM: TODO: move the key to after the command
    // and make it optional depending on the command.
    static const std::size_t OffsetCommandKey = 2;
    static const std::size_t OffsetCommand = 6;
    static const std::size_t OffsetCommandData = 7;
    static const std::size_t OffsetConnectData = OffsetCommandData;

    static const std::size_t MtuIp4 = 576;
    static const std::size_t MtuIp6 = 1280;
    static const std::size_t MtuEthernetV2 = 1500;
    static const std::size_t MtuEthernetLlcSnapPppoe = 1492;

    static const std::size_t SizeMaxMtu = MtuEthernetLlcSnapPppoe;
    static const std::size_t SizeIpHeaderMinimum = 20;
    static const std::size_t SizeUdpHeader = 8;

    static const std::size_t SizeMaxPacketSize = SizeMaxMtu - (SizeIpHeaderMinimum + SizeUdpHeader);

    static const std::vector<uint8_t> ChallengePacket;

    // RAM: TODO: Remove, it's moved to PacketCommand.
    enum class Command : uint8_t
    {
        Invalid = 0,
        Challenge,
        ChallengeResponse,
        Info,
        InfoResponse,
        Connect,
        ConnectResponse,
        Disconnect
    };

    enum class PacketEncoding
    {
        FromClient = 0,
        FromServer = 1
    };

    NetworkPacketParser(PacketEncoding details);
    ~NetworkPacketParser();

    // pure functional is good.
    static NetworkPacket PacketDefragment(const std::vector<NetworkPacket>& fragments);
    static std::vector<NetworkPacket>PacketFragment(NetworkPacket &whole);
    static uint32_t KeyGet(const NetworkPacket& commandPacket);
    static void KeySet(NetworkPacket& packetToModify, uint32_t key);

    static std::vector<uint8_t> ConnectDataGet(const NetworkPacket& fromPacket);


private:
    PacketEncoding myEncodingDetails;

    std::vector<NetworkPacket> myFragments;
    uint8_t myFragmentCount;
    uint8_t myFragmentTotal;
    WrappingCounter<uint16_t> myFragmentSequence;

    virtual void ParseCommand(NetworkPacket& packetData) = 0;
    virtual void ParseDelta(NetworkPacket& packetData) = 0;

    WrappingCounter<uint16_t> SequenceFromPacket(const NetworkPacket& packetData);
};

#endif // NETWORKMANAGERBASE_H
