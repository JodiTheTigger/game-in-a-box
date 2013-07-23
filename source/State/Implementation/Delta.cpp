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

// /////////////////////
// Third Attempt
// /////////////////////

// Realise that the client cannot keep track of state via lossy info
// as its state will diverge from the servers without resyncing. Think
// mpeg encoding where you only send one I frame and the rest is B/P frames.
// If I want to do lossy compression, I need to send regular I frames.

// So knowing that, do the quake 3 format, where I send full data, without dropping any bits.

// Secondly, I can get around strict aliasing issues by using memcopy.
// as it's legal to alias char *. eg:
// char* structAsCharPointer;
// float asFloat;
// memcpy(structAsCharPointer + someoffset, &asFloat, sizeof(float));
//
// Only downside is that it puts your trust into the compiler not to do something stupid.
// Which is pretty well placed since gcc and clang do do the right thing ... due to the aliasing rule!

#include "TypesDelta.hpp"

#include <Common/BitStream.hpp>

#include <cstdint>
#include <string>

#include <cstring>

namespace GameInABox { namespace State { namespace Implementation {

constexpr unsigned BitsNeededForValue(unsigned value, unsigned bits)
{
    return ((1U < bits) > value) ? bits : BitsNeededForValue(value, bits + 1);
}

constexpr unsigned BitsNeededForValue(unsigned value)
{
    return BitsNeededForValue(value, 1);
}

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
void Code(uint32_t base, uint32_t target, const DeltaMap& map, BitStream& out, bool doZeros, bool doXor)
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

