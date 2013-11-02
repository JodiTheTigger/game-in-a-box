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

#ifndef UNITSSI_HPP
#define UNITSSI_HPP

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Unit Structs
// /////////////////////
struct Radians
{
    float value;
};

// /////////////////////
// User defined literals for units.
// /////////////////////
Radians constexpr operator"" _radians(long double value)
{
    return {static_cast<float>(value)};
}

}}} // namespace

namespace GameInABox { namespace State { namespace Implementation { namespace Units {

template<int M, int K, int S>
struct Unit
{
    // MKS = Meters, Kilograms, Seconds.
    enum { m=M, kg=K, s=S };
};

template<typename UNIT, typename T=float>
struct Quantity
{
    T value;
};

// ///////////////////
// Simple Maths
// ///////////////////

template<typename UNIT, typename T>
inline Quantity<UNIT, T>& operator+=(Quantity<UNIT, T>& lhs, const Quantity<UNIT, T>& rhs)
{
    lhs.value+=rhs.value;
    return lhs;
}

template<typename UNIT, typename T>
inline Quantity<UNIT, T>& operator-=(Quantity<UNIT, T>& lhs, const Quantity<UNIT, T>& rhs)
{
    lhs.value-=rhs.value;
    return lhs;
}

template<int M1, int M2, int K1, int K2, int S1, int S2, typename T>
inline Quantity<Unit<M1+M2, K1+K2, S1+S2>, T>& operator*=(Quantity<Unit<M1, K1, S1>, T>& lhs, const Quantity<Unit<M2, K2, S2>, T>& rhs)
{
    lhs.value*=rhs.value;
    return lhs;
}

template<int M1, int M2, int K1, int K2, int S1, int S2, typename T>
inline Quantity<Unit<M1-M2, K1-K2, S1-S2>, T>& operator/=(Quantity<Unit<M1, K1, S1>, T>& lhs, const Quantity<Unit<M2, K2, S2>, T>& rhs)
{
    lhs.value/=rhs.value;
    return lhs;
}

template<typename UNIT, typename T>
inline constexpr Quantity<UNIT, T> operator-(const Quantity<UNIT, T>& lhs)
{
    return Quantity<UNIT, T>{-lhs.value};
}

}}}} // namespace


#endif // UNITSSI_HPP
