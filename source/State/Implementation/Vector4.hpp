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

#include "VectorPod.hpp"
#include "UnitsSI.hpp"
#include "VectorCommon.hpp"

#include <array>

// RAM: TODO: Try to use idioms?
// http://www.altdevblogaday.com/2011/12/24/beyond-intrinsics-to-code-idiom/
// Copy code pattern from SSE2 version, compare assembler output, be surprised?

namespace GameInABox { namespace State { namespace Implementation {

class alignas(16) Vector4
{
public:
    struct tagReplicate {};

    constexpr Vector4()
        : myValues{{0.0f, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector4(float x)
        : myValues{{x, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector4(float x, float y)
        : myValues{{x, y, 0.0f, 0.0f}} {}
    constexpr Vector4(float x, float y, float z)
        : myValues{{x, y, z, 0.0f}} {}
    constexpr Vector4(float x, float y, float z, float w)
        : myValues{{x, y, z, w}} {}

    constexpr Vector4(VectorPod vector)
        : myValues(vector.values) {}
    constexpr Vector4(const std::array<float, 4>& array)
        : myValues(array) {}
    constexpr Vector4(float x, tagReplicate)
        : myValues{{x, x, x, x}} {}

    Vector4(const Vector4&) = default;
    Vector4(Vector4&&) = default;
    Vector4& operator=(const Vector4&) & = default;
    Vector4& operator=(Vector4&&) & = default;

    constexpr float X() const { return myValues[0]; }
    constexpr float Y() const { return myValues[1]; }
    constexpr float Z() const { return myValues[2]; }
    constexpr float W() const { return myValues[3]; }

    constexpr VectorPod ToVectorPod() const { return VectorPod{myValues}; }

private:
    std::array<float, 4> myValues;
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
                (lhs.X()==rhs.X()) &&
                (lhs.Y()==rhs.Y()) &&
                (lhs.Z()==rhs.Z()) &&
                (lhs.W()==rhs.W())
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
    lhs.X() += rhs.X();
    lhs.Y() += rhs.Y();
    lhs.Z() += rhs.Z();
    lhs.W() += rhs.W();
    return lhs;
}

inline Vector4& operator-=(Vector4& lhs, const Vector4& rhs)
{
    lhs.X() -= rhs.X();
    lhs.Y() -= rhs.Y();
    lhs.Z() -= rhs.Z();
    lhs.W() -= rhs.W();
    return lhs;
}

inline Vector4& operator*=(Vector4& lhs, const Vector4& rhs)
{
    lhs.X() *= rhs.X();
    lhs.Y() *= rhs.Y();
    lhs.Z() *= rhs.Z();
    lhs.W() *= rhs.W();
    return lhs;
}

inline Vector4& operator/=(Vector4& lhs, const Vector4& rhs)
{
    lhs.X() /= rhs.X();
    lhs.Y() /= rhs.Y();
    lhs.Z() /= rhs.Z();
    lhs.W() /= rhs.W();
    return lhs;
}

inline constexpr Vector4 operator-(const Vector4& lhs)
{
    return Vector4
    {
        -lhs.X(),
        -lhs.Y(),
        -lhs.Z(),
        -lhs.W(),
    };
}

inline Vector4 operator+(Vector4 lhs, const Vector4& rhs){ lhs += rhs;  return lhs; }
inline Vector4 operator-(Vector4 lhs, const Vector4& rhs){ lhs -= rhs;  return lhs; }
inline Vector4 operator*(Vector4 lhs, const Vector4& rhs){ lhs *= rhs;  return lhs; }
inline Vector4 operator/(Vector4 lhs, const Vector4& rhs){ lhs /= rhs;  return lhs; }

inline Vector4& operator*=(Vector4& lhs, float rhs)
{
    lhs.X() *= rhs;
    lhs.Y() *= rhs;
    lhs.Z() *= rhs;
    lhs.W() *= rhs;
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

    if (lhs.Y() > lhs.X())
    {
       result = 1;
    }
    if (lhs.Z() > lhs.values[result])
    {
       result = 2;
    }
    if (lhs.W() > lhs.values[result])
    {
       result = 3;
    }

    return result;
}

inline int AxisMin(const Vector4& lhs)
{
    unsigned result = 0;

    if (lhs.Y() < lhs.X())
    {
       result = 1;
    }
    if (lhs.Z() < lhs.values[result])
    {
       result = 2;
    }
    if (lhs.W() < lhs.values[result])
    {
       result = 3;
    }

    return result;
}

inline bool IsZero(const Vector4& lhs)
{
    return  (lhs.X() == 0.0f) &&
            (lhs.Y() == 0.0f) &&
            (lhs.Z() == 0.0f) &&
            (lhs.W() == 0.0f);
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
        std::sqrt(lhs.X()),
        std::sqrt(lhs.Y()),
        std::sqrt(lhs.Z()),
        std::sqrt(lhs.W())
    };
}

inline Vector4 Absolute(const Vector4& lhs)
{
    return Vector4
    {
        std::fabs(lhs.X()),
        std::fabs(lhs.Y()),
        std::fabs(lhs.Z()),
        std::fabs(lhs.W())
    };
}

inline Vector4 Dot(const Vector4& lhs, const Vector4& rhs)
{
    // If this compiler is too dumb to do a decent DOT4, then do this instead:
    /*
    // http://www.gamedev.net/topic/617959-c-dot-product-vs-sse-dot-product/
    //__m128 m = _mm_mul_ps(v1, v2);
    //__m128 t = _mm_add_ps(m, _mm_shuffle_ps(m, m, _MM_SHUFFLE(2, 3, 0, 1)));
    //__m128 result = _mm_add_ps(t, _mm_shuffle_ps(t, t, _MM_SHUFFLE(1, 0, 3, 2)));

    auto multiply = lhs * rhs;
    auto shuffle1 = Vector4
    {
            multiply.Y(),
            multiply.X(),
            multiply.W(),
            multiply.Z(),
    };

    // x = x + y
    // y = y + x
    // z = z + w
    // w = w + z
    auto first = multiply + shuffle1;

    auto shuffle2 = Vector4
    {
           first.Z(),
           first.W(),
           first.X(),
           first.Y(),
    };

    // x = x + y + (z + w)
    // y = y + x + (w + z)
    // z = z + w + (x + y)
    // w = w + z + (y + x)
    return first + shuffle2;
    */

    // hope the compiler picks up on this pattern and recognises it as a Dot.
    auto mult = lhs * rhs;

    return Vector4
    {
        mult.X() + mult.Y() + mult.Z() + mult.W(),
        mult.X() + mult.Y() + mult.Z() + mult.W(),
        mult.X() + mult.Y() + mult.Z() + mult.W(),
        mult.X() + mult.Y() + mult.Z() + mult.W(),
    };
}

// Cross product doesn't exist for Vector4, only Vector3 and Vector7.

inline Vector4 Lerp(const Vector4& lhs, const Vector4& rhs, float scale)
{
    return Vector4
    {
        lhs.X() + (rhs.X() - lhs.X()) * scale,
        lhs.Y() + (rhs.Y() - lhs.Y()) * scale,
        lhs.Z() + (rhs.Z() - lhs.Z()) * scale,
        lhs.W() + (rhs.W() - lhs.W()) * scale
    };
}

inline Vector4 Max(const Vector4& lhs, const Vector4& rhs)
{
    return Vector4
    {
        lhs.X() > rhs.X() ? lhs.X() : rhs.X(),
        lhs.Y() > rhs.Y() ? lhs.Y() : rhs.Y(),
        lhs.Z() > rhs.Z() ? lhs.Z() : rhs.Z(),
        lhs.W() > rhs.W() ? lhs.W() : rhs.W()
    };
}

inline Vector4 Min(const Vector4& lhs, const Vector4& rhs)
{
    return Vector4
    {
        lhs.X() < rhs.X() ? lhs.X() : rhs.X(),
        lhs.Y() < rhs.Y() ? lhs.Y() : rhs.Y(),
        lhs.Z() < rhs.Z() ? lhs.Z() : rhs.Z(),
        lhs.W() < rhs.W() ? lhs.W() : rhs.W()
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
    return
            (lhs.X() * rhs.X()) +
            (lhs.Y() * rhs.Y()) +
            (lhs.Z() * rhs.Z()) +
            (lhs.W() * rhs.W());
}

}}} // namespace

#endif // VECTOR4_HPP
