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

#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <array>
#include <type_traits>

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Vector
{
    std::array<float, 4> values;
};

// ///////////////////
// Constants
// ///////////////////
constexpr Vector VectorZero{{{0.0f, 0.0f, 0.0f, 0.0f}}};
constexpr Vector VectorOne{{{1.0f, 1.0f, 1.0f, 1.0f}}};
constexpr Vector VectorNegativeOne{{{-1.0f, -1.0f, -1.0f, -1.0f}}};
constexpr Vector VectorOneZeroW{{{1.0f, 1.0f, 1.0f, 0.0f}}};
constexpr Vector VectorZeroWOne{{{0.0f, 0.0f, 0.0f, 1.0f}}};

// ///////////////////
// Comparison Operators
// ///////////////////
inline bool operator==(const Vector& lhs, const Vector& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const Vector& lhs, const Vector& rhs){return  !operator==(lhs,rhs);}
inline bool operator< (const Vector& lhs, const Vector& rhs){return lhs.values<rhs.values;}
inline bool operator> (const Vector& lhs, const Vector& rhs){return   operator< (rhs,lhs);}
inline bool operator<=(const Vector& lhs, const Vector& rhs){return  !operator> (lhs,rhs);}
inline bool operator>=(const Vector& lhs, const Vector& rhs){return  !operator< (lhs,rhs);}

// ///////////////////
// Access
// ///////////////////
inline constexpr float X(const Vector& rhs){ return rhs.values[0]; }
inline constexpr float Y(const Vector& rhs){ return rhs.values[1]; }
inline constexpr float Z(const Vector& rhs){ return rhs.values[2]; }
inline constexpr float W(const Vector& rhs){ return rhs.values[3]; }

// ///////////////////
// Testing
// ///////////////////
static_assert(std::is_pod<Vector>::value, "Vector is not a plain old data structure (POD).");
static_assert(alignof(Vector) == 16, "Vector is not aligned to a 16 bytes boundary.");

}}} // namespace

#endif // VECTOR_HPP
