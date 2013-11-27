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

#ifndef VECTORCOMON_HPP
#define VECTORCOMON_HPP

#include "UnitsSI.hpp"

#include <cmath>
#include <limits>

namespace GameInABox { namespace State { namespace Implementation {

// ///////////////////
// Helper Functions
// ///////////////////

template <typename VECTOR>
inline bool IsZeroFuzzy(const VECTOR& lhs)
{
    return LengthSquaredF(lhs) < std::numeric_limits<float>::epsilon();
}

// ///////////////////
// Complicated Maths (vector return)
// ///////////////////

template <typename VECTOR>
inline VECTOR PlaneArea(const VECTOR& lhs)
{
    return Dot(lhs, lhs);
}

template <typename VECTOR>
inline VECTOR Magnitude(const VECTOR& lhs)
{
    return Sqrt(PlaneArea(lhs));
}

template <typename VECTOR>
inline VECTOR Normalise(VECTOR lhs)
{
    auto length = Magnitude(lhs);

    if (length.values[0] > 0.0f)
    {
        return lhs / length;
    }

    return lhs;
}

// ///////////////////
// Complicated Maths (single return)
// ///////////////////

// Avoid these as they convert from Vectors to floats which
// apparently is a performance penalty, especially if you then
// use the value in more vector calculations.

template <typename VECTOR>
inline float PlaneAreaF(const VECTOR& lhs)
{
    return DotF(lhs, lhs);
}

template <typename VECTOR>
inline float MagnitudeF(const VECTOR& lhs)
{
    return sqrt(PlaneAreaF(lhs));
}

template <typename VECTOR>
inline Radians AngleF(const VECTOR& lhs, const VECTOR& rhs)
{
    // RAM: Investigate keeping the maths in vector form.
    auto squaredLengths = PlaneAreaF(lhs) * PlaneAreaF(rhs);

    return {std::acos(DotF(lhs, rhs) / squaredLengths)};
}

}}} // namespace

#endif // VECTORCOMON_HPP
