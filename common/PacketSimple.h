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

#ifndef PACKETQUERIESKEY_H
#define PACKETQUERIESKEY_H

#include "PacketKey.h"
#include "PacketBuffer.h"
#include "PacketString.h"

class PacketInfo : public PacketKey<PacketCommand::Command::Info>
{
public:
    PacketInfo(uint32_t key) : PacketKey<PacketCommand::Command::Info>(key) {}
    PacketInfo(std::vector<uint8_t> buffer) : PacketKey<PacketCommand::Command::Info>(buffer) {}
};

class PacketInfoResponse : public PacketBuffer<PacketCommand::Command::InfoResponse>
{
public:
    PacketInfoResponse() : PacketBuffer<PacketCommand::Command::InfoResponse>() {}
    PacketInfoResponse(std::vector<uint8_t> buffer) : PacketBuffer<PacketCommand::Command::InfoResponse>(buffer) {}
};

class PacketConnect : public PacketKey<PacketCommand::Command::Connect>
{
public:
    PacketConnect(uint32_t key) : PacketKey<PacketCommand::Command::Connect>(key) {}
    PacketConnect(std::vector<uint8_t> buffer) : PacketKey<PacketCommand::Command::Connect>(buffer) {}
};

class PacketConnectResponse : public PacketBuffer<PacketCommand::Command::ConnectResponse>
{
public:
    PacketConnectResponse() : PacketBuffer<PacketCommand::Command::ConnectResponse>() {}
    PacketConnectResponse(std::vector<uint8_t> buffer) : PacketBuffer<PacketCommand::Command::ConnectResponse>(buffer) {}
};

class PacketDisconnect : public PacketString<PacketCommand::Command::Disconnect>
{
public:
    PacketDisconnect(std::string message) : PacketString<PacketCommand::Command::Disconnect>(message) {}
    PacketDisconnect(std::vector<uint8_t> buffer) : PacketString<PacketCommand::Command::Disconnect>(buffer) {}
};

#endif // PACKETQUERIESKEY_H
