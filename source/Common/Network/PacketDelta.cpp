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

#include "PacketDelta.hpp"
#include "BufferSerialisation.hpp"

using namespace GameInABox::Common;
using namespace GameInABox::Common::Network;

bool PacketDelta::IsPacketDelta(const std::vector<uint8_t>& buffer)
{
    bool result(false);

    if (!buffer.empty())
    {
        if (buffer.size() >= MinimumPacketSizeCommon)
        {
            if ((buffer[0] != 0xFF) && (buffer[1] != 0xFF))
            {
                // Fragmented?
                if ((buffer[OffsetSequence] & MaskTopByteIsFragmented) != 0)
                {
                    // zero payload fragment is invalid.
                    if (buffer.size() > MinimumPacketSizeFragment)
                    {
                        result = true;
                    }
                }
                else
                {
                    if (buffer.size() >= MinimumPacketSizeServer)
                    {
                        // high byte first.
                        if (0 == (buffer[OffsetIsServerFlags] & MaskTopByteIsServerPacket))
                        {
                            // top bit set of ack == server packet
                            if (buffer.size() >= MinimumPacketSizeClient)
                            {
                                result = true;
                            }
                        }
                        else
                        {
                            if (buffer.size() >= MinimumPacketSizeServer)
                            {
                                result = true;
                            }
                        }
                    }
                }
            }
        }
        else
        {
            if (buffer.size() >= MinimumPacketSizeFragment)
            {
                result = (0 != (buffer[OffsetIsFragmented] & MaskTopByteIsFragmented));
            }
        }
    }

    return result;
}

PacketDelta::PacketDelta(std::vector<uint8_t> rawData)
    : Packet(rawData)
{
}

PacketDelta::PacketDelta(
        WrappingCounter<uint16_t> sequence,
        WrappingCounter<uint16_t> sequenceAck,
        uint8_t sequenceAckDelta,
        uint16_t* clientId,
        std::vector<uint8_t> deltaPayload)
    : PacketDelta()
{
    if (clientId != nullptr)
    {
        myBuffer.reserve(MinimumPacketSizeClient + deltaPayload.size());
    }
    else
    {
        myBuffer.reserve(MinimumPacketSizeServer + deltaPayload.size());
    }

    auto inserter = back_inserter(myBuffer);
    Push(inserter, sequence.Value());
    Push(inserter, sequenceAck.Value());
    myBuffer.push_back(sequenceAckDelta);

    if (clientId != nullptr)
    {
        Push(inserter, *clientId);
    }
    else
    {
        myBuffer[OffsetIsServerFlags] |= MaskTopByteIsServerPacket;
    }

    myBuffer.insert(end(myBuffer), begin(deltaPayload), end(deltaPayload));
}

PacketDelta::PacketDelta(
        PacketDelta toFragment,
        std::size_t maxPacketSize,
        uint8_t fragmentId)
    : PacketDelta()
{
    if (
            toFragment.IsValid() &&
            (!toFragment.IsFragmented()) &&
            (fragmentId < MaskIsLastFragment) &&
            (maxPacketSize > MinimumPacketSizeFragment)
       )
    {
        std::size_t maxSize((maxPacketSize - MinimumPacketSizeFragment) * (MaskIsLastFragment - 1));

        // too big to fit?
        if ((toFragment.Size() - OffsetSequenceAck) <= maxSize)
        {
            // test it needs fragmenting
            if ((toFragment.Size() < maxPacketSize) && (fragmentId == 0))
            {
                // just copy, no fragmentation needed.
                myBuffer = toFragment.myBuffer;
            }
            else
            {
                std::size_t start;
                std::size_t count;

                count = (maxPacketSize - OffsetFragmentPayload);
                start = OffsetSequenceAck + fragmentId * count;

                // Past the end?
                if (start <= toFragment.Size())
                {
                    // last packet?
                    if ((start + count) >= toFragment.Size())
                    {
                        fragmentId |= MaskIsLastFragment;
                        count = toFragment.Size() - start;
                    }

                    myBuffer.reserve(count + OffsetFragmentPayload);

                    // write out the sequence and fragment id
                    Push(back_inserter(myBuffer), toFragment.GetSequence().Value());
                    myBuffer.push_back(fragmentId);

                    // it's a fragment!
                    myBuffer[OffsetSequence] |= MaskTopByteIsFragmented;

                    myBuffer.insert(
                        end(myBuffer),
                        begin(toFragment.myBuffer) + start,
                        begin(toFragment.myBuffer) + start + count);
                }
            }
        }
    }
}

