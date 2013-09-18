/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012 Richard Maxwell <jodi.the.tigger@gmail.com>

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

#ifndef WRAPPINGCOUNTER_H
#define WRAPPINGCOUNTER_H

#include <cstdint>
#include <limits>
#include <type_traits>

namespace GameInABox { namespace Network {

template<typename T, int BITS = 0>
class WrappingCounter
{
public:
    static_assert(std::is_unsigned<T>::value, "Wrapping Counter only works on unsigned types.");

    static constexpr unsigned bits()
    {
        return BITS == 0 ?
            std::numeric_limits<T>::digits :
            BITS > std::numeric_limits<T>::digits ?
                std::numeric_limits<T>::digits :
                BITS;
    }

    static constexpr T max()
    {
        return BITS >= std::numeric_limits<T>::digits ?
            std::numeric_limits<T>::max() :
            static_cast<T>((1UL << bits()) - 1);
    }

    // RAM: TODO: just replace with .value and then use common macros.
    // RAM: TODO: can't do that cos the common macros don't & max() the values.
    // RAM: TODO: Still need to implement the stuff that's in UnitOperators.hpp

    // This class needs to stay as a POD.
    WrappingCounter() = default;
    explicit WrappingCounter(T newValue) : value(newValue & max()) {}

    T value;
};

// ///////////////////
// Operators
// ///////////////////

#define WRAP_C WrappingCounter<T, BITS>

// ///////////////////
// Increment / Decrement
// ///////////////////
template<typename T, int BITS>
inline WRAP_C& operator++(WRAP_C& rhs)
{
    ++(rhs.value);
    rhs.value &= WRAP_C::max();

    return rhs;
}

template<typename T, int BITS>
inline WRAP_C operator++(WRAP_C& lhs, int)
{
   auto copy = lhs;
   ++copy;

   return copy;
}

template<typename T, int BITS>
inline WRAP_C& operator--(WRAP_C& rhs)
{
    --(rhs.value);
    rhs.value &= WRAP_C::max();

    return rhs;
}

template<typename T, int BITS>
inline WRAP_C operator--(WRAP_C& lhs, int)
{
    auto copy = lhs;
    --copy;

    return copy;
}

// ///////////////////
// Equality Operators
// ///////////////////

template<typename T, int BITS> inline bool operator==(const WRAP_C& lhs, const WRAP_C& rhs){return lhs.value==rhs.value;}
template<typename T, int BITS> inline bool operator!=(const WRAP_C& lhs, const WRAP_C& rhs){return !operator==(lhs,rhs);}

// Compare against unsigned.
template<typename T, int BITS, typename U> inline bool operator==(const WRAP_C& lhs, U rhs){return lhs.value==rhs;}
template<typename T, int BITS, typename U> inline bool operator!=(const WRAP_C& lhs, U rhs){return !operator==(lhs,rhs);}

template<typename T, int BITS, typename U> inline bool operator==(U lhs, const WRAP_C& rhs){return lhs==rhs.value;}
template<typename T, int BITS, typename U> inline bool operator!=(U lhs, const WRAP_C& rhs){return !operator==(lhs,rhs);}

// ///////////////////
// Comparison Operators
// ///////////////////
template<typename T, int BITS>
bool operator<(const WRAP_C& lhs, const WRAP_C& rhs)
{
    if (lhs == rhs)
    {
        return false;
    }
    else
    {
        if (rhs.value > lhs.value)
        {
            return ((rhs - lhs).value <= ((WRAP_C::max() >> 1) + 1));
        }
        else
        {
            return ((lhs - rhs).value > ((WRAP_C::max() >> 1) + 1));
        }
    }
}

template<typename T, int BITS> inline bool operator> (const WRAP_C& lhs, const WRAP_C& rhs){return  operator< (rhs,lhs);}
template<typename T, int BITS> inline bool operator<=(const WRAP_C& lhs, const WRAP_C& rhs){return !operator> (lhs,rhs);}
template<typename T, int BITS> inline bool operator>=(const WRAP_C& lhs, const WRAP_C& rhs){return !operator< (lhs,rhs);}

// ///////////////////
// Maths assign
// ///////////////////
template<typename T, int BITS>
inline WRAP_C& operator+=(WRAP_C& lhs, const WRAP_C& rhs)
{
    lhs.value += rhs.value;
    lhs.value &= WRAP_C::max();

    return lhs;
}

template<typename T, int BITS>
inline WRAP_C& operator-=(WRAP_C& lhs, const WRAP_C& rhs)
{
    lhs.value -= rhs.value;
    lhs.value &= WRAP_C::max();

    return lhs;
}

template<typename T, int BITS>
inline WRAP_C& operator*=(WRAP_C& lhs, const WRAP_C& rhs)
{
    lhs.value *= rhs.value;
    lhs.value &= WRAP_C::max();

    return lhs;
}

template<typename T, int BITS>
inline WRAP_C& operator/=(WRAP_C& lhs, const WRAP_C& rhs)
{
    lhs.value /= rhs.value;

    // No need to clamp as it will always be smaller than max().
    // lhs.value &= WRAP_C::max();

    return lhs;
}

// against unsigned

template<typename T, int BITS>
inline WRAP_C& operator+=(WRAP_C& lhs, T rhs)
{
    lhs.value += rhs;
    lhs.value &= WRAP_C::max();

    return lhs;
}

template<typename T, int BITS>
inline WRAP_C& operator-=(WRAP_C& lhs, T rhs)
{
    lhs.value -= rhs;
    lhs.value &= WRAP_C::max();

    return lhs;
}

template<typename T, int BITS>
inline WRAP_C& operator*=(WRAP_C& lhs, T rhs)
{
    lhs.value *= rhs;
    lhs.value &= WRAP_C::max();

    return lhs;
}

template<typename T, int BITS>
inline WRAP_C& operator/=(WRAP_C& lhs, T rhs)
{
    lhs.value /= rhs;

    // No need to clamp as it will always be smaller than max().
    // lhs.value &= WRAP_C::max();

    return lhs;
}

// ///////////////////
// Maths
// ///////////////////

template<typename T, int BITS> inline WRAP_C operator+(WRAP_C lhs, const WRAP_C& rhs){ lhs += rhs;  return lhs; }
template<typename T, int BITS> inline WRAP_C operator-(WRAP_C lhs, const WRAP_C& rhs){ lhs -= rhs;  return lhs; }
template<typename T, int BITS> inline WRAP_C operator-(WRAP_C lhs)                     { lhs = WRAP_C{0} - lhs;  return lhs; }
template<typename T, int BITS> inline WRAP_C operator*(WRAP_C lhs, const WRAP_C& rhs){ lhs *= rhs;  return lhs; }
template<typename T, int BITS> inline WRAP_C operator/(WRAP_C lhs, const WRAP_C& rhs){ lhs /= rhs;  return lhs; }

// Unsigned
template<typename T, int BITS> inline WRAP_C operator+(WRAP_C lhs, T rhs){ lhs += rhs;  return lhs; }
template<typename T, int BITS> inline WRAP_C operator-(WRAP_C lhs, T rhs){ lhs -= rhs;  return lhs; }
template<typename T, int BITS> inline WRAP_C operator*(WRAP_C lhs, T rhs){ lhs *= rhs;  return lhs; }
template<typename T, int BITS> inline WRAP_C operator/(WRAP_C lhs, T rhs){ lhs /= rhs;  return lhs; }

#undef WRAP_C

}} // namespace

#endif // WRAPPINGCOUNTER_H
