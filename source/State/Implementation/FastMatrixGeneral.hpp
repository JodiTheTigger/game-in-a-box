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

#ifndef FASTMATRIXGENERAL_HPP
#define FASTMATRIXGENERAL_HPP

#include "FastMatrixGeneral.hpp"
#include "FastMatrixGeneral.hpp"

#include <cmath>

namespace GameInABox { namespace State { namespace Implementation {

// RAM: TODO: Unit test!
struct alignas(16) FastMatrixGeneral
{
    std::array<FastVectorGeneral, 4> values;

    constexpr FastMatrixGeneral(std::array<FastVectorGeneral, 4> vectors)
        : values(vectors) {}

    constexpr FastMatrixGeneral(FastVectorGeneral x, FastVectorGeneral y, FastVectorGeneral z, FastVectorGeneral w)
        : values{{x, y, z, w}} {}

    // row major
    constexpr FastMatrixGeneral(
                float xx, float xy, float xz, float xw,
                float yx, float yy, float yz, float yw,
                float zx, float zy, float zz, float zw,
                float wx, float wy, float wz, float ww)
        : values{{
              FastVectorGeneral(xx,xy,xz,xw),
              FastVectorGeneral(yx,yy,yz,yw),
              FastVectorGeneral(zx,zy,zz,zw),
              FastVectorGeneral(wx,wy,wz,ww)}} {}

    constexpr FastMatrixGeneral(
                float xx, float xy, float xz,
                float yx, float yy, float yz,
                float zx, float zy, float zz)
        : values{{
              FastVectorGeneral(xx,xy,xz,0),
              FastVectorGeneral(yx,yy,yz,0),
              FastVectorGeneral(zx,zy,zz,0),
              FastVectorGeneral(0,0,0,1.0f)}} {}
};

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Comparison Operators
// ///////////////////

inline bool operator==(const FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs){return  !operator==(lhs,rhs);}
inline bool operator< (const FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs){return lhs.values<rhs.values;}
inline bool operator> (const FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs){return   operator< (rhs,lhs);}
inline bool operator<=(const FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs){return  !operator> (lhs,rhs);}
inline bool operator>=(const FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs){return  !operator< (lhs,rhs);}

// ///////////////////
// Simple Maths
// ///////////////////
inline FastMatrixGeneral& operator+=(FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    lhs.values[3] += rhs.values[3];
    return lhs;
}

inline FastMatrixGeneral& operator-=(FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    lhs.values[3] -= rhs.values[3];
    return lhs;
}

inline FastMatrixGeneral& operator*=(FastMatrixGeneral& lhs, const FastMatrixGeneral& rhs)
{
    auto& l         = lhs.values;
    const auto& r   = rhs.values;

    auto result = FastMatrixGeneral
    {
            (l[0].values[0] * r[0].values[0]) + (l[0].values[1] * r[1].values[0]) + (l[0].values[2] * r[2].values[0]) + (l[0].values[3] * r[3].values[0]),
            (l[0].values[0] * r[0].values[1]) + (l[0].values[1] * r[1].values[1]) + (l[0].values[2] * r[2].values[1]) + (l[0].values[3] * r[3].values[1]),
            (l[0].values[0] * r[0].values[2]) + (l[0].values[1] * r[1].values[2]) + (l[0].values[2] * r[2].values[2]) + (l[0].values[3] * r[3].values[2]),
            (l[0].values[0] * r[0].values[3]) + (l[0].values[1] * r[1].values[3]) + (l[0].values[2] * r[2].values[3]) + (l[0].values[3] * r[3].values[3]),

            (l[1].values[0] * r[0].values[0]) + (l[1].values[1] * r[1].values[0]) + (l[1].values[2] * r[2].values[0]) + (l[1].values[3] * r[3].values[0]),
            (l[1].values[0] * r[0].values[1]) + (l[1].values[1] * r[1].values[1]) + (l[1].values[2] * r[2].values[1]) + (l[1].values[3] * r[3].values[1]),
            (l[1].values[0] * r[0].values[2]) + (l[1].values[1] * r[1].values[2]) + (l[1].values[2] * r[2].values[2]) + (l[1].values[3] * r[3].values[2]),
            (l[1].values[0] * r[0].values[3]) + (l[1].values[1] * r[1].values[3]) + (l[1].values[2] * r[2].values[3]) + (l[1].values[3] * r[3].values[3]),

            (l[2].values[0] * r[0].values[0]) + (l[2].values[1] * r[1].values[0]) + (l[2].values[2] * r[2].values[0]) + (l[2].values[3] * r[3].values[0]),
            (l[2].values[0] * r[0].values[1]) + (l[2].values[1] * r[1].values[1]) + (l[2].values[2] * r[2].values[1]) + (l[2].values[3] * r[3].values[1]),
            (l[2].values[0] * r[0].values[2]) + (l[2].values[1] * r[1].values[2]) + (l[2].values[2] * r[2].values[2]) + (l[2].values[3] * r[3].values[2]),
            (l[2].values[0] * r[0].values[3]) + (l[2].values[1] * r[1].values[3]) + (l[2].values[2] * r[2].values[3]) + (l[2].values[3] * r[3].values[3]),

            (l[3].values[0] * r[0].values[0]) + (l[3].values[1] * r[1].values[0]) + (l[3].values[2] * r[2].values[0]) + (l[3].values[3] * r[3].values[0]),
            (l[3].values[0] * r[0].values[1]) + (l[3].values[1] * r[1].values[1]) + (l[3].values[2] * r[2].values[1]) + (l[3].values[3] * r[3].values[1]),
            (l[3].values[0] * r[0].values[2]) + (l[3].values[1] * r[1].values[2]) + (l[3].values[2] * r[2].values[2]) + (l[3].values[3] * r[3].values[2]),
            (l[3].values[0] * r[0].values[3]) + (l[3].values[1] * r[1].values[3]) + (l[3].values[2] * r[2].values[3]) + (l[3].values[3] * r[3].values[3])
    };

    lhs = result;
    return lhs;
}

inline constexpr FastMatrixGeneral operator-(const FastMatrixGeneral& lhs)
{
    return FastMatrixGeneral
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
        -lhs.values[3]
    };
}

inline FastMatrixGeneral operator+(FastMatrixGeneral lhs, const FastMatrixGeneral& rhs){ lhs += rhs;  return lhs; }
inline FastMatrixGeneral operator-(FastMatrixGeneral lhs, const FastMatrixGeneral& rhs){ lhs -= rhs;  return lhs; }
inline FastMatrixGeneral operator*(FastMatrixGeneral lhs, const FastMatrixGeneral& rhs){ lhs *= rhs;  return lhs; }

// ///////////////////
// Vector and Matrix maths.
// ///////////////////
// RAM: TODO!

}}} // namespace

#endif // FASTMATRIXGENERAL_HPP
