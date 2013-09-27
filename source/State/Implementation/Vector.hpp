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

#include "VectorSimple.hpp"

#include <type_traits>

namespace GameInABox { namespace State { namespace Implementation {

// Gee, why is getting a nice vector library so hard?
// All the ones online are not POD compliant!
// http://www.gamasutra.com/view/feature/132636/designing_fast_crossplatform_simd_.php?print=1
// http://www.boost.org/doc/libs/1_37_0/libs/numeric/ublas/doc/overview.htm

using Vector = VectorSimple;

// ///////////////////
// Constants
// ///////////////////
constexpr Vector VectorZero{{{0.0f, 0.0f, 0.0f, 0.0f}}};
constexpr Vector VectorOne{{{1.0f, 1.0f, 1.0f, 1.0f}}};
constexpr Vector VectorNegativeOne{{{-1.0f, -1.0f, -1.0f, -1.0f}}};
constexpr Vector VectorOneZeroW{{{1.0f, 1.0f, 1.0f, 0.0f}}};
constexpr Vector VectorZeroWOne{{{0.0f, 0.0f, 0.0f, 1.0f}}};

// ///////////////////
// Testing
// ///////////////////
static_assert(std::is_pod<Vector>::value, "Vector is not a plain old data structure (POD).");

}}} // namespace

#endif // VECTOR_HPP
