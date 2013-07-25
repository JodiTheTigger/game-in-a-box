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

#ifndef TYPESDELTA_HPP
#define TYPESDELTA_HPP

#include <string>

#include "Units.hpp"

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Delta Map Data Types
// /////////////////////

// A Full float, encode all 32 bits please.
struct MapFloatFull
{
};

// Signed integer.
// Resolution is capped at 32 bits.
// Note that you need to count the sign bit, so the minimum
// size would be 1 bits (sign only ie +/- 0), and the max 32 (sign + 31).
// A resolution of 0 is ignored for delta coding.
struct MapSigned
{
    Bits resolution;
};

// Unsigned integer.
// Resolution is capped at 32 bits.
// A resolution of 0 is ignored for delta coding.
struct MapUnsigned
{
    Bits resolution;
};

// A Full float but with a shortcut:
// Use this if the value is normally
// between the range of +/- max
// It will be encoded to less bits if
// the value is a whole number and falls
// within the specified range, otherwise
// it's treated as a full float.
// floats only have 23 bits of mantessa, so if
// the range (2*maxValue) > 2^23-1 then a full float
// is used instead.
// A maxValue of 0 is ignored for delta coding.
struct MapFloatRanged
{
    unsigned maxValue;
};

// Float is always in the range provided
// encoding becomes undefined if it is not.
// use the bits to determine what resolution you
// store the range in.
// Resolution is capped at 32 bits.
// A resolution of 0 is ignored for delta coding.
struct MapFloatRangeStrict
{
    float maxValue;
    float minValue;
    Bits resolution;
};

// /////////////////////
// Delta Map interface
// /////////////////////

// Used to track offsets in structures for delta encoding/decoding.
struct Offset
{
    std::string name;
    Bytes       offset;
};

// ///////////////////
// Delta Map interface Macros
// ///////////////////
#define MAKE_OFFSET(CLASS_TYPE, CLASS_MEMBER) Offset{{#CLASS_MEMBER}, {offsetof(CLASS_TYPE, CLASS_MEMBER)}}

}}} // namespace

#endif // TYPESDELTA_HPP
