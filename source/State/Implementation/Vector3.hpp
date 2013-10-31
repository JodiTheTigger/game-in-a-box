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

#ifndef VECTOR3_HPP
#define VECTOR3_HPP

#include "UnitsSI.hpp"
#include "VectorCommon.hpp"

#include <array>
#include <cmath>

// Use idioms to generate vectorised code on modern compilers (gcc 4.8+, clang 3.3+).
// http://www.altdevblogaday.com/2011/12/24/beyond-intrinsics-to-code-idiom/

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Vector3
{
    std::array<float, 4> values;

    float X() const { return values[0]; }
    float Y() const { return values[1]; }
    float Z() const { return values[2]; }
};

// ///////////////////
// Testing
// ///////////////////
static_assert(std::is_pod<Vector3>::value, "Vector3 is not a plain old data structure (POD).");
static_assert(alignof(Vector3) == 16, "Vector3 is not aligned to a 16 byte boundary.");

// ///////////////////
// Comparison Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.
inline constexpr bool operator==(const Vector3& lhs, const Vector3& rhs)
{
    return  (
                (lhs.values[0]==rhs.values[0]) &&
                (lhs.values[1]==rhs.values[1]) &&
                (lhs.values[2]==rhs.values[2])
            );
}

inline constexpr bool operator!=(const Vector3& lhs, const Vector3& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Prototypes
// ///////////////////
inline constexpr Vector3 Absolute(const Vector3& lhs);

// ///////////////////
// Helper Functions
// ///////////////////
inline constexpr bool IsZero(const Vector3& lhs)
{
    return  (lhs.values[0] == 0.0f) &&
            (lhs.values[1] == 0.0f) &&
            (lhs.values[2] == 0.0f);
}

// ///////////////////
// Simple Maths
// ///////////////////
inline Vector3& operator+=(Vector3& lhs, const Vector3& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    return lhs;
}

inline Vector3& operator-=(Vector3& lhs, const Vector3& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    return lhs;
}

inline Vector3& operator*=(Vector3& lhs, const Vector3& rhs)
{
    lhs.values[0] *= rhs.values[0];
    lhs.values[1] *= rhs.values[1];
    lhs.values[2] *= rhs.values[2];
    return lhs;
}

inline Vector3& operator/=(Vector3& lhs, const Vector3& rhs)
{
    lhs.values[0] /= rhs.values[0];
    lhs.values[1] /= rhs.values[1];
    lhs.values[2] /= rhs.values[2];
    return lhs;
}

inline constexpr Vector3 operator-(const Vector3& lhs)
{
    return Vector3
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2]
    };
}

inline Vector3 operator+(Vector3 lhs, const Vector3& rhs){ lhs += rhs;  return lhs; }
inline Vector3 operator-(Vector3 lhs, const Vector3& rhs){ lhs -= rhs;  return lhs; }
inline Vector3 operator*(Vector3 lhs, const Vector3& rhs){ lhs *= rhs;  return lhs; }
inline Vector3 operator/(Vector3 lhs, const Vector3& rhs){ lhs /= rhs;  return lhs; }

inline Vector3& operator*=(Vector3& lhs, float rhs)
{
    lhs.values[0] *= rhs;
    lhs.values[1] *= rhs;
    lhs.values[2] *= rhs;
    return lhs;
}

inline Vector3& operator/=(Vector3& lhs, float rhs)
{
    return lhs *= 1.0f / rhs;
}

inline Vector3 operator*(Vector3 lhs, float rhs){ lhs *= rhs;  return lhs; }
inline Vector3 operator/(Vector3 lhs, float rhs){ lhs /= rhs;  return lhs; }

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////
inline constexpr Vector3 Sqrt(const Vector3& lhs)
{
    return Vector3
    {
        std::sqrt(lhs.values[0]),
        std::sqrt(lhs.values[1]),
        std::sqrt(lhs.values[2])
    };
}

inline constexpr Vector3 Absolute(const Vector3& lhs)
{
    return Vector3
    {
        std::fabs(lhs.values[0]),
        std::fabs(lhs.values[1]),
        std::fabs(lhs.values[2])
    };
}

inline Vector3 Dot(const Vector3& lhs, const Vector3& rhs)
{
    // If this compiler is too dumb to do a decent DOT4, then use the Vector4 version instead.
    auto mult = lhs * rhs;

    return Vector3
    {
        mult.values[0] + mult.values[1] + mult.values[2] + mult.values[3],
        mult.values[0] + mult.values[1] + mult.values[2] + mult.values[3],
        mult.values[0] + mult.values[1] + mult.values[2] + mult.values[3],
    };
}

inline constexpr Vector3 Cross(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3
    {
        lhs.values[1] * rhs.values[2] - lhs.values[2] * rhs.values[1],
        lhs.values[2] * rhs.values[0] - lhs.values[0] * rhs.values[2],
        lhs.values[0] * rhs.values[1] - lhs.values[1] * rhs.values[0]
    };
}

inline Vector3 Rotate(Vector3 lhs, const Vector3& wAxis, Radians rotation)
{
    // Stole this from  bullet3's btVector3

    auto o = wAxis * Dot(wAxis, lhs);
    auto _x = lhs - o;
    auto _y = Cross(wAxis, lhs);

    return (o + _x * cosf(rotation.value()) + _y * sinf(rotation.value()));
}

inline constexpr Vector3 Lerp(const Vector3& lhs, const Vector3& rhs, float scale)
{
    return Vector3
    {
        lhs.values[0] + (rhs.values[0] - lhs.values[0]) * scale,
        lhs.values[1] + (rhs.values[1] - lhs.values[1]) * scale,
        lhs.values[2] + (rhs.values[2] - lhs.values[2]) * scale
    };
}

inline constexpr Vector3 Max(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3
    {
        lhs.values[0] > rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] > rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] > rhs.values[2] ? lhs.values[2] : rhs.values[2]
    };
}

inline constexpr Vector3 Min(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3
    {
        lhs.values[0] < rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] < rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] < rhs.values[2] ? lhs.values[2] : rhs.values[2]
    };
}

// ///////////////////
// Complicated Maths (single return)
// ///////////////////
inline constexpr float DotF(const Vector3& lhs, const Vector3& rhs)
{
    return
         (lhs.values[0] * rhs.values[0]) +
         (lhs.values[1] * rhs.values[1]) +
         (lhs.values[2] * rhs.values[2]);
}

}}} // namespace

#endif // VECTOR3_HPP