PacketDelta::PacketDelta(std::vector<PacketDelta> fragments)
    : PacketDelta()
{
    if (!fragments.empty())
    {
        std::vector<PacketDelta*> sorted(fragments.size());

        // deduplicate
        for (auto& fragment : fragments)
        {
            if (fragment.IsFragmented())
            {
                // take the last duplicate.
                sorted[fragment.FragmentId()] = &fragment;
            }
        }

        if (sorted.size() < MaskIsLastFragment)
        {
            if (sorted[0] != nullptr)
            {
                if (sorted[0]->IsValid())
                {
                    std::size_t maxFragmentSize(sorted[0]->Size() - OffsetFragmentPayload);
                    std::size_t bufferSize(2 + (sorted.size() * maxFragmentSize));
                    std::vector<uint8_t> buffer;
                    WrappingCounter<uint16_t> fragmentSequence(sorted[0]->GetSequence());
                    bool isValid(true);

                    buffer.reserve(bufferSize);

                    // copy the sequence to the buffer
                    // (IsValid() so we assume the size is > 2).
                    Push(back_inserter(buffer), fragmentSequence.Value());

                    // verify and join.
                    for (auto& fragment : sorted)
                    {
                        if (fragment == nullptr)
                        {
                            isValid = false;
                            break;
                        }

                        if (!fragment->IsValid())
                        {
                            isValid = false;
                            break;
                        }

                        if (!fragment->IsFragmented())
                        {
                            isValid = false;
                            break;
                        }

                        if (fragment->GetSequence() != fragmentSequence)
                        {
                            isValid = false;
                            break;
                        }

                        buffer.insert(
                            end(buffer),
                            begin(fragment->myBuffer) + OffsetFragmentPayload,
                            end(fragment->myBuffer));

                        // last fragment id?
                        if (fragment->IsLastFragment())
                        {
                            break;
                        }
                    }

                    // any holes in the list (missing fragments) will be caught in the
                    // iterator due to the fragment pointer being nullptr.
                    if (isValid)
                    {
                        std::swap(myBuffer, buffer);
                    }
                }
            }
        }
    }
}

WrappingCounter<uint16_t> PacketDelta::GetSequence() const
{
    if (IsValid())
    {
        uint16_t rawSequence;
        Pull(begin(myBuffer) + OffsetSequence, rawSequence);

        return WrappingCounter<uint16_t>(rawSequence & MaskSequence);
    }
    else
    {
        return WrappingCounter<uint16_t>(0);
    }
}

WrappingCounter<uint16_t> PacketDelta::GetSequenceBase() const
{
    if (IsValid())
    {
        uint16_t base;
        Pull(begin(myBuffer) + OffsetSequenceAck, base);
        base &= MaskSequenceAck;

        return WrappingCounter<uint16_t>(base - myBuffer[OffsetDeltaBase]);
    }
    else
    {
        return WrappingCounter<uint16_t>(0);
    }
}

WrappingCounter<uint16_t> PacketDelta::GetSequenceAck() const
{
    if (IsValid())
    {
        uint16_t rawSequence;
        Pull(begin(myBuffer) + OffsetSequenceAck, rawSequence);

        return WrappingCounter<uint16_t>(rawSequence & MaskSequence);
    }
    else
    {
        return WrappingCounter<uint16_t>(0);
    }
}

bool PacketDelta::IsValid() const
{
    return IsPacketDelta(myBuffer);
}


bool PacketDelta::IsFragmented() const
{
    if (IsValid())
    {
        return (0 != (myBuffer[OffsetIsFragmented] & MaskTopByteIsFragmented));
    }
    else
    {
        return false;
    }
}

bool PacketDelta::IsLastFragment() const
{
    if (IsFragmented())
    {
        return ((myBuffer[OffsetFragmentId] & MaskIsLastFragment) != 0);
    }
    else
    {
        return false;
    }
}

uint8_t PacketDelta::FragmentId() const
{
    if (IsValid() && IsFragmented())
    {
        return myBuffer[OffsetFragmentId] & (0xFF ^ MaskIsLastFragment);
    }
    else
    {
        return 0;
    }
}

bool PacketDelta::HasClientId() const
{
    if (IsValid())
    {
        return (0 == (myBuffer[OffsetIsServerFlags] & MaskTopByteIsServerPacket));
    }
    else
    {
        return false;
    }
}

uint16_t PacketDelta::ClientId() const
{
    if (HasClientId())
    {
        uint16_t id;
        Pull(begin(myBuffer) + OffsetClientId, id);

        return id;
    }
    else
    {
        return 0;
    }
}

std::vector<uint8_t> PacketDelta::GetPayload() const
{
    if (IsValid())
    {
        if (IsFragmented())
        {
            return std::vector<uint8_t>(begin(myBuffer) + OffsetFragmentPayload, end(myBuffer));
        }
        else
        {
            if (HasClientId())
            {
                return std::vector<uint8_t>(begin(myBuffer) + OffsetDataClient, end(myBuffer));
            }
            else
            {
                return std::vector<uint8_t>(begin(myBuffer) + OffsetDataServer, end(myBuffer));
            }
        }
    }
    else
    {
        return std::vector<uint8_t>();
    }
}
