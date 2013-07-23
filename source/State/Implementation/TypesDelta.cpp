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

#include "TypesDelta.hpp"

#include <Common/MakeUnique.hpp>
#include <Common/BitStream.hpp>

// memcpy.
#include <cstring>

// RAM: TODO: Check that float == 4 bytes.

using namespace GameInABox::Common;

namespace GameInABox { namespace State { namespace Implementation {

enum class MapType
{
    Ignore,
    FloatFull,
    FloatRanged,
    FloatRangedStrict,
    Unsigned,
    Signed,
};

struct DeltaMapInternal
{
    std::string name{""};
    ByteOffset offset{0};
    MapType type{MapType::Unsigned};

    signed bits{0};

    unsigned maxRange{0};

    float m{1.0f};
    float c{0.0f};
};

DeltaMapItem::~DeltaMapItem()
{
    // Empty.
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse)
    : myPimpl(make_unique<DeltaMapInternal>())
{
    myPimpl->name = offsetToUse.name;
    myPimpl->offset = offsetToUse.offset;
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatFull)
    : DeltaMapItem(offsetToUse)
{
    myPimpl->type = MapType::FloatFull;
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapSigned specs)
    : DeltaMapItem(offsetToUse, MapUnsigned{specs.resolution})
{
    myPimpl->type = MapType::Signed;
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapUnsigned specs)
    : DeltaMapItem(offsetToUse)
{
    if (specs.resolution.value < 1)
    {
        myPimpl->type = MapType::Ignore;
    }
    else
    {
        myPimpl->bits = specs.resolution.value;
        if (myPimpl->bits > 32)
        {
            myPimpl->bits = 32;
        }
    }
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatRanged specs)
    : DeltaMapItem(offsetToUse)
{
    if (specs.maxValue > 0)
    {
        // figure out the range in bits.
        Bits resolution;

        while ((1U << resolution.value) < (2U*specs.maxValue))
        {
            ++(resolution.value);
        }

        // float has 23 bits of mantessa, no point doing this is we require a larger dynamic range.
        if (resolution.value > 22)
        {
            myPimpl->type = MapType::FloatFull;
        }
        else
        {
            myPimpl->type = MapType::FloatRanged;
            myPimpl->bits = resolution.value;
            myPimpl->maxRange = 1 << resolution.value;
        }
    }
    else
    {
        myPimpl->type = MapType::Ignore;

        // RAM: TODO: LOG!
    }
}

DeltaMapItem::DeltaMapItem(Offset offsetToUse, MapFloatRangeStrict specs)
    : DeltaMapItem(offsetToUse)
{
    if (specs.resolution.value >= 1)
    {
        auto range = specs.maxValue - specs.minValue;

        if (range > 0)
        {
            myPimpl->type = MapType::FloatRangedStrict;

            myPimpl->c = - specs.minValue;
            myPimpl->m = ((1 << specs.resolution.value) - 1) / range;
            myPimpl->bits = specs.resolution.value;
        }
        else
        {
            myPimpl->type = MapType::Ignore;

            // RAM: TODO: LOG!
        }
    }
    else
    {
        myPimpl->type = MapType::Ignore;

        // RAM: TODO: LOG!
    }
}

// RAM: TODO: Marge this and TypesDelta into it's own Delta class please.
// RAM: TODO: Clean Name.
// RAM: TODO: Move DeltaMap data types to their own header.
void Code(uint32_t base, uint32_t target, const DeltaMapInternal& map, BitStream& out, bool doZeros, bool doXor)
{
    if (base == target)
    {
        out.Push(true);
        return;
    }
    else
    {
        out.Push(false);

        if ((target == 0) && (doZeros))
        {
            out.Push(true);
            return;
        }
        else
        {
            if (doZeros)
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
                        out.Push(target, 32);
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

                    // RAM: TODO: Research Xoring or not.
                    if (doXor)
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


}}} // namespace
