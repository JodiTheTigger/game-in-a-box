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
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "GameSimple.hpp"

using namespace GameInABox::Unused;
using namespace GameInABox::Network;

std::array<uint64_t, 256> GameSimple::PrivateGetHuffmanFrequencies() const
{
    // RAM: TODO! BUILD BROKEN! FIX!
    return myFrequencies;
}

std::vector<uint8_t> GameSimple::PrivateStateInfo(const boost::optional<ClientHandle>&) const
{
    // RAM: TODO
    return std::vector<uint8_t>();
}

boost::optional<ClientHandle> GameSimple::PrivateConnect(
        std::vector<uint8_t>,
        std::string&)
{
    // RAM: TODO!
    return boost::optional<ClientHandle>();
}

bool GameSimple::PrivateCanSend(boost::optional<ClientHandle>, std::size_t)
{
    // RAM: TODO
    return true;
}

bool GameSimple::PrivateCanReceive(boost::optional<ClientHandle>, std::size_t)
{
    // RAM: TODO
    return true;
}

void GameSimple::PrivateDisconnect(ClientHandle)
{
    // RAM: TODO
}

bool GameSimple::PrivateIsConnected(ClientHandle) const
{
    // RAM: TODO
    return false;
}


Delta GameSimple::PrivateDeltaCreate(
        ClientHandle,
        boost::optional<Sequence>) const
{
    // RAM: TODO!
    return {};
}

Sequence GameSimple::PrivateDeltaParse(
        ClientHandle,
        const Delta &)
{
    // RAM: TODO!
    return {};
}
