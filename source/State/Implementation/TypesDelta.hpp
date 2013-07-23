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

#include <cstdint>
#include <memory>


namespace GameInABox {
// RAM: TODO: Clean this up
namespace Common {
class BitStream;
class BitStreamReadOnly;
}
namespace State { namespace Implementation {

// RAM: TODO! Move this into its own common types header.
struct Bits
{
    unsigned value;
};

// RAM: TODO! Move this into its own common types header.
Bits constexpr operator"" _bits(unsigned long long value)
{
    return {static_cast<unsigned>(value)};
}

// RAM: TODO! Move this into its own common types header.
using ByteOffset = std::size_t;

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
    ByteOffset  offset;
};

// Internal Store.
struct DeltaMapInternal;

class DeltaMapItem
{
public:
    // Can't have a destructor in the header file if using
    // unique_ptr + pimpl idiom.
    ~DeltaMapItem();

    DeltaMapItem(Offset offsetToUse, MapFloatFull specs);
    DeltaMapItem(Offset offsetToUse, MapSigned specs);
    DeltaMapItem(Offset offsetToUse, MapUnsigned specs);
    DeltaMapItem(Offset offsetToUse, MapFloatRanged specs);
    DeltaMapItem(Offset offsetToUse, MapFloatRangeStrict specs);

private:
    DeltaMapItem(Offset offsetToUse);

    std::unique_ptr<DeltaMapInternal> myPimpl;
};
/*
void Code(
     uint32_t base,
     uint32_t target,
     const DeltaMapItem& map,
     GameInABox::Common::BitStream& out,
     bool doZeros,
     bool doXor);*/

// /////////////////////
// Delta Coder Interface
// /////////////////////
/*template<class OBJECT>
class DeltaCoder
{
public:
    DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        OBJECT identity,
        bool researchEncodeZeros,
        bool researchEncodeXorDeltas);

 // if base == nullptr use the identity
     void DeltaDecode(
         const OBJECT* base,
         OBJECT& result,
         GameInABox::Common::BitStreamReadOnly& dataIn) const;

     // if base == nullptr use the identity
     // Returns true if base == toDelta, otherwise false.
     bool DeltaEncode(
         const OBJECT* base,
         const OBJECT& toDelta,
         GameInABox::Common::BitStream& dataOut) const;

private:
    const std::vector<DeltaMapItem> myDeltaMap;
    const OBJECT myIdentityObject;
    const bool myResearchEncodeZeros;
    const bool myResearchEncodeXorDeltas;
};*/


}}} // namespace

#endif // TYPESDELTA_HPP
