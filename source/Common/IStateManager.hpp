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

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <vector>
#include <string>
#include <array>
#include <boost/optional.hpp>
#endif

#include "No.hpp"
#include "Sequence.hpp"
#include "ClientHandle.hpp"

namespace GameInABox { namespace Common {

// Forward declarations
class BitStream;
class BitStreamReadOnly;

struct Delta
{
    Sequence base{};
    Sequence to{};
    std::vector<uint8_t> deltaPayload{};

    Delta(Sequence newBase, Sequence newTo, std::vector<uint8_t> newPayload)
        : base(newBase)
        , to(newTo)
        , deltaPayload(newPayload)
    {
    }

    Delta() = default;
    Delta(const Delta&) = default;
};

class IStateManager : NoCopyMoveNorAssign
{
public:
    std::array<uint64_t, 256> GetHuffmanFrequencies() const;

    // Returns general state info if the client handle is invalid
    // Otherwise info specific to the client (which is passed into Connect()).
    std::vector<uint8_t> StateInfo(const boost::optional<ClientHandle>& client) const;

    boost::optional<ClientHandle> Connect(std::vector<uint8_t> connectData, std::string& failReason);
    void Disconnect(ClientHandle toDisconnect);
    bool IsConnected(ClientHandle client) const;

    // called each time a packet is to be sent to the network, to be used by the state
    // manager to control throttling. Return true if can send, false otherwise.
    // If !client it is for handshaking.
    bool CanPacketSend(boost::optional<ClientHandle> client, std::size_t bytes);

    // Delta compression aggression should be inferred by the packet distance,
    // thus how well the packet compresses, therefore how big the packet should get.
    // A large packet delta should require more aggressive delta creation to get
    // smaller packet sizes.
    Delta DeltaCreate(
            ClientHandle client,
            Sequence lastAcked) const;

    // Returns the sequence of the payload, or if it couldn't parse the delta
    // the sequence it wants the next delta's base to be.
    Sequence DeltaParse(
            ClientHandle client,
            const Delta& payload);

protected:
    // Don't allow deletion or creation of the interface.
    IStateManager()  = default;
    ~IStateManager() = default;
    
private:
    virtual std::array<uint64_t, 256> PrivateGetHuffmanFrequencies() const = 0;

    virtual std::vector<uint8_t> PrivateStateInfo(const boost::optional<ClientHandle>& client) const = 0;

    virtual boost::optional<ClientHandle> PrivateConnect(
            std::vector<uint8_t> connectData,
            std::string& failReason) = 0;

    virtual void PrivateDisconnect(ClientHandle playerToDisconnect) = 0;    
    virtual bool PrivateIsConnected(ClientHandle client) const = 0;

    virtual bool PrivateCanPacketSend(boost::optional<ClientHandle> client, std::size_t bytes) = 0;    

    virtual Delta PrivateDeltaCreate(
            ClientHandle client,
            Sequence lastAcked) const = 0;

    virtual Sequence PrivateDeltaParse(
            ClientHandle client,
            const Delta& payload) = 0;
};

}} // namespace

#endif // ISTATEMANAGER_H
