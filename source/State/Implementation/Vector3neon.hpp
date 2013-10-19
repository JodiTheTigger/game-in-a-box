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

#ifndef VECTOR3NEON_HPP
#define VECTOR3NEON_HPP

#include "Vector.hpp"
#include "VectorCommon.hpp"
#include "UnitsSI.hpp"

#include <array>
#include <cmath>
#include <limits>

// RAM: TODO: single inputs are called lhs, double, lhs and rhs. Validate.

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Vector3neon
{
    struct tagReplicate {};
    struct tagAccurate {};

    std::array<float, 4> values;

    constexpr Vector3neon()
        : values{{0.0f, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector3neon(float x)
        : values{{x, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector3neon(float x, float y)
        : values{{x, y, 0.0f, 0.0f}} {}
    constexpr Vector3neon(float x, float y, float z)
        : values{{x, y, z, 0.0f}} {}

    constexpr Vector3neon(Vector vector)
        : values(vector.values) {}
    constexpr Vector3neon(const std::array<float, 4>& array)
        : values(array) {}
    constexpr Vector3neon(float x, tagReplicate)
        : values{{x, x, x, x}} {}

    Vector3neon(const Vector3neon&) = default;
    Vector3neon(Vector3neon&&) = default;
    Vector3neon& operator=(const Vector3neon&) & = default;
    Vector3neon& operator=(Vector3neon&&) & = default;

    constexpr Vector ToVector() const { return Vector{values}; }
};

// ///////////////////
// Comparison Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.
inline bool operator==(const Vector3neon& lhs, const Vector3neon& rhs)
{
    return  (
                (lhs.values[0]==rhs.values[0]) &&
                (lhs.values[1]==rhs.values[1]) &&
                (lhs.values[2]==rhs.values[2])
            );
}

inline bool operator!=(const Vector3neon& lhs, const Vector3neon& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Prototypes
// ///////////////////
inline Vector3neon Absolute(const Vector3neon& lhs);

// ///////////////////
// Simple Maths
// ///////////////////
inline Vector3neon& operator+=(Vector3neon& lhs, const Vector3neon& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    return lhs;
}

inline Vector3neon& operator-=(Vector3neon& lhs, const Vector3neon& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    return lhs;
}

inline Vector3neon& operator*=(Vector3neon& lhs, const Vector3neon& rhs)
{
    lhs.values[0] *= rhs.values[0];
    lhs.values[1] *= rhs.values[1];
    lhs.values[2] *= rhs.values[2];
    return lhs;
}

inline Vector3neon& operator/=(Vector3neon& lhs, const Vector3neon& rhs)
{
    lhs.values[0] /= rhs.values[0];
    lhs.values[1] /= rhs.values[1];
    lhs.values[2] /= rhs.values[2];
    return lhs;
}

inline constexpr Vector3neon operator-(const Vector3neon& lhs)
{
    return Vector3neon
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
    };
}

inline Vector3neon operator+(Vector3neon lhs, const Vector3neon& rhs){ lhs += rhs;  return lhs; }
inline Vector3neon operator-(Vector3neon lhs, const Vector3neon& rhs){ lhs -= rhs;  return lhs; }
inline Vector3neon operator*(Vector3neon lhs, const Vector3neon& rhs){ lhs *= rhs;  return lhs; }
inline Vector3neon operator/(Vector3neon lhs, const Vector3neon& rhs){ lhs /= rhs;  return lhs; }

inline Vector3neon& operator*=(Vector3neon& lhs, float rhs)
{
    lhs.values[0] *= rhs;
    lhs.values[1] *= rhs;
    lhs.values[2] *= rhs;
    return lhs;
}

inline Vector3neon& operator/=(Vector3neon& lhs, float rhs)
{
    return lhs *= 1.0f / rhs;
}

inline Vector3neon operator*(Vector3neon lhs, float rhs){ lhs *= rhs;  return lhs; }
inline Vector3neon operator/(Vector3neon lhs, float rhs){ lhs /= rhs;  return lhs; }

// ///////////////////
// Complicated Maths (single return)
// ///////////////////

inline float DotF(const Vector3neon& lhs, const Vector3neon& rhs)
{
    return
            (lhs.values[0] * rhs.values[0]) +
            (lhs.values[1] * rhs.values[1]) +
            (lhs.values[2] * rhs.values[2]) +
            (lhs.values[3] * rhs.values[3]);
}

// RAM: TODO: What does this do? Used to figure out the determinant of matrixes. Copied from btVector3
inline float TripleF(const Vector3neon& lhs, const Vector3neon& v1, const Vector3neon& v2)
{
    return
        lhs.values[0] * (v1.values[1] * v2.values[2] - v1.values[2] * v2.values[1]) +
        lhs.values[1] * (v1.values[2] * v2.values[0] - v1.values[0] * v2.values[2]) +
        lhs.values[2] * (v1.values[0] * v2.values[1] - v1.values[1] * v2.values[0]);
}

inline int AxisMax(const Vector3neon& lhs)
{
    return lhs.values[0] < lhs.values[1] ?
            (lhs.values[1] < lhs.values[2] ? 2 : 1) :
            (lhs.values[0] < lhs.values[2] ? 2 : 0);
}

inline int AxisMin(const Vector3neon& lhs)
{
    return lhs.values[0] < lhs.values[1] ?
            (lhs.values[0] < lhs.values[2] ? 0 : 2) :
            (lhs.values[1] < lhs.values[2] ? 1 : 2);
}

inline bool IsZero(const Vector3neon& lhs)
{
    return (lhs.values[0] == 0.0f) &&
           (lhs.values[1] == 0.0f) &&
           (lhs.values[2] == 0.0f);
}

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////

// RAM: TODO: vector versions of all of the above.
// Need all the single return functions to support vector return too.
// http://www.gamasutra.com/view/feature/132636/designing_fast_crossplatform_simd_.php?print=1
// (Keep Results Into SIMD Registers)
// That is, stop casting between SIMD and Float registers when it can all be kept in SIMD.
//inline Vector3neon LengthSquared(const Vector3neon& lhs);
//inline Vector3neon Length(const Vector3neon& lhs);
//inline Vector3neon Distance(const Vector3neon& lhs, const Vector3neon& rhs);
//inline Vector3neon DistanceSquared(const Vector3neon& lhs, const Vector3neon& rhs);
//// This too?
//inline Vector3neon Angle(const Vector3neon& lhs, const Vector3neon& rhs);

inline Vector3neon Dot(const Vector3neon& lhs, const Vector3neon& rhs)
{
    float dot = DotF(lhs, rhs);

    return Vector3neon
    {
        dot,
        dot,
        dot
    };
}

inline Vector3neon Dot3(const Vector3neon &lhs, const Vector3neon &v0, const Vector3neon &v1, const Vector3neon &v2)
{
    return Vector3neon
    {
        DotF(lhs, v0),
        DotF(lhs, v1),
        DotF(lhs, v2)
    };
}

inline Vector3neon Sqrt(const Vector3neon& lhs)
{
    return Vector3neon
    {
        std::sqrt(lhs.values[0]),
        std::sqrt(lhs.values[1]),
        std::sqrt(lhs.values[2])
    };
}

inline Vector3neon Absolute(const Vector3neon& lhs)
{
    return Vector3neon
    {
        std::fabs(lhs.values[0]),
        std::fabs(lhs.values[1]),
        std::fabs(lhs.values[2])
    };
}

inline Vector3neon Cross(const Vector3neon& lhs, const Vector3neon& rhs)
{
    return Vector3neon
    {
        lhs.values[1] * rhs.values[2] - lhs.values[2] * rhs.values[1],
        lhs.values[2] * rhs.values[0] - lhs.values[0] * rhs.values[2],
        lhs.values[0] * rhs.values[1] - lhs.values[1] * rhs.values[0]
    };
}

inline Vector3neon Rotate(Vector3neon lhs, const Vector3neon& wAxis, Radians rotation)
{
    // Stole this from  bullet3's btVector3

    auto o = wAxis * DotF(wAxis, lhs);
    auto _x = lhs - o;
    auto _y = Cross(wAxis, lhs);

    return (o + _x * cosf(rotation.value) + _y * sinf(rotation.value));
}

inline Vector3neon Lerp(const Vector3neon& lhs, const Vector3neon& rhs, float scale)
{
    return Vector3neon
    {
        lhs.values[0] + (rhs.values[0] - lhs.values[0]) * scale,
        lhs.values[1] + (rhs.values[1] - lhs.values[1]) * scale,
        lhs.values[2] + (rhs.values[2] - lhs.values[2]) * scale
    };
}

inline Vector3neon Max(const Vector3neon& lhs, const Vector3neon& rhs)
{
    return Vector3neon
    {
        lhs.values[0] > rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] > rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] > rhs.values[2] ? lhs.values[2] : rhs.values[2]
    };
}

inline Vector3neon Min(const Vector3neon& lhs, const Vector3neon& rhs)
{
    return Vector3neon
    {
        lhs.values[0] < rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] < rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] < rhs.values[2] ? lhs.values[2] : rhs.values[2]
    };
}

}}} // namespace

#endif // VECTOR3NEON_HPP
