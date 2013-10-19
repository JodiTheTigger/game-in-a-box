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

#ifndef VECTOR3CPP_HPP
#define VECTOR3CPP_HPP

#include "Vector.hpp"
#include "UnitsSI.hpp"
#include "VectorCommon.hpp"

#include <array>
#include <cmath>

// RAM: TODO: single inputs are called lhs, double, lhs and rhs. Validate.

// RAM: TODO: Try to use idioms?
// http://www.altdevblogaday.com/2011/12/24/beyond-intrinsics-to-code-idiom/
// Copy code pattern from SSE2 version, compare assembler output, be surprised?

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Vector3cpp
{
    struct tagReplicate {};

    std::array<float, 4> values;

    constexpr Vector3cpp()
        : values{{0.0f, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector3cpp(float x)
        : values{{x, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector3cpp(float x, float y)
        : values{{x, y, 0.0f, 0.0f}} {}
    constexpr Vector3cpp(float x, float y, float z)
        : values{{x, y, z, 0.0f}} {}

    constexpr Vector3cpp(Vector vector)
        : values(vector.values) {}
    constexpr Vector3cpp(const std::array<float, 4>& array)
        : values(array) {}
    constexpr Vector3cpp(float x, tagReplicate)
        : values{{x, x, x, x}} {}

    Vector3cpp(const Vector3cpp&) = default;
    Vector3cpp(Vector3cpp&&) = default;
    Vector3cpp& operator=(const Vector3cpp&) & = default;
    Vector3cpp& operator=(Vector3cpp&&) & = default;

    constexpr Vector ToVector() const { return Vector{values}; }
};

// ///////////////////
// Comparison Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.
inline bool operator==(const Vector3cpp& lhs, const Vector3cpp& rhs)
{
    return  (
                (lhs.values[0]==rhs.values[0]) &&
                (lhs.values[1]==rhs.values[1]) &&
                (lhs.values[2]==rhs.values[2])
            );
}

inline bool operator!=(const Vector3cpp& lhs, const Vector3cpp& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Prototypes
// ///////////////////
inline Vector3cpp Absolute(const Vector3cpp& lhs);

// ///////////////////
// Simple Maths
// ///////////////////
inline Vector3cpp& operator+=(Vector3cpp& lhs, const Vector3cpp& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    return lhs;
}

inline Vector3cpp& operator-=(Vector3cpp& lhs, const Vector3cpp& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    return lhs;
}

inline Vector3cpp& operator*=(Vector3cpp& lhs, const Vector3cpp& rhs)
{
    lhs.values[0] *= rhs.values[0];
    lhs.values[1] *= rhs.values[1];
    lhs.values[2] *= rhs.values[2];
    return lhs;
}

inline Vector3cpp& operator/=(Vector3cpp& lhs, const Vector3cpp& rhs)
{
    lhs.values[0] /= rhs.values[0];
    lhs.values[1] /= rhs.values[1];
    lhs.values[2] /= rhs.values[2];
    return lhs;
}

inline constexpr Vector3cpp operator-(const Vector3cpp& lhs)
{
    return Vector3cpp
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2]
    };
}

inline Vector3cpp operator+(Vector3cpp lhs, const Vector3cpp& rhs){ lhs += rhs;  return lhs; }
inline Vector3cpp operator-(Vector3cpp lhs, const Vector3cpp& rhs){ lhs -= rhs;  return lhs; }
inline Vector3cpp operator*(Vector3cpp lhs, const Vector3cpp& rhs){ lhs *= rhs;  return lhs; }
inline Vector3cpp operator/(Vector3cpp lhs, const Vector3cpp& rhs){ lhs /= rhs;  return lhs; }

inline Vector3cpp& operator*=(Vector3cpp& lhs, float rhs)
{
    lhs.values[0] *= rhs;
    lhs.values[1] *= rhs;
    lhs.values[2] *= rhs;
    return lhs;
}

inline Vector3cpp& operator/=(Vector3cpp& lhs, float rhs)
{
    return lhs *= 1.0f / rhs;
}

inline Vector3cpp operator*(Vector3cpp lhs, float rhs){ lhs *= rhs;  return lhs; }
inline Vector3cpp operator/(Vector3cpp lhs, float rhs){ lhs /= rhs;  return lhs; }

// ///////////////////
// Complicated Maths (single return)
// ///////////////////

inline float DotF(const Vector3cpp& lhs, const Vector3cpp& rhs)
{
    return
            (lhs.values[0] * rhs.values[0]) +
            (lhs.values[1] * rhs.values[1]) +
            (lhs.values[2] * rhs.values[2]);
}

// RAM: TODO: What does this do? Used to figure out the determinant of matrixes. Copied from btVector3
inline float TripleF(const Vector3cpp& lhs, const Vector3cpp& v1, const Vector3cpp& v2)
{
    return
        lhs.values[0] * (v1.values[1] * v2.values[2] - v1.values[2] * v2.values[1]) +
        lhs.values[1] * (v1.values[2] * v2.values[0] - v1.values[0] * v2.values[2]) +
        lhs.values[2] * (v1.values[0] * v2.values[1] - v1.values[1] * v2.values[0]);
}

inline int AxisMax(const Vector3cpp& lhs)
{
    return lhs.values[0] < lhs.values[1] ?
            (lhs.values[1] < lhs.values[2] ? 2 : 1) :
            (lhs.values[0] < lhs.values[2] ? 2 : 0);
}

inline int AxisMin(const Vector3cpp& lhs)
{
    return lhs.values[0] < lhs.values[1] ?
            (lhs.values[0] < lhs.values[2] ? 0 : 2) :
            (lhs.values[1] < lhs.values[2] ? 1 : 2);
}

inline int AxisFar(const Vector3cpp& lhs)
{
    return AxisMax(Absolute(lhs));
}

inline int AxisNear(const Vector3cpp& lhs)
{
    return AxisMin(Absolute(lhs));
}

inline bool IsZero(const Vector3cpp& lhs)
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

inline Vector3cpp Sqrt(const Vector3cpp& lhs)
{
    return Vector3cpp
    {
        std::sqrt(lhs.values[0]),
        std::sqrt(lhs.values[1]),
        std::sqrt(lhs.values[2])
    };
}

inline Vector3cpp Absolute(const Vector3cpp& lhs)
{
    return Vector3cpp
    {
        std::fabs(lhs.values[0]),
        std::fabs(lhs.values[1]),
        std::fabs(lhs.values[2])
    };
}

inline Vector3cpp Dot(const Vector3cpp& lhs, const Vector3cpp& rhs)
{
    // If this compiler is too dumb to do a decent DOT4, then use the Vector4 version instead.
    float dot = DotF(lhs, rhs);

    return Vector3cpp
    {
        dot,
        dot,
        dot
    };
}

inline Vector3cpp Dot3(const Vector3cpp &lhs, const Vector3cpp &v0, const Vector3cpp &v1, const Vector3cpp &v2)
{
    return Vector3cpp
    {
        Dot(lhs, v0).values[0],
        Dot(lhs, v1).values[0],
        Dot(lhs, v2).values[0]
    };
}

inline Vector3cpp Cross(const Vector3cpp& lhs, const Vector3cpp& rhs)
{
    return Vector3cpp
    {
        lhs.values[1] * rhs.values[2] - lhs.values[2] * rhs.values[1],
        lhs.values[2] * rhs.values[0] - lhs.values[0] * rhs.values[2],
        lhs.values[0] * rhs.values[1] - lhs.values[1] * rhs.values[0]
    };
}

// RAM: TODO: Use rotation in radians.
inline Vector3cpp Rotate(Vector3cpp lhs, const Vector3cpp& wAxis, Radians rotation)
{
    // Stole this from  bullet3's btVector3

    auto o = wAxis * DotF(wAxis, lhs);
    auto _x = lhs - o;
    auto _y = Cross(wAxis, lhs);

    return (o + _x * cosf(rotation.value) + _y * sinf(rotation.value));
}

inline Vector3cpp Lerp(const Vector3cpp& lhs, const Vector3cpp& rhs, float scale)
{
    return Vector3cpp
    {
        lhs.values[0] + (rhs.values[0] - lhs.values[0]) * scale,
        lhs.values[1] + (rhs.values[1] - lhs.values[1]) * scale,
        lhs.values[2] + (rhs.values[2] - lhs.values[2]) * scale
    };
}

inline Vector3cpp Max(const Vector3cpp& lhs, const Vector3cpp& rhs)
{
    return Vector3cpp
    {
        lhs.values[0] > rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] > rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] > rhs.values[2] ? lhs.values[2] : rhs.values[2]
    };
}

inline Vector3cpp Min(const Vector3cpp& lhs, const Vector3cpp& rhs)
{
    return Vector3cpp
    {
        lhs.values[0] < rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] < rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] < rhs.values[2] ? lhs.values[2] : rhs.values[2]
    };
}

}}} // namespace

#endif // VECTOR3CPP_HPP
