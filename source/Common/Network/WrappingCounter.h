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

template<typename T>
class WrappingCounter
{
public:
    T Value() const { return myValue; }

    WrappingCounter() : WrappingCounter(0) {};
    WrappingCounter(T newValue) : myValue(newValue) {}
    WrappingCounter(const WrappingCounter<T>& other) : myValue(other.myValue) {};

private:
    T myValue;
};

template<typename T>
bool operator==(const WrappingCounter<T>& leftHandSide, const WrappingCounter<T>& rightHandSide)
{
    return leftHandSide.Value() == rightHandSide.Value();
}

template<typename T>
bool operator>(const WrappingCounter<T>& leftHandSide, const WrappingCounter<T>& rightHandSide)
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

template<typename T>
bool operator<(const WrappingCounter<T>& leftHandSide, const WrappingCounter<T>& rightHandSide)
{
    if (leftHandSide == rightHandSide)
    {
        return false;
    }
    else
    {
        if (rightHandSide.Value() > leftHandSide.Value())
        {
            return (uint16_t(rightHandSide.Value() - leftHandSide.Value()) <= (1 << 15));
        }
        else
        {
            return (uint16_t(leftHandSide.Value() - rightHandSide.Value()) > (1 << 15));
        }
    }
}


#endif // WRAPPINGCOUNTER_H
