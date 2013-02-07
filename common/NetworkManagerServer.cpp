#include "NetworkManagerServer.h"

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
            if (sequence > myFragmentSequence)
            {
                myFragmentCount = 0;
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
            }
        }
    }
}

void NetworkManagerServer::ParseCommand(NetworkPacket &)
{
    // TODO!
}

void NetworkManagerServer::ParseDelta(NetworkPacket &)
{
    // TODO!
}
