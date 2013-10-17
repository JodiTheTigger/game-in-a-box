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

#ifndef QUATERNIONCPP_HPP
#define QUATERNIONCPP_HPP

#include "Vector.hpp"
#include "Vector3cpp.hpp"
#include "Vector4.hpp"

#include <array>
#include <cmath>

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) QuaternionCpp
{
    std::array<float, 4> values;

    constexpr QuaternionCpp()
        : values{{0.0f, 0.0f, 0.0f, 1.0f}} {}
    constexpr QuaternionCpp(float x, float y, float z, float w)
        : values{{x, y, z, w}} {}

    constexpr QuaternionCpp(Vector vector)
        : values(vector.values) {}
    constexpr QuaternionCpp(const std::array<float, 4>& array)
        : values(array) {}

    // RAM: TODO: Angle in radians.
    QuaternionCpp(const Vector3cpp& axis, float angle);

    QuaternionCpp(const QuaternionCpp&) = default;
    QuaternionCpp(QuaternionCpp&&) = default;
    QuaternionCpp& operator=(const QuaternionCpp&) & = default;
    QuaternionCpp& operator=(QuaternionCpp&&) & = default;

    constexpr Vector ToVector() const { return Vector{values}; }
};                                         

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Increment / Decrement
// ///////////////////
inline QuaternionCpp& operator++(QuaternionCpp& rhs)
{
 ++(rhs.values[0]);
 ++(rhs.values[1]);
 ++(rhs.values[2]);

 return rhs;
}

inline QuaternionCpp operator++(QuaternionCpp& lhs, int)
{
auto copy = lhs;
++lhs;

return copy;
}

inline QuaternionCpp& operator--(QuaternionCpp& rhs)
{
 --(rhs.values[0]);
 --(rhs.values[1]);
 --(rhs.values[2]);

 return rhs;
}

inline QuaternionCpp operator--(QuaternionCpp& lhs, int)
{
 auto copy = lhs;
 --lhs;

 return copy;
}

// ///////////////////
// Comparison Operators
// ///////////////////
inline bool operator==(const QuaternionCpp& lhs, const QuaternionCpp& rhs)
{
 return  (
             (lhs.values[0]==rhs.values[0]) &&
             (lhs.values[1]==rhs.values[1]) &&
             (lhs.values[2]==rhs.values[2])
         );
}

inline bool operator!=(const QuaternionCpp& lhs, const QuaternionCpp& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Prototypes
// ///////////////////
inline QuaternionCpp Absolute(const QuaternionCpp& lhs);

// ///////////////////
// Simple Maths
// ///////////////////
inline QuaternionCpp& operator+=(QuaternionCpp& lhs, const QuaternionCpp& rhs)
{
 lhs.values[0] += rhs.values[0];
 lhs.values[1] += rhs.values[1];
 lhs.values[2] += rhs.values[2];
 return lhs;
}

inline QuaternionCpp& operator-=(QuaternionCpp& lhs, const QuaternionCpp& rhs)
{
 lhs.values[0] -= rhs.values[0];
 lhs.values[1] -= rhs.values[1];
 lhs.values[2] -= rhs.values[2];
 return lhs;
}

inline QuaternionCpp& operator*=(QuaternionCpp& lhs, const QuaternionCpp& rhs)
{
 lhs.values[0] *= rhs.values[0];
 lhs.values[1] *= rhs.values[1];
 lhs.values[2] *= rhs.values[2];
 return lhs;
}

inline QuaternionCpp& operator/=(QuaternionCpp& lhs, const QuaternionCpp& rhs)
{
 lhs.values[0] /= rhs.values[0];
 lhs.values[1] /= rhs.values[1];
 lhs.values[2] /= rhs.values[2];
 return lhs;
}

inline constexpr QuaternionCpp operator-(const QuaternionCpp& lhs)
{
 return QuaternionCpp
 {
     -lhs.values[0],
     -lhs.values[1],
     -lhs.values[2],
     0.0f
 };
}

inline QuaternionCpp operator+(QuaternionCpp lhs, const QuaternionCpp& rhs){ lhs += rhs;  return lhs; }
inline QuaternionCpp operator-(QuaternionCpp lhs, const QuaternionCpp& rhs){ lhs -= rhs;  return lhs; }
inline QuaternionCpp operator*(QuaternionCpp lhs, const QuaternionCpp& rhs){ lhs *= rhs;  return lhs; }
inline QuaternionCpp operator/(QuaternionCpp lhs, const QuaternionCpp& rhs){ lhs /= rhs;  return lhs; }

inline QuaternionCpp& operator*=(QuaternionCpp& lhs, float rhs)
{
 lhs.values[0] *= rhs;
 lhs.values[1] *= rhs;
 lhs.values[2] *= rhs;
 return lhs;
}

inline QuaternionCpp& operator/=(QuaternionCpp& lhs, float rhs)
{
    return lhs *= 1.0f / rhs;
}

inline QuaternionCpp operator*(QuaternionCpp lhs, float rhs){ lhs *= rhs;  return lhs; }
inline QuaternionCpp operator/(QuaternionCpp lhs, float rhs){ lhs /= rhs;  return lhs; }

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////
inline QuaternionCpp Normalise(const QuaternionCpp& lhs)
{
    // Use Vector4's version.
    return QuaternionCpp{Normalise(Vector4{lhs.values}).values};
}

// ///////////////////
// Constructors
// ///////////////////

QuaternionCpp::QuaternionCpp(const Vector3cpp& axis, float angle)
   : values(Normalise(QuaternionCpp(
                           axis.values[0] * sin(angle / 2),
                           axis.values[1] * sin(angle / 2),
                           axis.values[2] * sin(angle / 2),
                           cos(angle / 2))).values) {}


}}} // namespace


#endif // QUATERNIONCPP_HPP
