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

#ifndef VECTOR3SSE2_HPP
#define VECTOR3SSE2_HPP

#include "Vector.hpp"

#include <emmintrin.h>

#include <array>
#include <cmath>
#include <limits>

// RAM: TODO: single inputs are called lhs, double, lhs and rhs. Validate.

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Vector3sse2
{
    struct tagReplicate {};

    __m128 value;

    // RAM: TODO: need to watch initlising value to 0 before an assignment.
    constexpr Vector3sse2()
        : value{_mm_setzero_ps()} {}
    constexpr Vector3sse2(float x)
        : value{_mm_set_ss(x)} {}
    constexpr Vector3sse2(float x, float y)
        : value{_mm_set_ps(x, y, 0.0f, 0.0f)} {}
    constexpr Vector3sse2(float x, float y, float z)
        : value{_mm_set_ps(x, y, z, 0.0f)} {}
    constexpr Vector3sse2(float x, float y, float z, float w)
        : value{_mm_set_ps(x, y, z, w)} {}

    constexpr Vector3sse2(Vector vector)
        : value(_mm_load_ps(vector.value.data())) {}
    constexpr Vector3sse2(const std::array<float, 4>& array)
        : value(_mm_load_ps(array.data())) {}
    constexpr Vector3sse2(float x, tagReplicate)
        : value{_mm_set_ps1(x)} {}

    constexpr Vector3sse2(__m128 raw)
        : value(raw) {}

    Vector3sse2(const Vector3sse2&) = default;
    Vector3sse2(Vector3sse2&&) = default;
    Vector3sse2& operator=(const Vector3sse2&) & = default;
    Vector3sse2& operator=(Vector3sse2&&) & = default;

    constexpr Vector ToVector() const
    {
        std::array<float, 4> result;

        _mm_store_ps(result.data(), value);

        return Vector{result};
    }
};

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Increment / Decrement
// ///////////////////
inline Vector3sse2& operator++(Vector3sse2& rhs)
{
    auto one = _mm_set_ps1(1.0f);

    rhs.value = _mm_add_ps(rhs.value, one);

    return rhs;
}

inline Vector3sse2 operator++(Vector3sse2& lhs, int)
{
   auto copy = lhs;
   ++lhs;

   return copy;
}

inline Vector3sse2& operator--(Vector3sse2& rhs)
{
    auto one = _mm_set_ps1(-1.0f);

    rhs.value = _mm_sub_ps(rhs.value, one);

    return rhs;
}

inline Vector3sse2 operator--(Vector3sse2& lhs, int)
{
    auto copy = lhs;
    --lhs;

    return copy;
}

// ///////////////////
// Comparison Operators
// ///////////////////
inline bool operator==(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    return  lhs.value==rhs.value;
}

