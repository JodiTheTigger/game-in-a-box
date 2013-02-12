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

#include "NetworkPacketParser.h"

#include <vector>
#include "NetworkPacket.h"

using namespace std;


// Challenge packet needs to be larger than the challenge response
// so that the server cannot be used for udp return address amplifation attacks.
const vector<uint8_t> NetworkPacketParser::ChallengePacket =
    { 0xFF, 0xFF, uint8_t(Command::Challenge), 'W', 'h', 'y', ' ', 'h', 'e', 'l', 'l', 'o', ' ', 't', 'h', 'e', 'r', 'e', '.'};


NetworkPacketParser::NetworkPacketParser(PacketEncoding details)
    : myEncodingDetails(details)
    , myFragments{0}
, myFragmentCount(0)
, myFragmentTotal(0)
, myFragmentSequence{0}
{
}

void NetworkPacketParser::ParsePacket(NetworkPacket &packetData)
{
    {
        if (packetData.data.size() >= NetworkPacketParser::MinimumPacketSizeFromClient)
        {
            if ((packetData.data[0] = 0xFF) || (packetData.data[1] = 0xFF))
            {
                ParseCommand(packetData);
            }
            else
            {
                Sequence sequence;

                sequence = SequenceFromPacket(packetData);

                // Reset the fragment buffer?
                if (myEncodingDetails == PacketEncoding::FromServer)
                {
                    if ((sequence > myFragmentSequence) && (!myFragments.empty()))
                    {
                        myFragmentCount = 0;
                        myFragmentTotal = 0;
                        myFragments.clear();
                    }
                }

                // Fragmented?
                if ((packetData.data[0] & 0x80) != 0)
                {
                    // Not fragmented, yippie.
                    ParseDelta(packetData);
                }
                else
                {
                    // servers ignore fragmented packets
                    // RAM: TODO make this a flag class please.
                    if (myEncodingDetails == PacketEncoding::FromServer)
                    {
                        uint8_t fragmentId;

                        fragmentId = packetData.data[OffsetFragmentId];

                        // deal with the fragment.
                        if (fragmentId & 0x80)
                        {
                            fragmentId &= 0x7F;
                            myFragmentTotal = fragmentId + 1;
                        }

                        // watch out for packets out of range
                        if (myFragmentCount < myFragmentTotal)
                        {
                            // don't count duplicates
                            if (myFragments[fragmentId].data.size() == 0)
                            {
                                ++myFragmentCount;
                            }

                            myFragments[fragmentId] = move(packetData);

                            // got them all?
                            if (myFragmentCount == myFragmentTotal)
                            {
                                NetworkPacket notFragmented;

                                notFragmented = PacketDefragment(myFragments);

                                // clean up
                                myFragmentCount = 0;
                                myFragmentTotal = 0;
                                myFragments.clear();

                                // Not fragmented, yippie.
                                ParseDelta(notFragmented);
                            }
                        }
                    }
                }
            }
        }
    }
}

// Assumed fragments.size() > 0
// TODO: Add assert or warning or something.
NetworkPacket NetworkPacketParser::PacketDefragment(const std::vector<NetworkPacket> &fragments)
{
    NetworkPacket notFragmented;
    notFragmented.data.reserve(MinimumPacketSize + (fragments.size() * SizeMaxPacketSize));

    // copy the header, strip the fragmented flag.
    // assumem the vectors size is the size of the largest offset used.
    notFragmented.address = fragments[0].address;
    notFragmented.data[OffsetSequence + 0] = fragments[0].data[OffsetSequence + 0] & 0x7F;
    notFragmented.data[OffsetSequence + 1] = fragments[0].data[OffsetSequence + 1];

    // Hmm, I assume this is going to be fast.
    for (const NetworkPacket& buffer : fragments)
    {
        notFragmented.data.insert(
                    notFragmented.data.end(),
                    buffer.data.begin() + OffsetFragmentData,
                    buffer.data.end());
    }

    return notFragmented;
}


uint32_t NetworkPacketParser::KeyGet(const NetworkPacket& commandPacket)
{
    // high byte first
    return
        uint32_t(commandPacket.data[OffsetCommandKey + 0]) << 24 |
        uint32_t(commandPacket.data[OffsetCommandKey + 1]) << 16 |
        uint32_t(commandPacket.data[OffsetCommandKey + 2]) << 8  |
        uint32_t(commandPacket.data[OffsetCommandKey + 3]);
}

void NetworkPacketParser::KeySet(NetworkPacket& packetToModify, uint32_t key)
{
    packetToModify.data[OffsetCommandKey + 0] = uint8_t(key >> 24);
    packetToModify.data[OffsetCommandKey + 1] = uint8_t(key >> 16);
    packetToModify.data[OffsetCommandKey + 2] = uint8_t(key >> 8);
    packetToModify.data[OffsetCommandKey + 3] = uint8_t(key);
}

std::vector<NetworkPacket> NetworkPacketParser::PacketFragment(NetworkPacket& whole)
{
    vector<NetworkPacket> result;

    if (whole.data.size() < SizeMaxPacketSize)
    {
        result.push_back(move(whole));
    }
    else
    {
        size_t indexSource;
        uint8_t fragmentCount;

        indexSource = OffsetDataFromServer;
        fragmentCount = 0;

        while ((indexSource + SizeMaxPacketSize) < whole.data.size())
        {
            NetworkPacket fragment;
            size_t bytesToCopy;

            bytesToCopy = whole.data.size() - indexSource;
            if (bytesToCopy > (SizeMaxPacketSize - OffsetFragmentData))
            {
                bytesToCopy = SizeMaxPacketSize - OffsetFragmentData;
            }
            else
            {
                // last fragment!
                fragmentCount |= 0x80;
            }

            // copy the header, add fragment bit.
            fragment.address = whole.address;
            fragment.data[OffsetSequence + 0] = whole.data[OffsetSequence + 0] | 0x80;
            fragment.data[OffsetSequence + 1] = whole.data[OffsetSequence + 1];

            fragment.data[OffsetFragmentId] = fragmentCount;

            // copy the data
            fragment.data.insert(
                        fragment.data.end(),
                        whole.data.begin() + indexSource,
                        whole.data.begin() + bytesToCopy);

            fragmentCount++;
            indexSource += bytesToCopy;

            result.push_back(move(fragment));
        }
    }

    return result;
}

Sequence NetworkPacketParser::SequenceFromPacket(const NetworkPacket& packetData)
{
    return Sequence((uint16_t) 0x7FFF &
        (((uint16_t) (packetData.data[OffsetSequence]) << 8) ||
        (uint16_t) (packetData.data[OffsetSequence + 0])));
}
