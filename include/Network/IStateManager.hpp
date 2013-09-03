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

#ifndef ISTATEMANAGER_H
#define ISTATEMANAGER_H

#include "Sequence.hpp"
#include "ClientHandle.hpp"
#include "Delta.hpp"

#include <Common/No.hpp>

#include <boost/optional.hpp>

#include <cstdint>
#include <vector>
#include <string>
#include <array>

namespace GameInABox { namespace Network {

// All methods are assumed to be multithreading and reentrance UNSAFE, unless told otherwise.
class IStateManager : GameInABox::Common::NoCopyMoveNorAssign
{
public:
    // Returns general state info if the client handle is invalid
    // Otherwise info specific to the client (which is passed into Connect()).
    std::vector<uint8_t> StateInfo(const boost::optional<ClientHandle>& client) const;

    boost::optional<ClientHandle> Connect(std::vector<uint8_t> connectData, std::string& failReason);
    void Disconnect(ClientHandle toDisconnect);
    bool IsConnected(ClientHandle client) const;

    // Called for each datagram received or to be sent, to be used by the state
    // manager for metrics and to control throttling. Return true to process the datagram further, false otherwise.
    // If !client it is for handshaking.
    bool CanReceive(boost::optional<ClientHandle> client, std::size_t bytes);
    bool CanSend(boost::optional<ClientHandle> client, std::size_t bytes);

    // Delta compression aggression should be inferred by the packet distance,
    // thus how well the packet compresses, therefore how big the packet should get.
    // A large packet delta should require more aggressive delta creation to get
    // smaller packet sizes.
    Delta DeltaCreate(
            ClientHandle client,
            boost::optional<Sequence> lastAcked) const;

    // Returns the ack sequence of the payload.
    boost::optional<Sequence> DeltaParse(
            ClientHandle client,
            const Delta& payload);

protected:
    // Don't allow deletion or creation of the interface.
    IStateManager()  = default;
    ~IStateManager() = default;
    
private:
    virtual std::vector<uint8_t> PrivateStateInfo(const boost::optional<ClientHandle>& client) const = 0;

    virtual boost::optional<ClientHandle> PrivateConnect(
            std::vector<uint8_t> connectData,
            std::string& failReason) = 0;

    virtual void PrivateDisconnect(ClientHandle playerToDisconnect) = 0;    
    virtual bool PrivateIsConnected(ClientHandle client) const = 0;

    virtual bool PrivateCanReceive(boost::optional<ClientHandle> client, std::size_t bytes) = 0;
    virtual bool PrivateCanSend(boost::optional<ClientHandle> client, std::size_t bytes) = 0;

    virtual Delta PrivateDeltaCreate(
            ClientHandle client,
            boost::optional<Sequence> lastAcked) const = 0;

    virtual boost::optional<Sequence> PrivateDeltaParse(
            ClientHandle client,
            const Delta& payload) = 0;
};

}} // namespace

#endif // ISTATEMANAGER_H
