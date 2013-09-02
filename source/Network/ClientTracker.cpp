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

#include "ClientTracker.hpp"

namespace GameInABox { namespace Network {

ClientTracker::ClientTracker(unsigned maximum)
    : myClients()
    , myNextFree(0)
    , myMaximumAllowed(maximum)
{
    myClients.reserve(myMaximumAllowed);
}

boost::optional<ClientHandle> ClientTracker::New()
{
    if (myNextFree < myMaximumAllowed)
    {
        ClientHandle result{myNextFree};

        FindNextFree();

        return {result};
    }
    else
    {

        // nup, no connection allowed.
        return {};
    }
}

void ClientTracker::Delete(ClientHandle toRemove)
{
    myClients[toRemove.value] = State::Free;

    if (toRemove.value < myNextFree)
    {
        myNextFree = toRemove.value;
    }
    else
    {
        FindNextFree();
    }
}

bool ClientTracker::IsConnected(ClientHandle toTest)
{
    if (toTest.value < myMaximumAllowed)
    {
        return myClients[toTest.value] == State::Used;
    }

    return false;
}

void ClientTracker::FindNextFree()
{
    while ((myNextFree < myMaximumAllowed) && (myClients[myNextFree] != State::Free))
    {
        ++myNextFree;
    }
}

}} // namespace
