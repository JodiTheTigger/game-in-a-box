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

#include "Sequence.h"

bool Sequence::operator<(const Sequence& other) const
{
    if (*this == other)
    {
        return false;
    }
    else
    {
        if (other.myValue > myValue)
        {
            // RAM: TODO! Unit test! Fix Maths!
            return (uint16_t(other.myValue - myValue) <= 32768);
        }
        else
        {
            // RAM: DEBUG
            //int diff(myValue - other.myValue);
            //bool result(diff >= 32768);

            // RAM: TODO! Unit test! Fix Maths!
            //return ((myValue - other.myValue) >= 32768);
            //return result;
            return (uint16_t(myValue - other.myValue) >= 32768);
        }
    }
}

bool Sequence::operator>(const Sequence& other) const
{
    if (*this == other)
    {
        return false;
    }
    else
    {
        return !(*this < other);
    }
}

bool Sequence::operator==(const Sequence& other) const
{
    return other.myValue == myValue;
}
