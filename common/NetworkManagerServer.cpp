#include "NetworkManagerServer.h"

using namespace std;

NetworkManagerServer::NetworkManagerServer()
    : NetworkPacketParser(PacketEncoding::FromClient)
{
}

void NetworkManagerServer::ParseCommand(NetworkPacket &)
{
    // TODO!
}

void NetworkManagerServer::ParseDelta(NetworkPacket &)
{
    // TODO!
}
