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

#ifndef UNITOPERATORS_HPP
#define UNITOPERATORS_HPP

// Comparison and Binary operators for struct based types
// with a solo member called "value"
template<class DATATYPE> inline bool operator==(const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value==rhs.value;}
template<class DATATYPE> inline bool operator!=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator==(lhs,rhs);}
template<class DATATYPE> inline bool operator< (const DATATYPE& lhs, const DATATYPE& rhs){return lhs.value< rhs.value;}
template<class DATATYPE> inline bool operator> (const DATATYPE& lhs, const DATATYPE& rhs){return  operator< (rhs,lhs);}
template<class DATATYPE> inline bool operator<=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator> (lhs,rhs);}
template<class DATATYPE> inline bool operator>=(const DATATYPE& lhs, const DATATYPE& rhs){return !operator< (lhs,rhs);}

// RAM: TODO: unit test this.
template<class DATATYPE> inline DATATYPE& operator+=(DATATYPE& lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE& operator-=(DATATYPE& lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }

template<class DATATYPE> inline DATATYPE operator+(DATATYPE lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs)                     { lhs.value = -lhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator*(DATATYPE lhs, const DATATYPE& rhs){ lhs.value *= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator/(DATATYPE lhs, const DATATYPE& rhs){ lhs.value /= rhs.value;  return lhs; }

#endif // UNITOPERATORS_HPP
