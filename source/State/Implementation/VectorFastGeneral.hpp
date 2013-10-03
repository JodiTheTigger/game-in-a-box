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

#ifndef VectorFastGeneral_HPP
#define VectorFastGeneral_HPP

#include "VectorLoad.hpp"

#include <array>
#include <cmath>

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) VectorFastGeneral
{
    std::array<float, 4> values;
};

// ///////////////////
// Loads
// ///////////////////
template<> inline constexpr VectorFastGeneral Load<VectorFastGeneral>(float x, float y, float z, float w)
{
    return VectorFastGeneral
    {{{
            x,
            y,
            z,
            w,
    }}};
}

template<> inline constexpr VectorFastGeneral Load<VectorFastGeneral>(float x, float y, float z)
{
    return VectorFastGeneral
    {{{
            x,
            y,
            z,
            0.0f,
    }}};
}

template<> inline constexpr VectorFastGeneral Load<VectorFastGeneral>(float x, float y)
{
    return VectorFastGeneral
    {{{
            x,
            y,
            0.0f,
            0.0f,
    }}};
}

template<> inline constexpr VectorFastGeneral Load<VectorFastGeneral>(float x)
{
    return VectorFastGeneral
    {{{
            x,
            0.0f,
            0.0f,
            0.0f,
    }}};
}

template<> inline constexpr VectorFastGeneral Load<VectorFastGeneral>(const Vector& v)
{
    return VectorFastGeneral
    {{{
            X(v),
            Y(v),
            Z(v),
            W(v),
    }}};
}

template<> inline constexpr VectorFastGeneral LoadReplicate<VectorFastGeneral>(float x)
{
    return VectorFastGeneral
    {{{
            x,
            x,
            x,
            x,
    }}};
}

// ///////////////////
// Save
// ///////////////////

Vector Save(VectorFastGeneral v)
{
    return Vector
    {{{
            v.values[0],
            v.values[1],
            v.values[2],
            v.values[3],
    }}};
}

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// RAM: Debug

// ///////////////////
// Increment / Decrement
// ///////////////////
inline VectorFastGeneral& operator++(VectorFastGeneral& rhs)
{
    ++(rhs.values[0]);
    ++(rhs.values[1]);
    ++(rhs.values[2]);
    ++(rhs.values[3]);

    return rhs;
}

inline VectorFastGeneral operator++(VectorFastGeneral& lhs, int)
{
   auto copy = lhs;
   ++lhs;

   return copy;
}

inline VectorFastGeneral& operator--(VectorFastGeneral& rhs)
{
    --(rhs.values[0]);
    --(rhs.values[1]);
    --(rhs.values[2]);
    --(rhs.values[3]);

    return rhs;
}

inline VectorFastGeneral operator--(VectorFastGeneral& lhs, int)
{
    auto copy = lhs;
    --lhs;

    return copy;
}

// ///////////////////
// Comparison Operators
// ///////////////////

inline bool operator==(const VectorFastGeneral& lhs, const VectorFastGeneral& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const VectorFastGeneral& lhs, const VectorFastGeneral& rhs){return  !operator==(lhs,rhs);}
inline bool operator< (const VectorFastGeneral& lhs, const VectorFastGeneral& rhs){return lhs.values<rhs.values;}
inline bool operator> (const VectorFastGeneral& lhs, const VectorFastGeneral& rhs){return   operator< (rhs,lhs);}
inline bool operator<=(const VectorFastGeneral& lhs, const VectorFastGeneral& rhs){return  !operator> (lhs,rhs);}
inline bool operator>=(const VectorFastGeneral& lhs, const VectorFastGeneral& rhs){return  !operator< (lhs,rhs);}

// ///////////////////
// Simple Maths
// ///////////////////
inline VectorFastGeneral& operator+=(VectorFastGeneral& lhs, const VectorFastGeneral& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    lhs.values[3] += rhs.values[3];
    return lhs;
}

