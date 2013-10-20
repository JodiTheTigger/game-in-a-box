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

#ifndef MATRIX3X3CPP_HPP
#define MATRIX3X3CPP_HPP

#include "Vector3cpp.hpp"
#include "QuaternionCpp.hpp"

#define Matrix3x3cppData	Matrix3x3cppFloatData

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Matrix3x3cpp
{
    std::array<Vector3cpp, 3> values;

    constexpr Matrix3x3cpp(Vector3cpp x, Vector3cpp y, Vector3cpp z)
        : values{{x, y, z}} {}

    // row major
    constexpr Matrix3x3cpp(
                float xx, float xy, float xz,
                float yx, float yy, float yz,
                float zx, float zy, float zz)
        : Matrix3x3cpp(
              Vector3cpp(xx,xy,xz),
              Vector3cpp(yx,yy,yz),
              Vector3cpp(zx,zy,zz)) {}

    constexpr explicit Matrix3x3cpp(std::array<Vector3cpp, 3> vectors)
        : values(vectors) {}

    // Optimising compiler should ignore hte initilistaion? otherwise
    // do all the calculations in the assignment
    constexpr explicit Matrix3x3cpp(const QuaternionCpp& rotation)
        : values()
    {
        auto xx      = rotation.values[0] * rotation.values[0];
        auto xy      = rotation.values[0] * rotation.values[1];
        auto xz      = rotation.values[0] * rotation.values[2];
        auto xw      = rotation.values[0] * rotation.values[3];

        auto yy      = rotation.values[1] * rotation.values[1];
        auto yz      = rotation.values[1] * rotation.values[2];
        auto yw      = rotation.values[1] * rotation.values[3];

        auto zz      = rotation.values[2] * rotation.values[2];
        auto zw      = rotation.values[2] * rotation.values[3];

        values[0] =
        {
            1.0f - 2.0f * ( yy + zz ),
                   2.0f * ( xy - zw ),
                   2.0f * ( xz + yw )
        };

        values[0] =
        {
                    2.0f * ( xy + zw ),
             1.0f - 2.0f * ( xx + zz ),
                    2.0f * ( yz - xw ),
        };

        values[0] =
        {
                    2.0f * ( xz - yw ),
                    2.0f * ( yz + xw ),
             1.0f - 2.0f * ( xx + yy ),
        };
    }
};

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Comparison Operators
// ///////////////////

inline bool operator==(const Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Simple Maths
// ///////////////////
inline Matrix3x3cpp& operator+=(Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    return lhs;
}

inline Matrix3x3cpp& operator-=(Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    return lhs;
}

inline Matrix3x3cpp& operator*=(Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs)
{
    auto& l         = lhs.values;
    const auto& r   = rhs.values;

    auto result = Matrix3x3cpp
    {
            (l[0].values[0] * r[0].values[0]) + (l[0].values[1] * r[1].values[0]) + (l[0].values[2] * r[2].values[0]),
            (l[0].values[0] * r[0].values[1]) + (l[0].values[1] * r[1].values[1]) + (l[0].values[2] * r[2].values[1]),
            (l[0].values[0] * r[0].values[2]) + (l[0].values[1] * r[1].values[2]) + (l[0].values[2] * r[2].values[2]),

            (l[1].values[0] * r[0].values[0]) + (l[1].values[1] * r[1].values[0]) + (l[1].values[2] * r[2].values[0]),
            (l[1].values[0] * r[0].values[1]) + (l[1].values[1] * r[1].values[1]) + (l[1].values[2] * r[2].values[1]),
            (l[1].values[0] * r[0].values[2]) + (l[1].values[1] * r[1].values[2]) + (l[1].values[2] * r[2].values[2]),

            (l[2].values[0] * r[0].values[0]) + (l[2].values[1] * r[1].values[0]) + (l[2].values[2] * r[2].values[0]),
            (l[2].values[0] * r[0].values[1]) + (l[2].values[1] * r[1].values[1]) + (l[2].values[2] * r[2].values[1]),
            (l[2].values[0] * r[0].values[2]) + (l[2].values[1] * r[1].values[2]) + (l[2].values[2] * r[2].values[2]),
    };

    lhs = result;
    return lhs;
}

inline constexpr Matrix3x3cpp operator-(const Matrix3x3cpp& lhs)
{
    return Matrix3x3cpp
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
    };
}

inline Matrix3x3cpp operator+(Matrix3x3cpp lhs, const Matrix3x3cpp& rhs){ lhs += rhs;  return lhs; }
inline Matrix3x3cpp operator-(Matrix3x3cpp lhs, const Matrix3x3cpp& rhs){ lhs -= rhs;  return lhs; }
inline Matrix3x3cpp operator*(Matrix3x3cpp lhs, const Matrix3x3cpp& rhs){ lhs *= rhs;  return lhs; }

// ///////////////////
// Vector and Matrix maths.
// ///////////////////
inline Vector3cpp operator*(const Matrix3x3cpp& lhs, const Vector3cpp& rhs)
{
    return Vector3cpp
    {
            Dot(lhs.values[0], rhs),
            Dot(lhs.values[1], rhs),
            Dot(lhs.values[2], rhs),
    };
}

inline Vector3cpp operator*(const Vector3cpp& lhs, const Matrix3x3cpp& rhs)
{
    return Vector3cpp
    {
            rhs.values[0].values[0] * lhs.values[0] + rhs.values[1].values[0] * lhs.values[1] + rhs.values[2].values[0] * lhs.values[2],
            rhs.values[0].values[1] * lhs.values[0] + rhs.values[1].values[1] * lhs.values[1] + rhs.values[2].values[1] * lhs.values[2],
            rhs.values[0].values[2] * lhs.values[0] + rhs.values[1].values[2] * lhs.values[1] + rhs.values[2].values[2] * lhs.values[2],
    };
}

}}} // namespace

#endif // MATRIX3X3CPP_HPP
