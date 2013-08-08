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

#ifndef UNITS_HPP
#define UNITS_HPP

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Unit Structs
// /////////////////////
struct Bits
{
    signed value;
};

struct Bytes
{
    signed value;
};

// /////////////////////
// User defined literals for units.
// /////////////////////
Bits constexpr operator"" _bits(unsigned long long value)
{
    return {static_cast<signed>(value)};
}
Bits constexpr operator"" _bit(unsigned long long value)
{
    // Argh, Plurals
    return {static_cast<signed>(value)};
}

Bytes constexpr operator"" _bytes(unsigned long long value)
{
    return {static_cast<signed>(value)};
}
Bytes constexpr operator"" _byte(unsigned long long value)
{
    return {static_cast<signed>(value)};
}

// /////////////////////
// Comparisons and maths
// /////////////////////
template<class DATATYPE> inline bool operator==(const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value==rhs.value;}
template<class DATATYPE> inline bool operator!=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator==(lhs,rhs);}
template<class DATATYPE> inline bool operator< (const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value< rhs.value;}
template<class DATATYPE> inline bool operator> (const DATATYPE& lhs, const DATATYPE& rhs){return  operator< (rhs,lhs);}
template<class DATATYPE> inline bool operator<=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator> (lhs,rhs);}
template<class DATATYPE> inline bool operator>=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator< (lhs,rhs);}

template<class DATATYPE> inline DATATYPE operator+(DATATYPE lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs)                     { lhs.value = -lhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator*(DATATYPE lhs, const DATATYPE& rhs){ lhs.value *= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator/(DATATYPE lhs, const DATATYPE& rhs){ lhs.value /= rhs.value;  return lhs; }

}}} // namespace

#endif // UNITS_HPP
