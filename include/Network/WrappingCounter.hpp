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

namespace GameInABox { namespace Network {

template<typename T, int BITS = 0>
class WrappingCounter
{
public:
    static constexpr unsigned bits()
    {
        return BITS == 0 ?
            std::numeric_limits<T>::digits :
            BITS > std::numeric_limits<T>::digits ?
                std::numeric_limits<T>::digits :
                BITS;
    }

    static constexpr T max() { return (1 << bits()) - 1; }

    // RAM: TODO: just replace with .value and then use common macros.
    // RAM: TODO: can't do that cos the common macros don't & max() the values.
    // RAM: TODO: Still need to implement the stuff that's in UnitOperators.hpp
    T Value() const { return myValue; }
    T& Value() { return myValue; }

    // This class needs to stay as a POD.
    WrappingCounter() = default;
    explicit WrappingCounter(T newValue) : myValue(newValue & max()) {}

    WrappingCounter<T, BITS>& operator++()
    {
        ++myValue;
        myValue &= max();
        return *this;
    }

    WrappingCounter<T, BITS>& operator--()
    {
        --myValue;
        myValue &= max();
        return *this;
    }

private:
    T myValue;
};

template<typename T, int BITS> inline bool operator==(const WrappingCounter<T, BITS>& lhs, const WrappingCounter<T, BITS>& rhs){return lhs.Value()==rhs.Value();}
template<typename T, int BITS> inline bool operator!=(const WrappingCounter<T, BITS>& lhs, const WrappingCounter<T, BITS>& rhs){return !operator==(lhs,rhs);}

template<typename T, int BITS>
bool operator<(const WrappingCounter<T, BITS>& lhs, const WrappingCounter<T, BITS>& rhs)
{
    if (lhs == rhs)
    {
        return false;
    }
    else
    {
        if (rhs.Value() > lhs.Value())
        {
            return ((rhs - lhs) <= ((WrappingCounter<T, BITS>::max() >> 1) + 1));
        }
        else
        {
            return ((lhs - rhs) > ((WrappingCounter<T, BITS>::max() >> 1) + 1));
        }
    }
}

template<typename T, int BITS> inline bool operator> (const WrappingCounter<T, BITS>& lhs, const WrappingCounter<T, BITS>& rhs){return  operator< (rhs,lhs);}
template<typename T, int BITS> inline bool operator<=(const WrappingCounter<T, BITS>& lhs, const WrappingCounter<T, BITS>& rhs){return !operator> (lhs,rhs);}
template<typename T, int BITS> inline bool operator>=(const WrappingCounter<T, BITS>& lhs, const WrappingCounter<T, BITS>& rhs){return !operator< (lhs,rhs);}

/* RAM: TODO
template<class DATATYPE> inline DATATYPE operator+(DATATYPE lhs, const DATATYPE& rhs){ lhs.value += rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs, const DATATYPE& rhs){ lhs.value -= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator-(DATATYPE lhs)                     { lhs.value = -lhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator*(DATATYPE lhs, const DATATYPE& rhs){ lhs.value *= rhs.value;  return lhs; }
template<class DATATYPE> inline DATATYPE operator/(DATATYPE lhs, const DATATYPE& rhs){ lhs.value /= rhs.value;  return lhs; }
*/

// Maths
template<typename T, int BITS>
T operator-(const WrappingCounter<T, BITS> &leftHandSide, const WrappingCounter<T, BITS> &rightHandSide)
{
    // Expecting overflow wraparound.
    return T((leftHandSide.Value() - rightHandSide.Value()) & WrappingCounter<T, BITS>::max());
}

template<typename T, int BITS>
T operator+(const WrappingCounter<T, BITS> &leftHandSide, const WrappingCounter<T, BITS> &rightHandSide)
{
    // Expecting overflow wraparound.
    return T((leftHandSide.Value() + rightHandSide.Value()) & WrappingCounter<T, BITS>::max());
}

template<typename T, int BITS>
WrappingCounter<T, BITS>& operator+=(WrappingCounter<T, BITS>& leftHandSide, const T rightHandSide)
{
    leftHandSide.Value() = (leftHandSide.Value() + rightHandSide) & WrappingCounter<T, BITS>::max();
    return leftHandSide;
}

template<typename T, int BITS>
WrappingCounter<T, BITS>& operator-=(WrappingCounter<T, BITS>& leftHandSide, const T rightHandSide)
{
    leftHandSide.Value() = (leftHandSide.Value() - rightHandSide) & WrappingCounter<T, BITS>::max();
    return leftHandSide;
}

// Boolean Maths
template<typename T, int BITS>
bool operator==(const WrappingCounter<T, BITS>& leftHandSide, uint64_t rightHandSide)
{
    return leftHandSide.Value() == rightHandSide;
}

template<typename T, int BITS>
bool operator==(uint64_t leftHandSide, const WrappingCounter<T, BITS>& rightHandSide)
{
    return leftHandSide == rightHandSide.Value();
}

}} // namespace

#endif // WRAPPINGCOUNTER_H
