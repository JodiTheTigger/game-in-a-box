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

#ifndef USING_PRECOMPILED_HEADERS
#include <vector>
#endif

#include "INetworkProvider.hpp"
#include "Connection.hpp"

namespace GameInABox { namespace Common { namespace Network {

Connection::Connection(
    INetworkProvider& network,
    IStateManager& state,
    Mode mode)
    : myNetwork(network)
    , myState(state)
    , myMode(mode)
    , myHandshake(state)
    , myFailReason()
    , myAddress()
    , myFragments()
{

}

void Connection::Connect(boost::asio::ip::udp::endpoint, Handshake::Mode mode)
{
    if (myMode == Mode::Solo)
    {
        myNetwork.Reset();
    }

    myFailReason = {};
    myHandshake.Start(mode);
}

void Connection::Disconnect(std::string failReason)
{
    Fail(failReason);
}

bool Connection::IsConnected()
{
    return myHandshake.IsConnected() && (!myNetwork.IsDisabled());
}

bool Connection::HasFailed()
{
    return myHandshake.HasFailed();
}

std::string Connection::FailReason()
{
    return myFailReason;
}

void Connection::Process()
{
    if (!myNetwork.IsDisabled())
    {
        auto packets = myNetwork.Receive();

        for (auto& packet : packets)
        {
            if (packet.address == myAddress)
            {
                // RAM: TODO: Best way to do this?
                /*
                if (PacketDelta::IsPacketDelta(packet.data))
                {
                    myFragments.AddPacket(PacketDelta(packet.data));
                }
                else
                {
                    // Pass though state incase we get disconnect packets.
                    auto response = myHandshake.Process(packet.data);
                    myNetwork.Send({{response, myAddress}});

                    if (myHandshake.HasFailed())
                    {
                        Fail(myHandshake.FailReason());
                        break;
                    }
                }*/
            }
        }
    }
}

void Connection::DeltaSend(PacketDelta toSend)
{
    if (!myNetwork.IsDisabled())
    {
        std::vector<NetworkPacket> packets;

        auto fragments = PacketDeltaFragmentManager::FragmentPacket(toSend);

        // Pretty sure I could use a lambda here to remove the loop.
        for (auto& fragment : fragments)
        {
            packets.emplace_back(fragment.TakeBuffer(), myAddress);
        }

        // send
        myNetwork.Send(packets);
    }
}

// Returns an invalid packet if there is nothing to get.
PacketDelta Connection::DeltaReceive()
{
    if (!myNetwork.IsDisabled())
    {
        if (myHandshake.IsConnected())
        {
            // RAM: TODO!
        }
    }

    return {};
}

void Connection::Fail(std::string failReason)
{
    myFailReason = failReason;

    if (!myHandshake.HasFailed())
    {
        myHandshake.Disconnect(failReason);
    }

    auto lastPacket = myHandshake.Process({});

    if (!myNetwork.IsDisabled())
    {
        myNetwork.Send({{lastPacket, myAddress}});

        if (myMode == Mode::Solo)
        {
            myNetwork.Flush();
            myNetwork.Disable();
        }
    }
}

}}} // namespace