                    if (
                            (target != floatTarget) ||
                            ((floatTarget + map.maxRangeHalf) < 0) ||
                            ((floatTarget + map.maxRangeHalf) >= map.maxRange)
                        )
                    {
                        out.Push(true);

                        // full float.
                        // checks to make sure float = 4 bytes are done elsewhere.
                        out.Push(target, 32);
                    }
                    else
                    {
                        out.Push(false);

                        // as int.
                        out.Push(target, map.maxRangeBits);
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

// /////////////////////
// Second Attempt
// /////////////////////
#if 0
#include <cstdint>
//#include <cmath>
//#include <cstring> // memcpy

#include <vector>
#include <boost/optional.hpp>

#include "Delta.hpp"

using namespace std;
using boost::optional;

// Index is external to this class.
enum class Priority
{
    Normal = 0,
    High,
    Highest,
    Lossless,

    MaxValue = Lossless
};

static const int PriorityMaxValue = static_cast<int>(Priority::MaxValue);

enum class DeltaType
{
    Ignored,
    Unsigned32,
    FloatRaw,
    //Float0to1,
    //Float1to1,
    //FloatCastToUnsigned32,
    FloatRange
};

struct MapIgnored
{
    // nada
};

struct MapUnsigned
{
    unsigned bits;
    float ignored;
    Priority importance;
};

struct MapLossless
{
    // Loselesssssssss!
};

struct MapFloatRaw
{
    // treat as lossless.
};

struct MapFloat0to1
{
    unsigned bits;
    float ignored;
    Priority importance;
};

struct MapFloat1to1
{
    unsigned bits;
    float ignored;
    Priority importance;
};

struct MapFloatCastToUnsigned
{
    unsigned bits;
    float ignored;
    Priority importance;
};

struct MapFloatRange
{
    unsigned bits;
    float ignored;
    Priority importance;
    float m;
    float c;
};

struct DeltaMap
{
    DeltaType   type        {DeltaType::Ignored};
    unsigned    bits        {0};
    float       bitsIgnored {0.0f};
    Priority    importance  {Priority::Normal};
    float       m           {1.0f};
    float       c           {0.0f};


    // Not explicit on purpose, so that creating the deltamap isn't polluted
    // with DeltaMap{...} on each line.
    DeltaMap(MapIgnored)
        : DeltaMap()
    {
    }

    DeltaMap(MapUnsigned data)
        : type(DeltaType::Unsigned32)
        , bits(data.bits)
        , bitsIgnored(data.ignored)
        , importance(data.importance)
    {
    }

    DeltaMap(MapLossless)
        : type(DeltaType::Unsigned32)
        , bits(32)
        , bitsIgnored(0)
        , importance(Priority::Lossless)
    {
    }

    DeltaMap(MapFloatRaw)
        : type(DeltaType::FloatRaw)
        , importance(Priority::Lossless)
    {
    }

    DeltaMap(MapFloat0to1 data)
        : type(DeltaType::FloatRange)
        , bits(data.bits)
        , bitsIgnored(data.ignored)
        , importance(data.importance)
    {
    }

    DeltaMap(MapFloat1to1 data)
        : type(DeltaType::FloatRange)
        , bits(data.bits)
        , bitsIgnored(data.ignored)
        , importance(data.importance)
        , m(0.5f * ((1 << data.bits) - 1))
        , c(-1.0f * ((1 << data.bits) - 1))
    {
    }

    DeltaMap(MapFloatCastToUnsigned data)
        : type(DeltaType::FloatRange)
        , bits(data.bits)
        , bitsIgnored(data.ignored)
        , importance(data.importance)
    {
    }

    DeltaMap(MapFloatRange data)
        : type(DeltaType::Unsigned32)
        , bits(data.bits)
        , bitsIgnored(data.ignored)
        , importance(data.importance)
        , m(data.m)
        , c(data.c)
    {
    }

    DeltaMap() = default;
};

/*
struct StatePlayer
{
    ServerId        id;
    std::uint32_t   health;
    std::uint32_t   energy;
    FlagsPlayer     flags;
    ServerTick      lastShot;
    Vec3            position;
    Vec3            orientation;
};
*/

// my first type
struct bitcount22
{
    unsigned value;
};

struct bitsIgnored
{
    float value;
};

bitcount22 operator"" _bits(unsigned long long valueToUse)
{
    return {static_cast<unsigned>(valueToUse)};
}

struct fred
{
    bitcount22 bbb;
    bitcount22 ccc;
};

auto bob = fred{12_bits, 44_bits};

// Still prone to PEBKAC as it's easy it forget a struct member
// Or put them out of order. Need to figure out how to make that
// less easy to do (either at run time or compile time).
// Second, 7,3 - what does that mean? why don't we try custom data types
// and change it to 7_bits and 3_bitsIgnored?
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

float CompressionRatio(float compress, Priority importance)
{
    switch (importance)
    {
        case Priority::Highest:      compress = compress * compress;
        case Priority::High:    compress = compress * compress;
        case Priority::Normal:
        {
            return compress;
        }

        default:
        case Priority::Lossless:
        {
            return 0.0f;
        }
    }
}

#endif // #if 0

// /////////////////////
// First Attempt
// /////////////////////
#if 0
// for now break pointer rules until you can figure out how to do this
// really don't want this as a template. How do references to arrays?
void DeltaCreate(
        const uint32_t* ,//base,
        const uint32_t* ,//target,
        const DeltaMap* ,//map,
        float ,//compression0to1,
        size_t size)
{
    // ints
    for (size_t i = 0; i < size; ++i)
    {
        // 1 map should be made so we don't need this test
        // 2 buffer override, how to check invalid inputs?
        /*
        if (map[i].mask != 0)
        {
            auto compression = CompressionRatio(compression0to1, map[i].importance);
            auto ignoredBits = static_cast<unsigned>(compression * map[i].bitsIgnored);
            auto ignoreMask = 0xFFFFFFFF << ignoredBits;

            auto delta = (base[i] ^ target[i]) & ignoreMask;

            if (delta > 0)
            {
                // Has changed.
            }
            else
            {
                // report nothing changed.
            }
        }*/
    }
}

/*
struct DeltaMap
{
    // Signifies the max bits allowed.
    uint32_t mask;

    // Signifies the max bits allowed - min bits allowed;
    unsigned bitDiff;

    // determines how we apply the compressions %
    Priority importance;
};

uint32_t FloatToInt(float toConvert)
{
    // straight conversion without aliasing.
    uint32_t result;
    memcpy(&toConvert, &result, sizeof(toConvert));

    return result;
}

uint32_t FloatToInt(float toConvert, float m, float c)
{
    // only useful if you use 23 bits or less, as the mantessa
    // is only 32 bits. And only useful if +ve.
    float scaled = m*toConvert + c;

    return static_cast<uint32_t>(scaled);
}

uint32_t FloatToIntNormalised(float toConvertNormalised)
{
    return static_cast<uint32_t>(toConvertNormalised * 65535.0f);
}



void Delta(uint32_t base, uint32_t target, DeltaMap delta, float compress)
{
    auto finalCompress = pow(compress, 2.0f * static_cast<float>(Priority::MaxValue - delta.importance));
    auto compressMask = 0xFFFFFFFF << (bitsToIgnore);
    mask &= compressMask;

    // where the compression % = 1 - (x^2*(MaxValue - priority));
    switch (delta.importance)
    {
        case High:      compress = compress * compress;
        case Normal:    compress = compress * compress;
        case Lowest:
        {
            auto bitsToIgnore = static_cast<unsigned>(delta.bitDiff * compress);
            auto compressMask = 0xFFFFFFFF << (bitsToIgnore);
            mask &= compressMask;
            break;
        }

        case Priority::Lossless:
        {
            break;
        }
    }

    auto result = (base ^ target) & mask;
}

// so, we want mask, and bitDiff (max bits - min bits).
*/
#endif // #if 0
