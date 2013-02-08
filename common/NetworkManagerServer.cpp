#include "NetworkManagerServer.h"

using namespace std;

NetworkManagerServer::NetworkManagerServer()
{
}

void NetworkManagerServer::ParsePacket(NetworkPacket &packetData)
{
    if (packetData.data.size() >= NetworkManagerServer::MinimumPacketSizeFromClient)
    {
        if ((packetData.data[0] = 0xFF) || (packetData.data[1] = 0xFF))
        {
            // Command

            // using a switch statement to validate the command means
            // I don't have to worry about casting an invalid value to
            // a CommandType.
            switch (packetData.data[OffsetCommand])
            {
                case (uint8_t) Command::Challenge:
                case (uint8_t) Command::Info:
                case (uint8_t) Command::Connect:
                {
                    ParseCommand(packetData);
                    break;
                }

                default:
                {
                    // Nada
                    break;
                }
            }
        }
        else
        {
            Sequence sequence;

            sequence = SequenceFromPacket(packetData);

            // Reset the fragment buffer?
            if ((sequence > myFragmentSequence) && (!myFragments.empty()))
            {
                myFragmentCount = 0;
                myFragmentTotal = 0;
                myFragments.clear();
            }

            // Fragmented?
            if ((packetData.data[0] & 0x80) != 0)
            {
                // Not fragmented, yippie.
                ParseDelta(packetData);
            }
            else
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

                        notFragmented = NetworkManagerServer::PacketDefragment(myFragments);

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

// Assumed fragments.size() > 0
// TODO: Add assert or warning or something.
NetworkPacket NetworkManagerServer::PacketDefragment(const std::vector<NetworkPacket> &fragments)
{
    NetworkPacket notFragmented;
    notFragmented.data.reserve(MinimumPacketSizeFromClient + (fragments.size() * SizeMaxPacketSize));

    // copy the header.
    notFragmented.address = fragments[0].address;
    notFragmented.data[OffsetSequence + 0] = fragments[0].data[OffsetSequence + 0] & 0x7F;
    notFragmented.data[OffsetSequence + 1] = fragments[0].data[OffsetSequence + 1];
    notFragmented.data[OffsetFragmentLinkId + 0] = fragments[0].data[OffsetFragmentLinkId + 0];
    notFragmented.data[OffsetFragmentLinkId + 1] = fragments[0].data[OffsetFragmentLinkId + 1];

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

void NetworkManagerServer::ParseCommand(NetworkPacket &)
{
    // TODO!
}

void NetworkManagerServer::ParseDelta(NetworkPacket &)
{
    // TODO!
}
