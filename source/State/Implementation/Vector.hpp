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

// RAM: TODO: Remove this class now we're C++14. Don't need it as
// We can have vector3 and vector4 as POD types due to C++14's new
// member initilisation.
struct alignas(16) Vector
{
    std::array<float, 4> values;
};

// ///////////////////
// Comparison Operators
// ///////////////////
inline bool operator==(const Vector& lhs, const Vector& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const Vector& lhs, const Vector& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Testing
// ///////////////////
static_assert(std::is_pod<Vector>::value, "Vector is not a plain old data structure (POD).");
static_assert(alignof(Vector) == 16, "Vector is not aligned to a 16 byte boundary.");

}}} // namespace

#endif // VECTOR_HPP
