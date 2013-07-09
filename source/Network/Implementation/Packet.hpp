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

#ifndef PACKET_H
#define PACKET_H

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <vector>
#include <string>
#endif

#include "Sequence.hpp"

namespace GameInABox { namespace Network { namespace Implementation {

enum class Command : uint8_t
{
    Unrecognised = 0,
    Challenge,
    ChallengeResponse,
    Info,
    InfoResponse,
    Connect,
    ConnectResponse,
    Disconnect,

    LastValidCommand = Disconnect,

    // Special case for invalid ack sequence (DeltaNoAck)
    // Even though we get this, we don't treat it as a valid
    // Command.
    DeltaNoAck = 0x7F
};

class Packet
{
public:
    // ////////////////////////
    // Static
    // ////////////////////////
    static Command GetCommand(const std::vector<uint8_t>& bufferToCheck);
    static Sequence GetSequence(const std::vector<uint8_t>& bufferToCheck);
    static bool IsPacket(const std::vector<uint8_t>& buffer);

    // ////////////////////////
    // Constructors
    // ////////////////////////
    explicit Packet(std::vector<uint8_t> fromBuffer);
    explicit Packet(Command command) : Packet({}, command) {}

    Packet() : Packet(std::vector<uint8_t>()) {}
    Packet(Sequence sequence, Command command);

    Packet(const Packet&) = default;
    Packet(Packet&&) = default;
    Packet& operator=(const Packet&) = default;
    Packet& operator=(Packet&&) = default;
    virtual ~Packet();

    // ////////////////////////
    // Methods
    // ////////////////////////
    // The result is undefined if !IsValid().
    Sequence GetSequence() const { return GetSequence(data); }
    Command GetCommand() const { return GetCommand(data); }

    // ////////////////////////
    // Virtual Methods
    // ////////////////////////
    virtual bool IsValid() const;
    virtual std::size_t OffsetPayload() const { return MinimumPacketSize; }

    // ////////////////////////
    // Members
    // ////////////////////////
    std::vector<uint8_t> data;

protected:
    static const std::size_t MinimumPacketSize = 3;
    static const std::size_t OffsetCommand = 2;

    static const std::size_t OffsetSequence = 0;
    static const std::size_t OffsetIsFragmented = OffsetSequence;
    static const std::size_t OffsetSequenceAck = 2;
    static const std::size_t OffsetIsCommand = OffsetSequenceAck;

    static const uint8_t MaskTopByteIsCommand = 0x80;
    static const uint8_t MaskTopByteIsFragmented = 0x80;

    static const uint16_t MaskSequence = (MaskTopByteIsFragmented << 8) - 1;

    // ////////////////////////
    // Constructors
    // ////////////////////////
    Packet(Sequence sequence, Command command, std::vector<uint8_t> payload);
    Packet(Sequence sequence, Command command, std::string payload);
};

// http://stackoverflow.com/questions/4421706/operator-overloading/4421719#4421719
inline bool operator==(const Packet& lhs, const Packet& rhs){return lhs.data == rhs.data;}
inline bool operator!=(const Packet& lhs, const Packet& rhs){return !operator==(lhs,rhs);}
inline bool operator< (const Packet& lhs, const Packet& rhs){return lhs.data < rhs.data;}
inline bool operator> (const Packet& lhs, const Packet& rhs){return  operator< (rhs,lhs);}
inline bool operator<=(const Packet& lhs, const Packet& rhs){return !operator> (lhs,rhs);}
inline bool operator>=(const Packet& lhs, const Packet& rhs){return !operator< (lhs,rhs);}

std::vector<uint8_t> GetPayloadBuffer(const Packet& packet);
std::string GetPayloadString(const Packet& packet);

}}} // namespace

#endif // PACKET_H
