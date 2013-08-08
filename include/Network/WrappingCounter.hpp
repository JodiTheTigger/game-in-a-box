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
    T Value() const { return myValue; }
    T& Value() { return myValue; }

    WrappingCounter() : WrappingCounter(0) {};
    explicit WrappingCounter(T newValue) : myValue(newValue & max()) {}
    WrappingCounter(const WrappingCounter<T, BITS>& other) : myValue(other.myValue) {};

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
bool operator==(const WrappingCounter<T, BITS>& leftHandSide, const WrappingCounter<T, BITS>& rightHandSide)
{
    return leftHandSide.Value() == rightHandSide.Value();
}

template<typename T, int BITS>
bool operator!=(const WrappingCounter<T, BITS>& leftHandSide, const WrappingCounter<T, BITS>& rightHandSide)
{
    return !(leftHandSide.Value() == rightHandSide.Value());
}

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

template<typename T, int BITS>
bool operator<(const WrappingCounter<T, BITS>& leftHandSide, const WrappingCounter<T, BITS>& rightHandSide)
{
    if (leftHandSide == rightHandSide)
    {
        return false;
    }
    else
    {
        if (rightHandSide.Value() > leftHandSide.Value())
        {
            return ((rightHandSide - leftHandSide) <= ((WrappingCounter<T, BITS>::max() >> 1) + 1));
        }
        else
        {
            return ((leftHandSide - rightHandSide) > ((WrappingCounter<T, BITS>::max() >> 1) + 1));
        }
    }
}

template<typename T, int BITS>
bool operator>(const WrappingCounter<T, BITS>& leftHandSide, const WrappingCounter<T, BITS>& rightHandSide)
{
    if (leftHandSide == rightHandSide)
    {
        return false;
    }
    else
    {
        return !(leftHandSide < rightHandSide);
    }
}

}} // namespace

#endif // WRAPPINGCOUNTER_H