inline bool operator!=(const Vector3sse2& lhs, const Vector3sse2& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Prototypes
// ///////////////////
inline Vector3sse2 Absolute(const Vector3sse2& lhs);

// ///////////////////
// Simple Maths
// ///////////////////
inline Vector3sse2& operator+=(Vector3sse2& lhs, const Vector3sse2& rhs)
{
    lhs.value = _mm_add_ps(lhs.value, rhs.value);
    return lhs;
}

inline Vector3sse2& operator-=(Vector3sse2& lhs, const Vector3sse2& rhs)
{
    lhs.value = _mm_sub_ps(lhs.value, rhs.value);
    return lhs;
}

inline Vector3sse2& operator*=(Vector3sse2& lhs, const Vector3sse2& rhs)
{
    lhs.value = _mm_mul_ps(lhs.value, rhs.value);
    return lhs;
}

inline Vector3sse2& operator/=(Vector3sse2& lhs, const Vector3sse2& rhs)
{
    lhs.value = _mm_div_ps(lhs.value, rhs.value);
    return lhs;
}

inline constexpr Vector3sse2 operator-(const Vector3sse2& lhs)
{
    auto negativeZero = _mm_set_ps1(-0.0);

    return {_mm_xor_ps(lhs.value, negativeZero)};
}

inline Vector3sse2 operator+(const Vector3sse2& lhs, const Vector3sse2& rhs){ return {_mm_add_ps(lhs.value, rhs.value)}; }
inline Vector3sse2 operator-(const Vector3sse2& lhs, const Vector3sse2& rhs){ return {_mm_sub_ps(lhs.value, rhs.value)}; }
inline Vector3sse2 operator*(const Vector3sse2& lhs, const Vector3sse2& rhs){ return {_mm_mul_ps(lhs.value, rhs.value)}; }
inline Vector3sse2 operator/(const Vector3sse2& lhs, const Vector3sse2& rhs){ return {_mm_div_ps(lhs.value, rhs.value)}; }

inline Vector3sse2& operator*=(Vector3sse2& lhs, float rhs)
{    
    return lhs *= {rhs, Vector3sse2::tagReplicate};
}

inline Vector3sse2& operator/=(Vector3sse2& lhs, float rhs)
{
    return lhs /= {rhs, Vector3sse2::tagReplicate};
}

inline Vector3sse2 operator*(const Vector3sse2& lhs, float rhs){ return {_mm_mul_ps(lhs.value, _mm_set_ps1(rhs))}; }
inline Vector3sse2 operator/(const Vector3sse2& lhs, float rhs){ return {_mm_div_ps(lhs.value, _mm_set_ps1(rhs))}; }

// ///////////////////
// Complicated Maths (single return)
// ///////////////////

inline float DotF(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    // http://www.gamedev.net/topic/617959-c-dot-product-vs-sse-dot-product/
    //__m128 m = _mm_mul_ps(v1, v2);
    //__m128 t = _mm_add_ps(m, _mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 3, 0, 1)));
    //__m128 result = _mm_add_ps(t, _mm_shuffle_ps(t, t, _MM_SHUFFLE(1, 0, 3, 2)));

    __m128 m = _mm_mul_ps(lhs.value, rhs.value);
    __m128 s1 = _mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 3, 0, 1);
    __m128 a1 = _mm_add_ps(m, s1);
    __m128 s2 = _mm_shuffle_ps(a1, a1, _MM_SHUFFLE(1, 0, 3, 2));
    __m128 result = _mm_add_ps(a1, s2);

    // RAM: TODO: Save to a float and return the float.
}

inline float LengthSquaredF(const Vector3sse2& lhs)
{
    return DotF(lhs, lhs);
}

inline float LengthF(const Vector3sse2& lhs)
{
    return sqrt(LengthSquaredF(lhs));
}

inline float DistanceF(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    return LengthF(lhs - rhs);
}

inline float DistanceSquaredF(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    return LengthSquaredF(lhs - rhs);
}

// RAM: TODO: Use Radians type.
inline float Angle(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    auto squaredLengths = LengthSquaredF(lhs) * LengthSquaredF(rhs);

    return acos(DotF(lhs, rhs) / squaredLengths);
}

// RAM: TODO: What does this do? Used to figure out the determinant of matrixes. Copied from btVector3
inline float TripleF(const Vector3sse2& lhs, const Vector3sse2& v1, const Vector3sse2& v2)
{
    return
        lhs.value[0] * (v1.value[1] * v2.value[2] - v1.value[2] * v2.value[1]) +
        lhs.value[1] * (v1.value[2] * v2.value[0] - v1.value[0] * v2.value[2]) +
        lhs.value[2] * (v1.value[0] * v2.value[1] - v1.value[1] * v2.value[0]);
}

inline int AxisMax(const Vector3sse2& lhs)
{
    return lhs.value[0] < lhs.value[1] ?
            (lhs.value[1] < lhs.value[2] ? 2 : 1) :
            (lhs.value[0] < lhs.value[2] ? 2 : 0);
}

inline int AxisMin(const Vector3sse2& lhs)
{
    return lhs.value[0] < lhs.value[1] ?
            (lhs.value[0] < lhs.value[2] ? 0 : 2) :
            (lhs.value[1] < lhs.value[2] ? 1 : 2);
}

inline int AxisFar(const Vector3sse2& lhs)
{
    return AxisMax(Absolute(lhs));
}

inline int AxisNear(const Vector3sse2& lhs)
{
    return AxisMin(Absolute(lhs));
}

inline bool IsZero(const Vector3sse2& lhs)
{
    return (lhs.value[0] == 0.0f) &&
           (lhs.value[1] == 0.0f) &&
           (lhs.value[2] == 0.0f);
}

inline bool IsZeroFuzzy(const Vector3sse2& lhs)
{
    return LengthSquaredF(lhs) < std::numeric_limits<float>::epsilon();
}

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////

