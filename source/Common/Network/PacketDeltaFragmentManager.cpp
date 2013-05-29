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
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "PacketDeltaFragmentManager.hpp"

using namespace GameInABox::Common::Network;

PacketDeltaFragmentManager::PacketDeltaFragmentManager()
    : myFragments()
    , myComplete()
    , myCurrentSequence()
{
}

std::vector<PacketDelta> PacketDeltaFragmentManager::FragmentPacket(PacketDelta toFragment)
{
    std::vector<PacketDelta> result;
    bool keepGoing(true);
    uint8_t count(0);

    while (keepGoing)
    {
        PacketDelta fragment(toFragment, SizeMaxPacketSize, count++);

        if ((count == 255) || (!fragment.IsValid()))
        {
            keepGoing = false;
        }
        else
        {
            result.push_back(fragment);
        }
    }

    return result;
}

void PacketDeltaFragmentManager::AddPacket(PacketDelta fragmentToAdd)
{
    if (myCurrentSequence < fragmentToAdd.GetSequence())
    {
        myFragments.clear();
    }

    if (fragmentToAdd.IsFragmented())
    {
        if (myFragments.empty())
        {
            myCurrentSequence = fragmentToAdd.GetSequence();
            myFragments.push_back(fragmentToAdd);
        }
        else
        {
            if (myCurrentSequence == fragmentToAdd.GetSequence())
            {
                myFragments.push_back(fragmentToAdd);
            }
        }
    }
    else
    {
        myComplete = fragmentToAdd;
    }
}

PacketDelta PacketDeltaFragmentManager::GetDefragmentedPacket()
{
    PacketDelta defragmented(myFragments);

    if (defragmented.IsValid())
    {
        if (myComplete.IsValid())
        {
            if (defragmented.GetSequence() > myComplete.GetSequence())
            {
                myComplete = defragmented;
            }
            else
            {
                // leave alone.
            }
        }
        else
        {
            myComplete = defragmented;
        }
    }

    return myComplete;
}


