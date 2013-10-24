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

#include "VectorPod.hpp"
#include "UnitsSI.hpp"
#include "VectorCommon.hpp"

#include <array>
#include <cmath>

// Idioms create good enough vector code with modern compilers (gcc 4.8+, clang 3.3+)
// http://www.altdevblogaday.com/2011/12/24/beyond-intrinsics-to-code-idiom/

namespace GameInABox { namespace State { namespace Implementation {

class alignas(16) Vector3
{
public:
    struct tagReplicate {};

    constexpr Vector3()
        : myValues{{0.0f, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector3(float x)
        : myValues{{x, 0.0f, 0.0f, 0.0f}} {}
    constexpr Vector3(float x, float y)
        : myValues{{x, y, 0.0f, 0.0f}} {}
    constexpr Vector3(float x, float y, float z)
        : myValues{{x, y, z, 0.0f}} {}

    constexpr Vector3(VectorPod vector)
        : myValues(vector.values) {}
    constexpr Vector3(const std::array<float, 4>& array)
        : myValues(array) {}
    constexpr Vector3(float x, tagReplicate)
        : myValues{{x, x, x, x}} {}

    Vector3(const Vector3&) = default;
    Vector3(Vector3&&) = default;
    Vector3& operator=(const Vector3&) & = default;
    Vector3& operator=(Vector3&&) & = default;

    constexpr float X() const { return myValues[0]; }
    constexpr float Y() const { return myValues[1]; }
    constexpr float Z() const { return myValues[2]; }

    Vector3& operator+=(const Vector3& rhs)
    {
        myValues[0] += rhs.myValues[0];
        myValues[1] += rhs.myValues[1];
        myValues[2] += rhs.myValues[2];
        return *this;
    }

    Vector3& operator-=(const Vector3& rhs)
    {
        myValues[0] -= rhs.myValues[0];
        myValues[1] -= rhs.myValues[1];
        myValues[2] -= rhs.myValues[2];
        return *this;
    }

    Vector3& operator*=(const Vector3& rhs)
    {
        myValues[0] *= rhs.myValues[0];
        myValues[1] *= rhs.myValues[1];
        myValues[2] *= rhs.myValues[2];
        return *this;
    }

    Vector3& operator*=(float rhs)
    {
        myValues[0] *= rhs;
        myValues[1] *= rhs;
        myValues[2] *= rhs;
        return *this;
    }

    Vector3& operator/=(const Vector3& rhs)
    {
        myValues[0] /= rhs.myValues[0];
        myValues[1] /= rhs.myValues[1];
        myValues[2] /= rhs.myValues[2];
        return *this;
    }

    Vector3& operator/=(float rhs)
    {
        return (*this *= (1.0f / rhs));
    }

    constexpr VectorPod ToVectorPod() const { return VectorPod{myValues}; }

private:
    std::array<float, 4> myValues;
};

// ///////////////////
// Comparison Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
inline bool operator==(const Vector3& lhs, const Vector3& rhs)
{
    return  (
                (lhs.X()==rhs.X()) &&
                (lhs.Y()==rhs.Y()) &&
                (lhs.Z()==rhs.Z())
            );
}

inline bool operator!=(const Vector3& lhs, const Vector3& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Prototypes
// ///////////////////
inline Vector3 Absolute(const Vector3& lhs);

// ///////////////////
// Simple Maths
// ///////////////////
inline constexpr Vector3 operator-(const Vector3& lhs)
{
    return Vector3
    {
        -lhs.X(),
        -lhs.Y(),
        -lhs.Z()
    };
}

inline Vector3 operator+(Vector3 lhs, const Vector3& rhs){ lhs += rhs;  return lhs; }
inline Vector3 operator-(Vector3 lhs, const Vector3& rhs){ lhs -= rhs;  return lhs; }
inline Vector3 operator*(Vector3 lhs, const Vector3& rhs){ lhs *= rhs;  return lhs; }
inline Vector3 operator/(Vector3 lhs, const Vector3& rhs){ lhs /= rhs;  return lhs; }

inline Vector3 operator*(Vector3 lhs, float rhs){ lhs *= rhs;  return lhs; }
inline Vector3 operator/(Vector3 lhs, float rhs){ lhs /= rhs;  return lhs; }

// ///////////////////
// Complicated Maths (single return)
// ///////////////////

inline float DotF(const Vector3& lhs, const Vector3& rhs)
{
    return
            (lhs.X() * rhs.X()) +
            (lhs.Y() * rhs.Y()) +
            (lhs.Z() * rhs.Z());
}

// RAM: TODO: What does this do? Used to figure out the determinant of matrixes. Copied from btVector3
inline float TripleF(const Vector3& lhs, const Vector3& v1, const Vector3& v2)
{
    return
        lhs.X() * (v1.Y() * v2.Z() - v1.Z() * v2.Y()) +
        lhs.Y() * (v1.Z() * v2.X() - v1.X() * v2.Z()) +
        lhs.Z() * (v1.X() * v2.Y() - v1.Y() * v2.X());
}

inline int AxisMax(const Vector3& lhs)
{
    return lhs.X() < lhs.Y() ?
            (lhs.Y() < lhs.Z() ? 2 : 1) :
            (lhs.X() < lhs.Z() ? 2 : 0);
}

inline int AxisMin(const Vector3& lhs)
{
    return lhs.X() < lhs.Y() ?
            (lhs.X() < lhs.Z() ? 0 : 2) :
            (lhs.Y() < lhs.Z() ? 1 : 2);
}

inline int AxisFar(const Vector3& lhs)
{
    return AxisMax(Absolute(lhs));
}

inline int AxisNear(const Vector3& lhs)
{
    return AxisMin(Absolute(lhs));
}

inline bool IsZero(const Vector3& lhs)
{
    return (lhs.X() == 0.0f) &&
           (lhs.Y() == 0.0f) &&
           (lhs.Z() == 0.0f);
}

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////

// RAM: TODO: vector versions of all of the above.
// Need all the single return functions to support vector return too.
// http://www.gamasutra.com/view/feature/132636/designing_fast_crossplatform_simd_.php?print=1
// (Keep Results Into SIMD Registers)
// That is, stop casting between SIMD and Float registers when it can all be kept in SIMD.

inline Vector3 Sqrt(const Vector3& lhs)
{
    return Vector3
    {
        std::sqrt(lhs.X()),
        std::sqrt(lhs.Y()),
        std::sqrt(lhs.Z())
    };
}

inline Vector3 Absolute(const Vector3& lhs)
{
    return Vector3
    {
        std::fabs(lhs.X()),
        std::fabs(lhs.Y()),
        std::fabs(lhs.Z())
    };
}

inline Vector3 Dot(const Vector3& lhs, const Vector3& rhs)
{
    // If this compiler is too dumb to do a decent DOT4, then use the Vector4 version instead.
    float dot = DotF(lhs, rhs);

    return Vector3
    {
        dot,
        dot,
        dot
    };
}

inline Vector3 Cross(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3
    {
        lhs.Y() * rhs.Z() - lhs.Z() * rhs.Y(),
        lhs.Z() * rhs.X() - lhs.X() * rhs.Z(),
        lhs.X() * rhs.Y() - lhs.Y() * rhs.X()
    };
}

inline Vector3 Rotate(Vector3 lhs, const Vector3& wAxis, Radians rotation)
{
    // Stole this from  bullet3's btVector3

    auto o = wAxis * Dot(wAxis, lhs);
    auto _x = lhs - o;
    auto _y = Cross(wAxis, lhs);

    return (o + _x * cosf(rotation.value) + _y * sinf(rotation.value));
}

inline Vector3 Lerp(const Vector3& lhs, const Vector3& rhs, float scale)
{
    return Vector3
    {
        lhs.X() + (rhs.X() - lhs.X()) * scale,
        lhs.Y() + (rhs.Y() - lhs.Y()) * scale,
        lhs.Z() + (rhs.Z() - lhs.Z()) * scale
    };
}

inline Vector3 Max(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3
    {
        lhs.X() > rhs.X() ? lhs.X() : rhs.X(),
        lhs.Y() > rhs.Y() ? lhs.Y() : rhs.Y(),
        lhs.Z() > rhs.Z() ? lhs.Z() : rhs.Z()
    };
}

inline Vector3 Min(const Vector3& lhs, const Vector3& rhs)
{
    return Vector3
    {
        lhs.X() < rhs.X() ? lhs.X() : rhs.X(),
        lhs.Y() < rhs.Y() ? lhs.Y() : rhs.Y(),
        lhs.Z() < rhs.Z() ? lhs.Z() : rhs.Z()
    };
}

}}} // namespace

#endif // VECTOR3_HPP
