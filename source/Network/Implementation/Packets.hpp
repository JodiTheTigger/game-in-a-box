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

#include "Packet.hpp"
#include "PacketCommand.hpp"
#include "PacketCommandWithKey.hpp"
#include "PacketFragment.hpp"
#include "PacketDelta.hpp"
#include "PacketChallenge.hpp"
#include "PacketChallengeResponse.hpp"

#include <Common/BufferSerialisation.hpp>

#include <boost/optional.hpp>

namespace GameInABox { namespace Network { namespace Implementation {

using PacketInfo            = PacketCommandWithKey<Command::Info>;
using PacketInfoResponse    = PacketCommand<Command::InfoResponse>;
using PacketConnect         = PacketCommandWithKey<Command::Connect>;
using PacketConnectResponse = PacketCommand<Command::ConnectResponse>;
using PacketDisconnect      = PacketCommandWithKey<Command::Disconnect>;

}}} // namespace

#endif // PACKETQUERIESKEY_H
