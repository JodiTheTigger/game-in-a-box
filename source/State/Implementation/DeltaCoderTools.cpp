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

#include "DeltaCoderTools.hpp"
#include "DeltaMapItem.hpp"

#include <Common/BitStream.hpp>
#include <Common/BitStreamReadOnly.hpp>
#include <Common/Logging.hpp>

#include <cstring>

using namespace GameInABox::Common;

namespace GameInABox { namespace State { namespace Implementation {

void DeltaCreate(
        std::uint32_t base,
        std::uint32_t target,
        const DeltaMapItem& map,
        BitStream& out,
        Research settings)
{
    if (base == target)
    {
        out.Push(true);
        return;
    }
    else
    {
        out.Push(false);

        if ((target == 0) && (settings.doZeros))
        {
            out.Push(true);
            return;
        }
        else
        {
            if (settings.doZeros)
            {
                out.Push(false);
            }

            switch (map.type)
            {
                // If the number is an uint, send as one, otherwise
                // send as a full float. Not as good as FloatRangedStrict.
                case DeltaMapItem::MapType::FloatRanged:
                {
                    float floatTarget;

                    // memcpy avoids aliasing issues.
                    memcpy(&floatTarget, &target, sizeof(float));

                    int32_t intTarget = static_cast<int32_t>(floatTarget);

                    if (
                            (intTarget != floatTarget) ||
                            (floatTarget > map.maxRange) ||
                            (floatTarget < -map.maxRange)
                        )
                    {
                        out.Push(true);

                        // full float.
                        if (settings.doXor)
                        {
                            out.Push(base ^ target, 32);
                        }
                        else
                        {
                            out.Push(target, 32);
                        }
                    }
                    else
                    {
                        uint32_t uintTarget;

                        out.Push(false);

                        // C++ standard states that aliasing is allowed between
                        // signed and unsigned values of the same width data type.
                        // but reinterpret_cast only works on pointers. Meh, just memcpy.
                        memcpy(&uintTarget, &intTarget, sizeof(uint32_t));

                        out.Push(uintTarget, map.resolution.value);
                    }

                    break;
                }

                // Assume the scaled range is from 0 to (2^bits - 1) inclusive.
                case DeltaMapItem::MapType::FloatRangedStrict:
                {
                    float floatTarget;
                    float floatBase;

                    // memcpy avoids aliasing issues.
                    memcpy(&floatTarget, &target, sizeof(float));
                    memcpy(&floatBase, &base, sizeof(float));

                    // Scale and convert to straight dword of x bits.
                    floatTarget = floatTarget * map.m + map.c;
                    floatBase = floatBase * map.m + map.c;

                    // Convert to dword and treat as a normal dword.
                    target = static_cast<uint32_t>(floatTarget);
                    base = static_cast<uint32_t>(floatBase);

                    // drop through to DeltaMapItem::MapType::Unsigned on purpose.
                }

                case DeltaMapItem::MapType::Unsigned:
                case DeltaMapItem::MapType::Signed:
                {
                    // NOTE: For signed the top bit is always set if -ve
                    // due to 2s complement, so no special treatment here.
                    if (settings.doXor)
                    {
                        out.Push(base ^ target, map.resolution.value);
                    }
                    else
                    {
                        out.Push(target, map.resolution.value);
                    }

                    break;
                }

                default:
                case DeltaMapItem::MapType::Ignore:
                {
                    // ignore.
                    break;
                }
            }
        }
    }
}

std::uint32_t DeltaParse(
        std::uint32_t base,
        const DeltaMapItem& map,
        GameInABox::Common::BitStreamReadOnly& in,
        Research settings)
{
    if (in.Pull1Bit())
    {
        return base;
    }
    else
    {
        bool isZero = false;

        if (settings.doZeros)
        {
            isZero = in.Pull1Bit();
        }

        if (isZero)
        {
            return 0;
        }
        else
        {
            switch (map.type)
            {
                case DeltaMapItem::MapType::FloatRanged:
                {
                    if (in.Pull1Bit())
                    {
                        auto delta = in.PullU32(32);

                        // full float.
                        if (settings.doXor)
                        {
                            return base ^ delta;
                        }
                        else
                        {
                            return delta;
                        }
                    }
                    else
                    {
                        float asFloat;
                        uint32_t result;

                        asFloat = in.PullU32(map.resolution.value);
                        memcpy(&result, &asFloat, sizeof(float));

                        return result;
                    }
                }

                case DeltaMapItem::MapType::FloatRangedStrict:
                {
                    uint32_t delta = in.PullU32(map.resolution.value);

                    if (settings.doXor)
                    {
                        float floatBase;

                        memcpy(&floatBase, &base, sizeof(float));
                        floatBase = floatBase * map.m + map.c;
                        auto baseConverted = static_cast<uint32_t>(floatBase);

                        delta ^= baseConverted;
                    }

                    float asFloat = (delta - map.c) * map.inversem;
                    uint32_t result;

                    memcpy(&result, &asFloat, sizeof(float));

                    return result;
                }

                case DeltaMapItem::MapType::Signed:
                {
                    uint32_t result = in.PullU32(map.resolution.value);

                    if (settings.doXor)
                    {
                        result ^= base;
                    }

                    // extend the sign bit all along to make a valid -ve number.
                    if ((result & (1 << (map.resolution.value - 1))) > 0)
                    {
                        result |= (0xFFFFFFFF << map.resolution.value);
                    }

                    return result;
                }

                case DeltaMapItem::MapType::Unsigned:
                {
                    // full float.
                    if (settings.doXor)
                    {
                        return base ^ in.PullU32(map.resolution.value);
                    }
                    else
                    {
                        return in.PullU32(map.resolution.value);
                    }
                }

                default:
                case DeltaMapItem::MapType::Ignore:
                {                    
                    Common::Log(
                        Common::LogLevel::Warning,
                        "DeltaCoder: Map item '",
                        map.offsetInfo.name.c_str(),
                        "' is ignored returning 0xFFFFFFFF. This shouldn't happen, there is a bug. Find it and squash it!");

                    return 0xFFFFFFFF;
                }
            }
        }
    }
}

}}} // namespace

