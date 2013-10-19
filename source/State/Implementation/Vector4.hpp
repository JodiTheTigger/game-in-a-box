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

#ifndef VECTOR4_HPP
#define VECTOR4_HPP

#include "Vector.hpp"
#include "UnitsSI.hpp"

#include <array>
#include <cmath>
#include <limits>

// RAM: TODO: Try to use idioms?
// http://www.altdevblogaday.com/2011/12/24/beyond-intrinsics-to-code-idiom/
// Copy code pattern from SSE2 version, compare assembler output, be surprised?

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Vector4
{
    struct tagReplicate {};

    std::array<float, 4> values;

    constexpr Vector4()
        : values{{0.0f, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector4(float x)
        : values{{x, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector4(float x, float y)
        : values{{x, y, 0.0f, 0.0f}} {}
    constexpr Vector4(float x, float y, float z)
        : values{{x, y, z, 0.0f}} {}
    constexpr Vector4(float x, float y, float z, float w)
        : values{{x, y, z, w}} {}

    constexpr Vector4(Vector vector)
        : values(vector.values) {}
    constexpr Vector4(const std::array<float, 4>& array)
        : values(array) {}
    constexpr Vector4(float x, tagReplicate)
        : values{{x, x, x, x}} {}

    Vector4(const Vector4&) = default;
    Vector4(Vector4&&) = default;
    Vector4& operator=(const Vector4&) & = default;
    Vector4& operator=(Vector4&&) & = default;

    constexpr Vector ToVector() const { return Vector{values}; }
};

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Comparison Operators
// ///////////////////
inline bool operator==(const Vector4& lhs, const Vector4& rhs)
{
    // Investiage, exact equals, or epislon compare?
    return  (
                (lhs.values[0]==rhs.values[0]) &&
                (lhs.values[1]==rhs.values[1]) &&
                (lhs.values[2]==rhs.values[2]) &&
                (lhs.values[3]==rhs.values[3])
            );
}

inline bool operator!=(const Vector4& lhs, const Vector4& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Prototypes
// ///////////////////
inline Vector4 Absolute(const Vector4& lhs);

// ///////////////////
// Simple Maths
// ///////////////////
inline Vector4& operator+=(Vector4& lhs, const Vector4& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    lhs.values[3] += rhs.values[3];
    return lhs;
}

inline Vector4& operator-=(Vector4& lhs, const Vector4& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    lhs.values[3] -= rhs.values[3];
    return lhs;
}

inline Vector4& operator*=(Vector4& lhs, const Vector4& rhs)
{
    lhs.values[0] *= rhs.values[0];
    lhs.values[1] *= rhs.values[1];
    lhs.values[2] *= rhs.values[2];
    lhs.values[3] *= rhs.values[3];
    return lhs;
}

inline Vector4& operator/=(Vector4& lhs, const Vector4& rhs)
{
    lhs.values[0] /= rhs.values[0];
    lhs.values[1] /= rhs.values[1];
    lhs.values[2] /= rhs.values[2];
    lhs.values[3] /= rhs.values[3];
    return lhs;
}

inline constexpr Vector4 operator-(const Vector4& lhs)
{
    return Vector4
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
        -lhs.values[3],
    };
}

inline Vector4 operator+(Vector4 lhs, const Vector4& rhs){ lhs += rhs;  return lhs; }
inline Vector4 operator-(Vector4 lhs, const Vector4& rhs){ lhs -= rhs;  return lhs; }
inline Vector4 operator*(Vector4 lhs, const Vector4& rhs){ lhs *= rhs;  return lhs; }
inline Vector4 operator/(Vector4 lhs, const Vector4& rhs){ lhs /= rhs;  return lhs; }

inline Vector4& operator*=(Vector4& lhs, float rhs)
{
    lhs.values[0] *= rhs;
    lhs.values[1] *= rhs;
    lhs.values[2] *= rhs;
    lhs.values[3] *= rhs;
    return lhs;
}

inline Vector4& operator/=(Vector4& lhs, float rhs)
{
    return lhs *= 1.0f / rhs;
}

inline Vector4 operator*(Vector4 lhs, float rhs){ lhs *= rhs;  return lhs; }
inline Vector4 operator/(Vector4 lhs, float rhs){ lhs /= rhs;  return lhs; }


// ///////////////////
// Helper Functions
// ///////////////////

inline unsigned AxisMax(const Vector4& lhs)
{
    unsigned result = 0;

    if (lhs.values[1] > lhs.values[0])
    {
       result = 1;
    }
    if (lhs.values[2] > lhs.values[result])
    {
       result = 2;
    }
    if (lhs.values[3] > lhs.values[result])
    {
       result = 3;
    }

    return result;
}

inline int AxisMin(const Vector4& lhs)
{
    unsigned result = 0;

    if (lhs.values[1] < lhs.values[0])
    {
       result = 1;
    }
    if (lhs.values[2] < lhs.values[result])
    {
       result = 2;
    }
    if (lhs.values[3] < lhs.values[result])
    {
       result = 3;
    }

    return result;
}

inline int AxisFar(const Vector4& lhs)
{
    return AxisMax(Absolute(lhs));
}

inline int AxisNear(const Vector4& lhs)
{
    return AxisMin(Absolute(lhs));
}

inline bool IsZero(const Vector4& lhs)
{
    return  (lhs.values[0] == 0.0f) &&
            (lhs.values[1] == 0.0f) &&
            (lhs.values[2] == 0.0f) &&
            (lhs.values[3] == 0.0f);
}

inline bool IsZeroFuzzy(const Vector4& lhs)
{
    // LengthSquaredF(lhs) < xxx
    return (lhs * lhs).values[0] < std::numeric_limits<float>::epsilon();
}

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////

// RAM: TODO: vector versions of all of the above.
// Need all the single return functions to support vector return too.
// http://www.gamasutra.com/view/feature/132636/designing_fast_crossplatform_simd_.php?print=1
// (Keep Results Into SIMD Registers)
// That is, stop casting between SIMD and Float registers when it can all be kept in SIMD.

inline Vector4 Sqrt(const Vector4& lhs)
{
    return Vector4
    {
        std::sqrt(lhs.values[0]),
        std::sqrt(lhs.values[1]),
        std::sqrt(lhs.values[2]),
        std::sqrt(lhs.values[3])
    };
}

inline Vector4 Absolute(const Vector4& lhs)
{
    return Vector4
    {
        std::fabs(lhs.values[0]),
        std::fabs(lhs.values[1]),
        std::fabs(lhs.values[2]),
        std::fabs(lhs.values[3])
    };
}

inline Vector4 Dot(const Vector4& lhs, const Vector4& rhs)
{
    // http://www.gamedev.net/topic/617959-c-dot-product-vs-sse-dot-product/
    //__m128 m = _mm_mul_ps(v1, v2);
    //__m128 t = _mm_add_ps(m, _mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 3, 0, 1)));
    //__m128 result = _mm_add_ps(t, _mm_shuffle_ps(t, t, _MM_SHUFFLE(1, 0, 3, 2)));

    auto multiply = lhs * rhs;
    auto shuffle1 = Vector4
    {
            multiply.values[1],
            multiply.values[0],
            multiply.values[3],
            multiply.values[2],
    };

    // x = x + y
    // y = y + x
    // z = z + w
    // w = w + z
    auto first = multiply + shuffle1;

    auto shuffle2 = Vector4
    {
           first.values[2],
           first.values[3],
           first.values[0],
           first.values[1],
    };

    // x = x + y + (z + w)
    // y = y + x + (w + z)
    // z = z + w + (x + y)
    // w = w + z + (y + x)
    return first + shuffle2;
}

inline Vector4 LengthSquared(const Vector4& lhs)
{
    return Dot(lhs, lhs);
}

inline Vector4 Length(const Vector4& lhs)
{
    return Sqrt(LengthSquared(lhs));
}

inline Vector4 Distance(const Vector4& lhs, const Vector4& rhs)
{
    return Length(lhs - rhs);
}

inline Vector4 DistanceSquared(const Vector4& lhs, const Vector4& rhs)
{
    return LengthSquared(lhs - rhs);
}

inline Vector4 NormaliseStable(Vector4 lhs)
{
    auto absolute = Absolute(lhs);
    auto maxIndex = AxisMax(absolute);

    if (absolute.values[maxIndex] > 0)
    {
        lhs /= absolute.values[maxIndex];
        return lhs / Length(lhs);
    }

    // Too small to actually normalise without becoming unstable.
    // So just set the maxindex to 1. Same diff.
    return Vector4{}.values[maxIndex] = 1.0f;
}

inline Vector4 Normalise(Vector4 lhs)
{
    return lhs / Length(lhs);
}

// Cross product doesn't exist for Vector4, only Vector3 and Vector7.

inline Vector4 Lerp(const Vector4& lhs, const Vector4& rhs, float scale)
{
    return Vector4
    {
        lhs.values[0] + (rhs.values[0] - lhs.values[0]) * scale,
        lhs.values[1] + (rhs.values[1] - lhs.values[1]) * scale,
        lhs.values[2] + (rhs.values[2] - lhs.values[2]) * scale,
        lhs.values[3] + (rhs.values[3] - lhs.values[3]) * scale
    };
}

inline Vector4 Max(const Vector4& lhs, const Vector4& rhs)
{
    return Vector4
    {
        lhs.values[0] > rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] > rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] > rhs.values[2] ? lhs.values[2] : rhs.values[2],
        lhs.values[3] > rhs.values[3] ? lhs.values[3] : rhs.values[3]
    };
}

inline Vector4 Min(const Vector4& lhs, const Vector4& rhs)
{
    return Vector4
    {
        lhs.values[0] < rhs.values[0] ? lhs.values[0] : rhs.values[0],
        lhs.values[1] < rhs.values[1] ? lhs.values[1] : rhs.values[1],
        lhs.values[2] < rhs.values[2] ? lhs.values[2] : rhs.values[2],
        lhs.values[3] < rhs.values[3] ? lhs.values[3] : rhs.values[3]
    };
}

// ///////////////////
// Complicated Maths (single return)
// ///////////////////

// Avoid these as they convert from Vectors to floats which
// apparently is a performance penalty, especially if you then
// use the value in more vector calculations.

inline float DotF(const Vector4& lhs, const Vector4& rhs)
{
    return (lhs * rhs).values[0];
}

inline float LengthSquaredF(const Vector4& lhs)
{
    return (lhs * lhs).values[0];
}

inline float LengthF(const Vector4& lhs)
{
    return std::sqrt((lhs * lhs).values[0]);
}

inline float DistanceF(const Vector4& lhs, const Vector4& rhs)
{
    return LengthF(lhs - rhs);
}

inline float DistanceSquaredF(const Vector4& lhs, const Vector4& rhs)
{
    return LengthSquaredF(lhs - rhs);
}

inline Radians AngleF(const Vector4& lhs, const Vector4& rhs)
{
    auto squaredLengths = lhs * lhs * rhs * rhs;
    auto thingToACos = Dot(lhs, rhs) / squaredLengths;

    return {std::acos(thingToACos.values[0])};
}

}}} // namespace

#endif // VECTOR4_HPP
