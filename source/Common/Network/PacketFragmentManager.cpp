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

#include "PacketFragmentManager.hpp"

using namespace GameInABox::Common::Network;

PacketFragmentManager::PacketFragmentManager()
    : myFragments()
    , myCurrentSequence()
{
}

std::vector<std::vector<uint8_t>> PacketFragmentManager::FragmentPacket(PacketDelta toFragment)
{
    if (toFragment.data.size() < PacketFragment::MaxTotalPayloadSize(SizeMaxPacketSize))
    {
        if (toFragment.IsValid())
        {
            std::vector<std::vector<uint8_t>> result;

            if (toFragment.data.size() > SizeMaxPacketSize)
            {
                bool keepGoing(true);
                uint8_t count(0);

                while (keepGoing)
                {
                    auto fragment = PacketFragment{
                            toFragment.GetSequence(),
                            toFragment.data,
                            SizeMaxPacketSize,
                            count++};

                    if ((count == 255) || (!fragment.IsValid()))
                    {
                        keepGoing = false;
                    }
                    else
                    {
                        result.emplace_back(move(fragment.data));
                    }
                }
            }
            else
            {
                result.emplace_back(move(toFragment.data));
            }

            return result;
        }
    }

    return {};
}

void PacketFragmentManager::AddPacket(PacketFragment fragment)
{
    if (fragment.IsValid())
    {
        if (myCurrentSequence < fragment.GetSequence())
        {
            myFragments.clear();
        }

        if (myFragments.empty())
        {
            myCurrentSequence = fragment.GetSequence();
            myFragments.push_back(fragment);
        }
        else
        {
            if (myCurrentSequence == fragment.GetSequence())
            {
                myFragments.push_back(fragment);
            }
        }
    }
}

PacketDelta PacketFragmentManager::GetDefragmentedPacket()
{
    if (!myFragments.empty())
    {
        std::vector<PacketFragment*> sorted(myFragments.size());

        // deduplicate
        for (auto& fragment : myFragments)
        {
            // take the last duplicate.
            sorted[fragment.FragmentId()] = &fragment;
        }

        // have we got all the packets?
        if ((sorted.back() != nullptr) && (sorted.front() != nullptr))
        {
            if (sorted.back()->IsLastFragment())
            {
                if (myFragments.size() > sorted.back()->FragmentId())
                {
                    std::vector<uint8_t> buffer;
                    const auto offsetPayload = sorted[0]->OffsetPayload();
                    auto maxFragmentSize = sorted[0]->data.size() - offsetPayload;
                    auto bufferSize = sorted.size() * maxFragmentSize;

                    buffer.reserve(bufferSize);

                    // verify and join.
                    for (auto& fragment : sorted)
                    {
                        if (fragment == nullptr)
                        {
                            // not a complete buffer, quit
                            return {};
                        }

                        buffer.insert(
                            end(buffer),
                            begin(fragment->data) + offsetPayload,
                            end(fragment->data));
                    }

                    // any holes in the list (missing fragments) will be caught in the
                    // iterator due to the fragment pointer being nullptr.
                    return PacketDelta{buffer};
                }
            }
        }
    }

    return {};
}
