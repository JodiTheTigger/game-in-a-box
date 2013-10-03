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

#ifndef VECTORFAST_HPP
#define VECTORFAST_HPP

#include "VectorFastGeneral.hpp"

namespace GameInABox { namespace State { namespace Implementation {

// Gee, why is getting a nice vector library so hard?
// All the ones online are not POD compliant!
// http://www.gamasutra.com/view/feature/132636/designing_fast_crossplatform_simd_.php?print=1
// http://www.boost.org/doc/libs/1_37_0/libs/numeric/ublas/doc/overview.htm
using VectorFast = VectorFastGeneral;

}}} // namespace

#endif // VECTORFAST_HPP
