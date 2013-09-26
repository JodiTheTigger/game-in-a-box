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

#ifndef VECTORSIMPLE_HPP
#define VECTORSIMPLE_HPP

#include <array>
#include <cmath>

// RAM: TODO: If referring, use reference, if copying use value.

namespace GameInABox { namespace State { namespace Implementation {

struct VectorSimple
{
    std::array<float, 4> values;
};

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Increment / Decrement
// ///////////////////
inline VectorSimple& operator++(VectorSimple& rhs)
{
    ++(rhs.values[0]);
    ++(rhs.values[1]);
    ++(rhs.values[2]);
    ++(rhs.values[3]);

    return rhs;
}

inline VectorSimple operator++(VectorSimple& lhs, int)
{
   auto copy = lhs;
   ++copy;

   return copy;
}

inline VectorSimple& operator--(VectorSimple& rhs)
{
    --(rhs.values[0]);
    --(rhs.values[1]);
    --(rhs.values[2]);
    --(rhs.values[3]);

    return rhs;
}

inline VectorSimple operator--(VectorSimple& lhs, int)
{
    auto copy = lhs;
    --copy;

    return copy;
}

// ///////////////////
// Comparison Operators
// ///////////////////

inline bool operator==(const VectorSimple& lhs, const VectorSimple& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const VectorSimple& lhs, const VectorSimple& rhs){return  !operator==(lhs,rhs);}
inline bool operator< (const VectorSimple& lhs, const VectorSimple& rhs){return lhs.values<rhs.values;}
inline bool operator> (const VectorSimple& lhs, const VectorSimple& rhs){return   operator< (rhs,lhs);}
inline bool operator<=(const VectorSimple& lhs, const VectorSimple& rhs){return  !operator> (lhs,rhs);}
inline bool operator>=(const VectorSimple& lhs, const VectorSimple& rhs){return  !operator< (lhs,rhs);}

// ///////////////////
// Simple Maths
// ///////////////////
inline VectorSimple& operator+=(VectorSimple& lhs, const VectorSimple& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    lhs.values[3] += rhs.values[3];
    return lhs;
}

inline VectorSimple& operator-=(VectorSimple& lhs, const VectorSimple& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    lhs.values[3] -= rhs.values[3];
    return lhs;
}

inline VectorSimple& operator*=(VectorSimple& lhs, const VectorSimple& rhs)
{
    lhs.values[0] *= rhs.values[0];
    lhs.values[1] *= rhs.values[1];
    lhs.values[2] *= rhs.values[2];
    lhs.values[3] *= rhs.values[3];
    return lhs;
}

inline VectorSimple& operator/=(VectorSimple& lhs, const VectorSimple& rhs)
{
    lhs.values[0] /= rhs.values[0];
    lhs.values[1] /= rhs.values[1];
    lhs.values[2] /= rhs.values[2];
    lhs.values[3] /= rhs.values[3];
    return lhs;
}

inline VectorSimple operator-(VectorSimple lhs)
{
    lhs.values[0] = -lhs.values[0];
    lhs.values[1] = -lhs.values[1];
    lhs.values[2] = -lhs.values[2];
    lhs.values[3] = -lhs.values[3];
    return lhs;
}

inline VectorSimple operator+(VectorSimple lhs, const VectorSimple& rhs){ lhs += rhs;  return lhs; }
inline VectorSimple operator-(VectorSimple lhs, const VectorSimple& rhs){ lhs -= rhs;  return lhs; }
inline VectorSimple operator*(VectorSimple lhs, const VectorSimple& rhs){ lhs *= rhs;  return lhs; }
inline VectorSimple operator/(VectorSimple lhs, const VectorSimple& rhs){ lhs /= rhs;  return lhs; }

// ///////////////////
// Complicated Maths
// ///////////////////
inline VectorSimple Sqrt(const VectorSimple& rhs)
{
    return VectorSimple
    {
            std::sqrt(rhs.values[0]),
            std::sqrt(rhs.values[1]),
            std::sqrt(rhs.values[2]),
            std::sqrt(rhs.values[3])
    };
}

inline VectorSimple Dot(VectorSimple lhs, const VectorSimple& rhs)
{
    lhs *= rhs;
    auto sum = lhs.values[0] + lhs.values[1] + lhs.values[2] + lhs.values[3];
    return VectorSimple
    {
         sum,
         sum,
         sum,
         sum
    };
}

// ///////////////////
// Access
// ///////////////////
inline float X(const VectorSimple& rhs){ return rhs.values[0]; }
inline float Y(const VectorSimple& rhs){ return rhs.values[1]; }
inline float Z(const VectorSimple& rhs){ return rhs.values[2]; }
inline float W(const VectorSimple& rhs){ return rhs.values[3]; }

}}} // namespace

#endif // VECTORSIMPLE_HPP