inline VectorFastGeneral& operator-=(VectorFastGeneral& lhs, const VectorFastGeneral& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    lhs.values[3] -= rhs.values[3];
    return lhs;
}

inline VectorFastGeneral& operator*=(VectorFastGeneral& lhs, const VectorFastGeneral& rhs)
{
    lhs.values[0] *= rhs.values[0];
    lhs.values[1] *= rhs.values[1];
    lhs.values[2] *= rhs.values[2];
    lhs.values[3] *= rhs.values[3];
    return lhs;
}

inline VectorFastGeneral& operator/=(VectorFastGeneral& lhs, const VectorFastGeneral& rhs)
{
    lhs.values[0] /= rhs.values[0];
    lhs.values[1] /= rhs.values[1];
    lhs.values[2] /= rhs.values[2];
    lhs.values[3] /= rhs.values[3];
    return lhs;
}

inline constexpr VectorFastGeneral operator-(const VectorFastGeneral& lhs)
{
    // Ugh, clang!
    // Three braces: First for copy init, second for the struct, third for the array.
    return VectorFastGeneral
    {{{
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
        -lhs.values[3]
    }}};
}

inline VectorFastGeneral operator+(VectorFastGeneral lhs, const VectorFastGeneral& rhs){ lhs += rhs;  return lhs; }
inline VectorFastGeneral operator-(VectorFastGeneral lhs, const VectorFastGeneral& rhs){ lhs -= rhs;  return lhs; }
inline VectorFastGeneral operator*(VectorFastGeneral lhs, const VectorFastGeneral& rhs){ lhs *= rhs;  return lhs; }
inline VectorFastGeneral operator/(VectorFastGeneral lhs, const VectorFastGeneral& rhs){ lhs /= rhs;  return lhs; }

// ///////////////////
// Complicated Maths
// ///////////////////
inline VectorFastGeneral Sqrt(const VectorFastGeneral& rhs)
{
    return VectorFastGeneral
    {{{
            std::sqrt(rhs.values[0]),
            std::sqrt(rhs.values[1]),
            std::sqrt(rhs.values[2]),
            std::sqrt(rhs.values[3])
    }}};
}

inline VectorFastGeneral Dot(VectorFastGeneral lhs, const VectorFastGeneral& rhs)
{
    auto sum =
            (lhs.values[0] * rhs.values[0]) +
            (lhs.values[1] * rhs.values[1]) +
            (lhs.values[2] * rhs.values[2]) +
            (lhs.values[3] * rhs.values[3]);

    return VectorFastGeneral
    {{{
         sum,
         sum,
         sum,
         sum
    }}};
}

inline VectorFastGeneral Length(VectorFastGeneral rhs)
{
    // return Sqrt(Dot(rhs, rhs));

    float length = sqrt(
            (rhs.values[0] * rhs.values[0]) +
            (rhs.values[1] * rhs.values[1]) +
            (rhs.values[2] * rhs.values[2]) +
            (rhs.values[3] * rhs.values[3]));

    return VectorFastGeneral
    {{{
         length,
         length,
         length,
         length
    }}};
}

inline VectorFastGeneral LengthSquared(VectorFastGeneral rhs)
{
    return Dot(rhs, rhs);
}

inline VectorFastGeneral Mad(const VectorFastGeneral& lhs, const VectorFastGeneral& rhs, const VectorFastGeneral& add)
{
    return VectorFastGeneral
    {{{
        fmaf(lhs.values[0], rhs.values[0], add.values[0]),
        fmaf(lhs.values[1], rhs.values[1], add.values[1]),
        fmaf(lhs.values[2], rhs.values[2], add.values[2]),
        fmaf(lhs.values[3], rhs.values[3], add.values[3])
    }}};
}

inline VectorFastGeneral NormaliseFast(VectorFastGeneral lhs)
{
    return lhs / Length(lhs);
}

inline VectorFastGeneral NormaliseAccurate(VectorFastGeneral lhs)
{
    return lhs / Length(lhs);
}

}}} // namespace

#endif // VectorFastGeneral_HPP
