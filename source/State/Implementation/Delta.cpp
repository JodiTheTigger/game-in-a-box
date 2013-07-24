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

#include "Delta.hpp"
#include "DeltaMapItemInternal.hpp"

#include <cstdint>
#include <cstring>

// RAM: TODO: Check that float == 4 bytes.

using namespace GameInABox::Common;

namespace GameInABox { namespace State { namespace Implementation {

/*
DeltaMap myMap[] = {
    {OFFSET_INFO(StatePlayer, position.x), MapFloatRangeStrict{500.0f, -500.0f, 17}},
    {OFFSET_INFO(StatePlayer, position.y), MapFloatRangeStrict{500.0f, -500.0f, 17}},
    {OFFSET_INFO(StatePlayer, position.z}, MapFloatRangeStrict{500.0f, -500.0f, 17}},

    {OFFSET_INFO(StatePlayer, orientation.x}, MapFloatRangeStrict{1.0f, -1.0f, 12}},
    {OFFSET_INFO(StatePlayer, orientation.y}, MapFloatRangeStrict{1.0f, -1.0f, 12}},
    {OFFSET_INFO(StatePlayer, orientation.z}, MapFloatRangeStrict{1.0f, -1.0f, 12}},

    {OFFSET_INFO(StatePlayer, health), Map32bitsUnsigned{10}},
    {OFFSET_INFO(StatePlayer, energy), Map32bitsUnsigned{10}},

    {OFFSET_INFO(StatePlayer, flags), Map32bitsUnsigned{32}},
};
*/
/*
struct StatePlayer
{
    ServerId        id;             // ignore
    Vec3            position;       // assume range is +/- 500m, so ranged, 17 bits.
    Vec3            orientation;    // assume range is +/- 1.0,  12 bits should be fine for angles (360/4096).
    std::uint32_t   health;         // 0 - 1000, 10 bits is fine
    std::uint32_t   energy;         // 0 - 1000, 10 bits is fine again
    FlagsPlayer     flags;          // full uint until I know how many flags there are. 32 bits.
    ServerTick      lastShot;       // Ignore
};
*/

/*
DeltaMap myMap2[] = {
    MapIgnored{},
    MapUnsigned{7, 3, Priority::Normal},
    MapIgnored{},
    MapLossless{},
    MapIgnored{},

    // Position
    MapFloatCastToUnsigned{16, 11, Priority::Highest},
    MapFloatCastToUnsigned{16, 11, Priority::Highest},
    MapFloatCastToUnsigned{16, 11, Priority::Highest},

    // Orientation
    MapFloat1to1{16, 11, Priority::Normal},
    MapFloat1to1{16, 11, Priority::Normal},
    MapFloat1to1{16, 11, Priority::Normal},
};
*/


// RAM: TODO: Marge this and TypesDelta into it's own Delta class please.
// RAM: TODO: Clean Name.
// RAM: TODO: Research Xoring or not.
void DeltaCreate(
        std::uint32_t base,
        std::uint32_t target,
        const DeltaMapItemInternal& map,
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
                case MapType::FloatRanged:
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

                        memcpy(&uintTarget, &intTarget, sizeof(uint32_t));

                        // C++ standard states that aliasing is allowed between
                        // signed and unsigned values of the same width data type.
                        out.Push(uintTarget, map.bits);
                    }

                    break;
                }

                // Assume the scaled range is from 0 to (2^bits - 1) inclusive.
                case MapType::FloatRangedStrict:
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

                    // drop through to MapType::Unsigned on purpose.
                }

                default:
                case MapType::FloatFull:
                case MapType::Unsigned:
                case MapType::Signed:
                {
                    // NOTE: For signed the top bit is always set if -ve
                    // due to 2s complement, so no special treatment here.
                    if (settings.doXor)
                    {
                        out.Push(base ^ target, map.bits);
                    }
                    else
                    {
                        out.Push(target, map.bits);
                    }

                    break;
                }
            }
        }
    }
}

std::uint32_t DeltaParse(
        std::uint32_t base,
        const DeltaMapItemInternal& map,
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
                case MapType::FloatRanged:
                {
                    // RAM: TODO
                    return 0;
                }

                // Assume the scaled range is from 0 to (2^bits - 1) inclusive.
                case MapType::FloatRangedStrict:
                {
                    // RAM: TODO
                    return 0;
                }

                default:
                case MapType::FloatFull:
                case MapType::Unsigned:
                case MapType::Signed:
                {

                    // RAM: TODO
                    return 0;
                }
            }
        }
    }
}

}}} // namespace