// RAM: TODO: vector versions of all of the above.
// Need all the single return functions to support vector return too.
// http://www.gamasutra.com/view/feature/132636/designing_fast_crossplatform_simd_.php?print=1
// (Keep Results Into SIMD Registers)
// That is, stop casting between SIMD and Float registers when it can all be kept in SIMD.
//inline Vector3sse2 LengthSquared(const Vector3sse2& lhs);
//inline Vector3sse2 Length(const Vector3sse2& lhs);
//inline Vector3sse2 Distance(const Vector3sse2& lhs, const Vector3sse2& rhs);
//inline Vector3sse2 DistanceSquared(const Vector3sse2& lhs, const Vector3sse2& rhs);
//// This too?
//inline Vector3sse2 Angle(const Vector3sse2& lhs, const Vector3sse2& rhs);

inline Vector3sse2 Dot(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    float dot = DotF(lhs, rhs);

    return Vector3sse2
    {
        dot,
        dot,
        dot
    };
}

inline Vector3sse2 Dot3(const Vector3sse2 &lhs, const Vector3sse2 &v0, const Vector3sse2 &v1, const Vector3sse2 &v2)
{
    return Vector3sse2
    {
        DotF(lhs, v0),
        DotF(lhs, v1),
        DotF(lhs, v2)
    };
}

inline Vector3sse2 Absolute(const Vector3sse2& lhs)
{
    return Vector3sse2
    {
        std::fabs(lhs.value[0]),
        std::fabs(lhs.value[1]),
        std::fabs(lhs.value[2]),
        0.0f
    };
}

inline Vector3sse2 NormaliseStable(Vector3sse2 lhs)
{
    auto absolute = Absolute(lhs);
    auto maxIndex = AxisMax(absolute);

    if (absolute.value[maxIndex] > 0)
    {
        lhs /= absolute.value[maxIndex];
        return lhs / LengthF(lhs);
    }

    // Too small to actually normalise without becoming unstable.
    // So just set x to 1. Same diff.
    return Vector3sse2
    {
            1.0f
    };
}

inline Vector3sse2 Normalise(Vector3sse2 lhs)
{
    return lhs / LengthF(lhs);
}

inline Vector3sse2 Cross(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    return Vector3sse2
    {
        lhs.value[1] * rhs.value[2] - lhs.value[2] * rhs.value[1],
        lhs.value[2] * rhs.value[0] - lhs.value[0] * rhs.value[2],
        lhs.value[0] * rhs.value[1] - lhs.value[1] * rhs.value[0]
    };
}

// RAM: TODO: Use rotation in radians.
inline Vector3sse2 Rotate(Vector3sse2 lhs, const Vector3sse2& wAxis, float rotation)
{
    // Stole this from  bullet3's btVector3

    auto o = wAxis * DotF(wAxis, lhs);
    auto _x = lhs - o;
    auto _y = Cross(wAxis, lhs);

    return (o + _x * cosf(rotation) + _y * sinf(rotation));
}

inline Vector3sse2 Lerp(const Vector3sse2& lhs, const Vector3sse2& rhs, float scale)
{
    return Vector3sse2
    {
        lhs.value[0] + (rhs.value[0] - lhs.value[0]) * scale,
        lhs.value[1] + (rhs.value[1] - lhs.value[1]) * scale,
        lhs.value[2] + (rhs.value[2] - lhs.value[2]) * scale
    };
}

inline Vector3sse2 Max(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    return Vector3sse2
    {
        lhs.value[0] > rhs.value[0] ? lhs.value[0] : rhs.value[0],
        lhs.value[1] > rhs.value[1] ? lhs.value[1] : rhs.value[1],
        lhs.value[2] > rhs.value[2] ? lhs.value[2] : rhs.value[2]
    };
}

inline Vector3sse2 Min(const Vector3sse2& lhs, const Vector3sse2& rhs)
{
    return Vector3sse2
    {
        lhs.value[0] < rhs.value[0] ? lhs.value[0] : rhs.value[0],
        lhs.value[1] < rhs.value[1] ? lhs.value[1] : rhs.value[1],
        lhs.value[2] < rhs.value[2] ? lhs.value[2] : rhs.value[2]
    };
}

}}} // namespace

#endif // VECTOR3SSE2_HPP
