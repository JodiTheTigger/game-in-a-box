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

#ifndef QUATERNION_HPP
#define QUATERNION_HPP

#include "Vector.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "UnitsSI.hpp"

#include <array>
#include <cmath>

namespace GameInABox { namespace State { namespace Implementation {

// NOTE: Lerping isn't all just slerp.
// http://number-none.com/product/Understanding%20Slerp,%20Then%20Not%20Using%20It/
// http://keithmaggio.wordpress.com/2011/02/15/math-magician-lerp-slerp-and-nlerp/

struct alignas(16) Quaternion
{
    std::array<float, 4> values;

    constexpr Quaternion()
        : values{{0.0f, 0.0f, 0.0f, 1.0f}} {}
    constexpr Quaternion(float x, float y, float z, float w)
        : values{{x, y, z, w}} {}

    constexpr Quaternion(Vector vector)
        : values(vector.values) {}
    constexpr Quaternion(const std::array<float, 4>& array)
        : values(array) {}

    Quaternion(const Vector3& axis, Radians angle)
       : values(Normalise(Vector4{(Quaternion(
                               axis.values[0] * sin(angle.value / 2),
                               axis.values[1] * sin(angle.value / 2),
                               axis.values[2] * sin(angle.value / 2),
                               cos(angle.value / 2))).values}).values) {}

    // From and to are unit vectors.
    // The Quaternion is the rotation between them.
    Quaternion(const Vector3& from, const Vector3& to)
        : values({0.0f, 0.0f, 0.0f, 1.0f})
    {
        // http://lolengine.net/blog/2013/09/18/beautiful-maths-quaternion-from-vectors
        // from and to are unit vectors.

        auto w = Cross(from, to);
        auto q = Vector4(w.values[0], w.values[1], w.values[2], 1.0f + DotF(to, from));

        values = Normalise(q).values;
    }

    Quaternion(const Quaternion&) = default;
    Quaternion(Quaternion&&) = default;
    Quaternion& operator=(const Quaternion&) & = default;
    Quaternion& operator=(Quaternion&&) & = default;

    constexpr Vector ToVector() const { return Vector{values}; }
};                                         

// ///////////////////
// Comparison Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.
inline bool operator==(const Quaternion& lhs, const Quaternion& rhs)
{
 return  (
             (lhs.values[0]==rhs.values[0]) &&
             (lhs.values[1]==rhs.values[1]) &&
             (lhs.values[2]==rhs.values[2])
         );
}

inline bool operator!=(const Quaternion& lhs, const Quaternion& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Simple Maths
// ///////////////////
inline Quaternion& operator*=(Quaternion& lhs, const Quaternion& rhs)
{
    lhs = Quaternion
    {
        (lhs.values[0] * rhs.values[0]) - (lhs.values[0] * rhs.values[1]) - (lhs.values[0] * rhs.values[2]) - (lhs.values[0] * rhs.values[3]),
        (lhs.values[1] * rhs.values[1]) + (lhs.values[1] * rhs.values[0]) + (lhs.values[1] * rhs.values[3]) - (lhs.values[1] * rhs.values[2]),
        (lhs.values[2] * rhs.values[2]) - (lhs.values[2] * rhs.values[3]) + (lhs.values[2] * rhs.values[0]) + (lhs.values[2] * rhs.values[1]),
        (lhs.values[3] * rhs.values[3]) + (lhs.values[3] * rhs.values[2]) - (lhs.values[3] * rhs.values[1]) + (lhs.values[3] * rhs.values[0])
    };

    return lhs;
}

inline Quaternion operator*(Quaternion lhs, const Quaternion& rhs){ lhs *= rhs;  return lhs; }

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////
inline Quaternion Normalise(const Quaternion& lhs)
{
    // Use Vector4's version.
    return Quaternion{Normalise(Vector4{lhs.values}).values};
}

// commutative          : yes
// constant velocity    : no
// torque-minimal       : yes
// Computation          : cheap
inline Quaternion NLerp(const Quaternion& lhs, const Quaternion& rhs, float scale)
{
    // Use Vector4's version.
    return Quaternion{Normalise(Lerp(Vector4{lhs.values}, Vector4{rhs.values}, scale)).values};
}

// commutative          : no
// constant velocity    : yes
// torque-minimal       : yes
// Computation          : expensive (sin + acos)
inline Quaternion SLerp(const Quaternion& lhs, const Quaternion& rhs, float scale)
{
    // Adapted from bullet3
    auto vector4lhs = Vector4{lhs.values};
    auto vector4rhs = Vector4{rhs.values};
    auto magnitude  = std::sqrt(LengthSquaredF(vector4lhs) * LengthSquaredF(vector4rhs));

    // RAM: TODO: deal with quaternions that are too close. Ie magnitude !> 0

    float product = DotF(vector4lhs, vector4rhs) / magnitude;

    if (std::fabs(product) < 1.0f)
    {
        // Take care of long angle case see http://en.wikipedia.org/wiki/Slerp
        auto sign = (product < 0) ? float(-1) : float(1);

        auto theta  = std::acos(sign * product);
        auto s1     = std::sin(sign * scale * theta);
        auto d      = 1.0f / std::sin(theta);
        auto s0     = std::sin((1.0f - scale) * theta);

        return
        {
            (lhs.values[0] * s0 + rhs.values[0] * s1) * d,
            (lhs.values[1] * s0 + rhs.values[1] * s1) * d,
            (lhs.values[2] * s0 + rhs.values[2] * s1) * d,
            (lhs.values[3] * s0 + rhs.values[3] * s1) * d
        };
    }

    return lhs;
}


}}} // namespace


#endif // QUATERNION_HPP
