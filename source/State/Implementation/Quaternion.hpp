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
// Prototypes
// ///////////////////
inline Quaternion Absolute(const Quaternion& lhs);

// ///////////////////
// Simple Maths
// ///////////////////
inline Quaternion& operator+=(Quaternion& lhs, const Quaternion& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    lhs.values[3] += rhs.values[3];
    return lhs;
}

inline Quaternion& operator-=(Quaternion& lhs, const Quaternion& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    lhs.values[3] -= rhs.values[3];
    return lhs;
}

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

inline constexpr Quaternion operator-(const Quaternion& lhs)
{
    return Quaternion
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
        -lhs.values[3]
    };
}

inline Quaternion operator+(Quaternion lhs, const Quaternion& rhs){ lhs += rhs;  return lhs; }
inline Quaternion operator-(Quaternion lhs, const Quaternion& rhs){ lhs -= rhs;  return lhs; }
inline Quaternion operator*(Quaternion lhs, const Quaternion& rhs){ lhs *= rhs;  return lhs; }

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////
inline Quaternion Normalise(const Quaternion& lhs)
{
    // Use Vector4's version.
    return Quaternion{Normalise(Vector4{lhs.values}).values};
}

}}} // namespace


#endif // QUATERNION_HPP
