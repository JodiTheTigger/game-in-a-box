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

#include <array>

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

    // RAM: TODO: From Vector3 + Angle

    QuaternionCpp(const QuaternionCpp&) = default;
    QuaternionCpp(QuaternionCpp&&) = default;
    QuaternionCpp& operator=(const QuaternionCpp&) & = default;
    QuaternionCpp& operator=(QuaternionCpp&&) & = default;

    constexpr Vector ToVector() const { return Vector{values}; }
};

// RAM: TODO: The Maths!

}}} // namespace


#endif // QUATERNIONCPP_HPP
