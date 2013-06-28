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
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "Common/IStateManager.hpp"
#include "NetworkPacket.hpp"
#include "Packets.hpp"
#include "Connection.hpp"

using namespace std::chrono;
using Timepoint = std::chrono::steady_clock::time_point;

namespace GameInABox { namespace Common { namespace Network {

enum class State
{
    Idle,
    FailedConnection,
    Disconnecting,

    // Server States
    Listening,
    ConnectedToClient,

    // Client States
    Challenging,
    Connecting,
    ConnectedToServer
};

Connection::Connection(
        IStateManager& stateManager,
        TimeFunction timepiece)
    : myStateManager(&stateManager)
    , myState(State::Idle)
    , myFailReason("")
    , myKey(GetNetworkKeyNil())
    , myPacketCount(0)
    , myLastTimestamp(Timepoint::min())
    , myLastSequenceAck()
    , myStateHandle()
    , myIdConnection()
    , myFragments()
    , myLastDelta()
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
    myFailReason = {};

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
        myStateManager->Disconnect(*myStateHandle);
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

        case State::ConnectedToServer:
        {
            auto fragment = PacketFragment{packet};
            auto delta = PacketDelta{};

            if (fragment.IsValid())
            {
                if (fragment.GetSequence() > myLastSequenceRecieved)
                {
                    myFragments.AddPacket(fragment);

                    // test to see if the fragment is finished.
                    delta = myFragments.GetDefragmentedPacket();
                }
            }
            else
            {
                delta = PacketDelta{packet};
            }            

            if (IsValidDeltaTestDisconnectIfNot(delta))
            {
                myLastDelta = delta;
                myLastSequenceRecieved = delta.GetSequence();
                myLastSequenceAck = delta.GetSequenceAck();
            }

            break;
        }            

        case State::ConnectedToClient:
        {
            auto delta = PacketDelta{packet};

            if (IsValidDeltaTestDisconnectIfNot(delta))
            {
                auto connection = Network::IdConnection(delta);

                // can't end here unless connection is valid.
                if (connection = myIdConnection)
                {
                    myLastDelta = delta;
                    myLastSequenceRecieved = delta.GetSequence();
                    myLastSequenceAck = delta.GetSequenceAck();
                }
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
                        auto handle = myStateManager->Connect(GetPayloadBuffer(connection), failReason);

                        if (!handle)
                        {
                            result = std::move(PacketDisconnect(myKey, failReason).data);
                            Fail(failReason);
                        }
                        else
                        {
                            myStateHandle = handle;
                            Reset(State::ConnectedToServer);
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
            if (myPacketCount < FloodTrigger)
            {
                switch (command)
                {
                    case Command::Challenge:
                    {
                        auto challenge = PacketChallenge{packet};

                        if (challenge.IsValid())
                        {
                            result = std::move(PacketChallengeResponse(Version, myKey).data);
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
                                auto infoData = myStateManager->StateInfo({});
                                auto packet = PacketInfoResponse::WithBuffer(infoData);

                                // NOTE: Packet will be UDP fragmented if too big.
                                // But I'm not going to do anything about that.
                                result = std::move(packet.data);
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
                                    auto handle = myStateManager->Connect(GetPayloadBuffer(connect), failMessage);

                                    if (handle)
                                    {
                                        myStateHandle = handle;
                                    }
                                    else
                                    {
                                        result = std::move(PacketDisconnect(myKey, failMessage).data);
                                        Fail(failMessage);
                                    }
                                }

                                if (myStateHandle)
                                {
                                    auto infoData = myStateManager->StateInfo(myStateHandle);
                                    auto response = PacketConnectResponse::WithBuffer(infoData);

                                    result = std::move(response.data);
                                    myLastTimestamp = myTimeNow();
                                    ++myPacketCount;
                                }
                            }
                        }
                        else
                        {
                            result = std::move(PacketDisconnect(myKey, "Invalid Key.").data);
                            ++myPacketCount;
                        }

                        break;
                    }

                    case Command::Unrecognised:
                    {
                        // Don't bother if we're not connected-but-waiting-for-delta
                        if (myStateHandle)
                        {
                            auto delta = PacketDelta{packet};

                            if (delta.IsValid())
                            {
                                auto idConnection = Network::IdConnection(delta);

                                if (idConnection)
                                {
                                    myIdConnection = idConnection;

                                    Reset(State::ConnectedToClient);
                                    myLastDelta = delta;
                                    myLastSequenceRecieved = delta.GetSequence();
                                    myLastSequenceAck = delta.GetSequenceAck();
                                }
                            }
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
        case State::FailedConnection:
        case State::ConnectedToServer:
        case State::ConnectedToClient:
        {
            // Nothing, ignore everything
            break;
        }

        case State::Disconnecting:
        {
            result = std::move(PacketDisconnect(myKey, myFailReason).data);
            Fail(myFailReason);
            break;
        }

        // ///////////////////
        // Client
        // ///////////////////
        case State::Challenging:
        case State::Connecting:
        {
            if (myPacketCount > HandshakeRetries)
            {
                if (myState == State::Challenging)
                {
                    Fail("Timeout: Challenging.");
                }
                else
                {
                    Fail("Timeout: Connecting.");
                }
            }
            else
            {
                auto sinceLastPacket = myTimeNow() - myLastTimestamp;

                if (duration_cast<milliseconds>(sinceLastPacket) > HandshakeRetryPeriod())
                {
                    if (myState == State::Challenging)
                    {
                        result = std::move(PacketChallenge().data);
                    }
                    else
                    {
                        auto info = myStateManager->StateInfo({});

                        // may be so big it UDP fragments, not my problem.
                        result = std::move(PacketConnect(myKey, info).data);
                    }

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
                Fail("Timeout: Client.");
            }

            break;
        }
    }

    return result;
}

PacketDelta Connection::GetDefragmentedPacket()
{
    // I want the current delta to be invalidated every time this is called.
    return std::move(myLastDelta);
}

boost::optional<Sequence> Connection::LastSequenceAck() const
{
    return myLastSequenceAck;
}

bool Connection::IsConnected() const
{
    return ((myState == State::ConnectedToServer) || (myState == State::ConnectedToClient));
}

bool Connection::HasFailed() const
{
    return myState == State::FailedConnection;
}

boost::optional<ClientHandle> Connection::IdClient() const
{
    return myStateHandle;
}

boost::optional<uint16_t> Connection::IdConnection() const
{
    return myIdConnection;
}

NetworkKey Connection::Key() const
{
    return myKey;
}

void Connection::Reset(State resetState)
{
    myState         = resetState;
    myPacketCount   = 0;

    // If I use Timepoint::min(), I end up with -ve durations.
    // This breaks the timestamping mechanism. bah.
    myLastTimestamp = myTimeNow() - (HandshakeRetryPeriod() * 2);
}

void Connection::Fail(std::string failReason)
{
    myState = State::FailedConnection;
    myFailReason = failReason;
}


bool Connection::IsValidDeltaTestDisconnectIfNot(const PacketDelta& delta)
{
    if (delta.IsValid())
    {
        if (delta.GetSequence() > myLastSequenceRecieved)
        {
            return true;
        }
    }
    else
    {
        // check to see if we disconnect.
        Disconnected(delta.data);
    }

    return false;
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
                Fail(GetPayloadString(disconnect));
                return true;
            }
        }
    }

    return false;
}

}}} // namespace
