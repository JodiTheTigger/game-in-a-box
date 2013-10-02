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

#ifndef VECTORLOAD_HPP
#define VECTORLOAD_HPP

#include "VectorAligned.hpp"

namespace GameInABox { namespace State { namespace Implementation {

// I need a way of differing functions by return type. This is what I came up with.
// Use template specialisation. I get around the one defintion rule by using "inline"
// for the specilisation itself.
// NOTE: All of this is necessary because I can't just init a vector using:
// auto pos = Vector{0.0, 0.0, 0.0, 0,0};
// This is a problem when Vector isn't a struct{std::array<float, 4>} but actually a
// using VectorSSE = __m128;
template<typename T> constexpr T Load(float x, float y, float z, float w);
template<typename T> constexpr T Load(float x, float y, float z);
template<typename T> constexpr T Load(float x, float y);
template<typename T> constexpr T Load(float x);
template<typename T> constexpr T Load(const VectorAligned& v);

// Copies X into all the type's arrays.
template<typename T> constexpr T LoadReplicate(float x);

}}} // namespace

#endif // VECTORLOAD_HPP
