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

#include "PacketCommand.h"

using namespace std;
using namespace GameInABox::Common::Network;

PacketCommand::PacketCommand(std::vector<uint8_t> fromBuffer)
    : myBuffer(fromBuffer)
{
}

PacketCommand::PacketCommand(Command command)
    : PacketCommand({0xFF, 0xFF, uint8_t(command)})
{
}

PacketCommand::~PacketCommand()
{
}


PacketCommand::Command PacketCommand::GetCommand(const std::vector<uint8_t>& bufferToCheck)
{
    if (bufferToCheck.size() >= MinimumPacketSize)
    {
        if ((bufferToCheck[0] == 0xFF) && (bufferToCheck[1] == 0xFF))
        {
            return static_cast<Command>(bufferToCheck[OffsetCommand]);
        }
    }

    return Command::Unrecognised;
}

bool PacketCommand::IsValid() const
{
    return (GetCommand() != Command::Unrecognised);
}
