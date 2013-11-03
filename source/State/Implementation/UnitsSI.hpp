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
template<typename UNIT, typename T1, typename T2>
inline Quantity<UNIT, T1>& operator+=(Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs)
{
    lhs.value+=rhs.value;
    return lhs;
}

template<typename UNIT, typename T1, typename T2>
inline Quantity<UNIT, T1>& operator-=(Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs)
{
    lhs.value-=rhs.value;
    return lhs;
}

template<int M1, int M2, int K1, int K2, int S1, int S2, typename T1, typename T2>
inline Quantity<Unit<M1+M2, K1+K2, S1+S2>, T1>& operator*=(Quantity<Unit<M1, K1, S1>, T1>& lhs, const Quantity<Unit<M2, K2, S2>, T2>& rhs)
{
    lhs.value*=rhs.value;
    return lhs;
}

template<int M1, int M2, int K1, int K2, int S1, int S2, typename T1, typename T2>
inline Quantity<Unit<M1-M2, K1-K2, S1-S2>, T1>& operator/=(Quantity<Unit<M1, K1, S1>, T1>& lhs, const Quantity<Unit<M2, K2, S2>, T2>& rhs)
{
    lhs.value/=rhs.value;
    return lhs;
}

template<typename UNIT, typename T>
inline constexpr Quantity<UNIT, T> operator-(const Quantity<UNIT, T>& lhs)
{
    return Quantity<UNIT, T>{-lhs.value};
}

template<typename UNIT, typename T1, typename T2>
inline Quantity<UNIT, T1> operator+(Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){ lhs += rhs;  return lhs; }
template<typename UNIT, typename T1, typename T2>
inline Quantity<UNIT, T1> operator-(Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){ lhs -= rhs;  return lhs; }

template<int M1, int M2, int K1, int K2, int S1, int S2, typename T1, typename T2>
inline Quantity<Unit<M1-M2, K1-K2, S1-S2>, T1> operator*(Quantity<Unit<M1, K1, S1>, T1> lhs, const Quantity<Unit<M2, K2, S2>, T2>& rhs)
{
    lhs *= rhs;  return lhs;
}
template<int M1, int M2, int K1, int K2, int S1, int S2, typename T1, typename T2>
inline Quantity<Unit<M1-M2, K1-K2, S1-S2>, T1> operator/(Quantity<Unit<M1, K1, S1>, T1> lhs, const Quantity<Unit<M2, K2, S2>, T2>& rhs)
{
    lhs /= rhs;  return lhs;
}

// ///////////////////
// Comparison
// ///////////////////
template<typename UNIT, typename T1, typename T2>
inline constexpr bool operator==(const Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){return lhs.value==rhs.value;}
template<typename UNIT, typename T1, typename T2>
inline constexpr bool operator!=(const Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){return !operator==(lhs,rhs);}
template<typename UNIT, typename T1, typename T2>
inline constexpr bool operator< (const Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){return lhs.value< rhs.value;}
template<typename UNIT, typename T1, typename T2>
inline constexpr bool operator> (const Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){return  operator< (rhs,lhs);}
template<typename UNIT, typename T1, typename T2>
inline constexpr bool operator<=(const Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){return !operator> (lhs,rhs);}
template<typename UNIT, typename T1, typename T2>
inline constexpr bool operator>=(const Quantity<UNIT, T1>& lhs, const Quantity<UNIT, T2>& rhs){return !operator< (lhs,rhs);}

// ///////////////////
// Basic SI Units
// ///////////////////

// Have a read of this if you want to make this more complicated.
// http://www.csee.umbc.edu/portal/help/theory/units.shtml

//Speed         = Length / Time
//Acceleration	= Speed	 / Time
//Force         = Mass	 * Acceleration
//AngularSpeed	= Angle	 / Time
//Length        = Angle	 * Length
//Area          = Length * Length
//Volume        = Area	 * Length
//Density       = Mass	 / Volume
//MassFlowRate	= Mass	 / Time
//Frequency     = float	 / Time
//Pressure      = Force	 / Area

using Meter = Unit<1,0,0>;
using Meters = Meter;

using Kilogram = Unit<0,1,0>;
using Kilograms = Kilogram;

using Second = Unit<0,0,1>;
using Seconds = Second;

using MetersPerSecond = Unit<1,0,-1>;
using MetersPerSecondSquared = Unit<1,0,-2>;

// Unitless
using Radian = Unit<0,0,0>;
using Radians = Radian;

}}}} // namespace


#endif // UNITSSI_HPP
