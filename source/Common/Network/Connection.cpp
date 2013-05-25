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

#ifndef USING_PRECOMPILED_HEADERS
#include <chrono>
#include <tuple>
#include <sstream>
#endif

#include "Common/IStateManager.hpp"
#include "NetworkPacket.hpp"
#include "PacketTypes.hpp"
#include "PacketChallenge.hpp"
#include "PacketChallengeResponse.hpp"
#include "PacketDelta.hpp"

#include "Connection.hpp"

using namespace std::chrono;
using Timepoint = std::chrono::steady_clock::time_point;

namespace GameInABox { namespace Common { namespace Network {

enum class State
{
    Idle,
    FailedConnection,
    Connected,
    Disconnecting,

    // Server States
    Listening,

    // Client States
    Challenging,
    Connecting,
};

Connection::Connection(
        IStateManager& stateManager,
        TimeFunction timepiece)
    : myStateManager(stateManager)
    , myState(State::Idle)
    , myFailReason("")
    , myKey(GetNetworkKeyNil())
    , myPacketCount(0)
    , myLastTimestamp(Timepoint::min())
    , myStateHandle()
    , myFragments()
    , myTimeNow(timepiece)
{
    if (!myTimeNow)
    {
        myTimeNow = std::chrono::steady_clock::now;
    }
}

void Connection::Start(Mode mode)
{
    myStateHandle.reset();
    myFragments = {};

    if (mode == Mode::Server)
    {
        myKey = GetNetworkKeyRandom();
        Reset(State::Listening);
    }
    else
    {
        myKey = GetNetworkKeyNil();
        Reset(State::Challenging);
    }
}

void Connection::Disconnect(std::string failReason)
{
    if (myStateHandle)
    {
        myStateManager.Disconnect(*myStateHandle);
    }

    if (failReason.empty())
    {
        failReason = std::string{"Normal Disconnect."};
    }

    myFailReason = failReason;
    Reset(State::Disconnecting);
}

std::vector<uint8_t> Connection::Process(std::vector<uint8_t> packet)
{
    std::vector<uint8_t> result{};

    // NOTE:
    // pure things:
    // State, NetworkPacket, packetCount, timeLastPacket, timeCurrent, failReason
    // impure things:
    // IStateManager (Disconnect, Connect, StateInfo), Fail()
    // tried to have this all in a pure function, but calling IStateManager connect
    // and disconnect was impure, wrong place to try.

    // Done in two parts, parse the packets then generate packets.
    // Parse first as that can change the state we're in.

    // Finally, it's assumed that packet address checks have been done
    // Before we get here, so I won't be checking.

    // ///////////////////
    // Parse
    // ///////////////////
    switch (myState)
    {
        case State::Idle:
        case State::FailedConnection:
        case State::Disconnecting:
        {
            // Nothing, ignore everything
            break;
        }

        case State::Connected:
        {
            if (PacketDelta::IsPacketDelta(packet))
            {
                myFragments.AddPacket(PacketDelta{packet});
            }
            else
            {
                // check to see if we disconnect.
                Disconnected(packet);
            }

            break;
        }

        case State::Challenging:
        {
            if (!Disconnected(packet))
            {
                if (Packet::GetCommand(packet) == Command::ChallengeResponse)
                {
                    auto response = PacketChallengeResponse{packet};

                    if (response.IsValid())
                    {
                        if (response.Version() == Version)
                        {
                            myKey = response.Key();
                            Reset(State::Connecting);
                        }
                        else
                        {
                            std::ostringstream theFail{};

                            theFail
                                << "Network Protocol Mistmatch, expecting version: "
                                << Version
                                << " recieved: "
                                << response.Version()
                                << ".";

                            Fail(theFail.str());
                        }
                    }
                }
            }

            break;
        }

        case State::Connecting:
        {
            if (!Disconnected(packet))
            {
                if (Packet::GetCommand(packet) == Command::ConnectResponse)
                {
                    auto connection = PacketConnectResponse{packet};

                    if (connection.IsValid())
                    {
                        std::string failReason{};
                        auto handle = myStateManager.Connect(connection.GetBuffer(), failReason);

                        if (!handle)
                        {
                            result = PacketDisconnect(myKey, failReason).TakeBuffer();
                            Fail(failReason);
                        }
                        else
                        {
                            myStateHandle = handle;
                            Reset(State::Connected);
                        }
                    }
                }
            }

            break;
        }

        // ///////////////////
        // Server
        // ///////////////////
        case State::Listening:
        {
            auto command = Packet::GetCommand(packet);

            // Deal with floods by not sending the response.
            if (myPacketCount > FloodTrigger)
            {
                switch (command)
                {
                    case Command::Challenge:
                    {
                        auto challenge = PacketChallenge{packet};

                        if (challenge.IsValid())
                        {
                            result = PacketChallengeResponse(Version, myKey).TakeBuffer();
                            myLastTimestamp = myTimeNow();
                            ++myPacketCount;
                        }

                        break;
                    }

                    case Command::Info:
                    {
                        auto info = PacketConnect{packet};

                        if (info.IsValid())
                        {
                            if (info.Key() == myKey)
                            {
                                auto infoData = myStateManager.StateInfo({});

                                // Bah can't just create the packet with std::vector as it'll treat it
                                // as the entire packet buffer, not just the payload.
                                PacketInfoResponse packet{};
                                packet.Append(infoData);

                                // NOTE: Packet will be UDP fragmented if too big.
                                // But I'm not going to do anything about that.
                                result = packet.TakeBuffer();
                                myLastTimestamp = myTimeNow();
                                ++myPacketCount;
                            }
                        }

                        break;
                    }

                    case Command::Connect:
                    {
                        auto connect = PacketConnect{packet};

                        if (connect.IsValid())
                        {
                            if (connect.Key() == myKey)
                            {
                                // register the client with the game state
                                // and if successful send the packet.
                                if (!myStateHandle)
                                {
                                    std::string failMessage{};
                                    auto handle = myStateManager.Connect(connect.GetBuffer(), failMessage);

                                    if (handle)
                                    {
                                        myStateHandle = handle;
                                    }
                                    else
                                    {
                                        result = PacketDisconnect(myKey, failMessage).TakeBuffer();
                                        Fail(failMessage);
                                    }
                                }

                                if (myStateHandle)
                                {
                                    auto infoData = myStateManager.StateInfo(myStateHandle);
                                    PacketConnectResponse response{};
                                    response.Append(infoData);

                                    result = response.TakeBuffer();
                                    myLastTimestamp = myTimeNow();
                                    ++myPacketCount;
                                }
                            }
                        }
                        else
                        {
                            result = PacketDisconnect(myKey, "Invalid Key.").TakeBuffer();
                            ++myPacketCount;
                        }

                        break;
                    }

                    case Command::Unrecognised:
                    {
                        // If we get deltas, that means we're connected.
                        if (PacketDelta::IsPacketDelta(packet))
                        {
                            Reset(State::Connected);
                            myFragments.AddPacket(PacketDelta{packet});
                        }

                        break;
                    }

                    default:
                    {
                        // nothing.
                        break;
                    }
                }
            }

            break;
        }
    }

    // ///////////////////
    // Generate
    // ///////////////////

    switch (myState)
    {
        // ///////////////////
        // Common
        // ///////////////////
        case State::Idle:
        {
            // Nothing, ignore everything
            break;
        }

        case State::FailedConnection:
        {
            // Again, Nothing, ignore everything.
            break;
        }

        case State::Connected:
        {
            // check to see if we disconnect.
            Disconnected(packet);

            break;
        }

        case State::Disconnecting:
        {
            result = PacketDisconnect(myKey, myFailReason).TakeBuffer();
            Fail(myFailReason);
            break;
        }

        // ///////////////////
        // Client
        // ///////////////////
        case State::Challenging:
        {
            if (myPacketCount > HandshakeRetries)
            {
                Fail("Timeout: Challenging.");
            }
            else
            {
                auto sinceLastPacket = myTimeNow() - myLastTimestamp;

                if (duration_cast<milliseconds>(sinceLastPacket) > HandshakeRetryPeriod())
                {
                    result = PacketChallenge().TakeBuffer();
                    myLastTimestamp = myTimeNow();
                    ++myPacketCount;
                }
            }

            break;
        }

        case State::Connecting:
        {
            // RAM: duplicate code! remove!

            if (myPacketCount > HandshakeRetries)
            {
                Fail("Timeout: Connecting.");
            }
            else
            {
                auto sinceLastPacket = myTimeNow() - myLastTimestamp;

                if (duration_cast<milliseconds>(sinceLastPacket) > HandshakeRetryPeriod())
                {
                    auto info = myStateManager.StateInfo({});

                    // may be so big it UDP fragments, not my problem.
                    result = PacketConnect(myKey, info).TakeBuffer();
                    myLastTimestamp = myTimeNow();
                    ++myPacketCount;
                }
            }

            break;
        }

        // ///////////////////
        // Server
        // ///////////////////
        case State::Listening:
        {
            auto sinceLastPacket = myTimeNow() - myLastTimestamp;

            // int{HandshakeRetries} because for some reason te compiler needs
            // the address of HandshakeRetries if I just do * HandshakeRetries.
            // Which means I'll need to put a defintion in this file. Argh.
            // Yes, I know that this comment is bigger than the defintion I would
            // have needed.
            if (duration_cast<milliseconds>(sinceLastPacket) > (HandshakeRetryPeriod() * int{HandshakeRetries}))
            {
                // Meh, timeout.
                Fail("Timeout: Connecting to server.");
            }

            break;
        }
    }

    return result;
}

PacketDelta Connection::GetDefragmentedPacket()
{
    return myFragments.GetDefragmentedPacket();
}

bool Connection::IsConnected() const
{
    return myState == State::Connected;
}

bool Connection::HasFailed() const
{
    return myState == State::FailedConnection;
}

boost::optional<ClientHandle> Connection::Handle() const
{
    return myStateHandle;
}

void Connection::Reset(State resetState)
{
    myState         = resetState;
    myFailReason    = "";
    myPacketCount   = 0;
    myLastTimestamp = Timepoint::min();
}

void Connection::Fail(std::string failReason)
{
    myState = State::FailedConnection;
    myFailReason = failReason;
}

bool Connection::Disconnected(const std::vector<uint8_t> &packet)
{
    if (Packet::GetCommand(packet) == Command::Disconnect)
    {
        auto disconnect = PacketDisconnect{packet};

        if (disconnect.IsValid())
        {
            if (disconnect.Key() == myKey)
            {
                Fail(disconnect.Message());
                return true;
            }
        }
    }

    return false;
}

}}} // namespace
