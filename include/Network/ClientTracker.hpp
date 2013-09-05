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

#ifndef CLIENTTRACKER_HPP
#define CLIENTTRACKER_HPP

#include "ClientHandle.hpp"

#include <boost/optional.hpp>
#include <vector>

namespace GameInABox { namespace Network {

// RAM: TODO: Make the value random, track index in the lower bits, upper bits are random.
class ClientTracker
{
public:
    ClientTracker() : ClientTracker(0) {}
    ClientTracker(unsigned maximum);

    // returns an index of the next free slot, if there is one.
    boost::optional<ClientHandle> New();

    // Removes the passed client from the connection list.
    // Ignored if it isn't connected.
    void Delete(ClientHandle toRemove);

    bool IsConnected(ClientHandle toTest);

private:
    enum class State
    {
        Free,
        Used
    };

    std::vector<State> myClients;
    unsigned myNextFree;
    unsigned myMaximumAllowed;

    void FindNextFree();
};

}} // namespace

#endif // CLIENTTRACKER_HPP
