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

#ifndef XORCODE_H
#define XORCODE_H

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <vector>
#include <type_traits>
#include <array>
#endif

namespace GameInABox { namespace Common { namespace Network {

/*

// Left this comment block here incase I decide to check for iterator data types.

// Only allow uint8_t iterators by checking that the iterator
// type (T::value_type) is a uint8_t.
template<class Uint8tIteratorWrite,
         class Uint8tIteratorRead,
         std::size_t Size,
         class = typename std::enable_if<
             std::is_same<typename Uint8tIteratorWrite::value_type, std::uint8_t>::value>
         ::type,
         class = typename std::enable_if<
             std::is_same<typename Uint8tIteratorRead::value_type, std::uint8_t>::value>
         ::type>
void XorCode(
    Uint8tIteratorWrite writeBegin,
    Uint8tIteratorWrite writeEnd,
    const Uint8tIteratorRead& xorBuffer)
{
    static_assert(Size != 0, "Buffer to xor Against is empty.");

    std::size_t index(0);
    while (writeBegin != writeEnd)
    {
        *writeBegin = *writeBegin ^ xorBuffer[index];
        index = (index + 1) % Size;

        ++writeBegin;
    }
}
*/

template<class Uint8tIteratorWrite,
         class Uint8tIteratorRead,
         std::size_t Size>
void XorCode(
    Uint8tIteratorWrite writeBegin,
    Uint8tIteratorWrite writeEnd,
    const Uint8tIteratorRead (&xorBuffer)[Size])
{
    static_assert(Size != 0, "Buffer to xor Against is empty.");

    std::size_t index(0);
    while (writeBegin != writeEnd)
    {
        *writeBegin = *writeBegin ^ xorBuffer[index];
        index = (index + 1) % Size;

        ++writeBegin;
    }

    // I wanted to do the following, but it just calls this function again instead of the one
    // above. As xorBuffer is a uint8_t[xx] as opposed to a &uint8_t.
    //XorCode<Uint8tIteratorWrite, Uint8tIteratorRead, Size>(writeBegin, writeEnd, xorBuffer);
}

template<class Uint8tIteratorWrite,
         typename T,
         std::size_t Size>
void XorCode(
    Uint8tIteratorWrite writeBegin,
    Uint8tIteratorWrite writeEnd,
    const std::array<T, Size>& xorBuffer)
{
    static_assert(Size != 0, "Buffer to xor Against is empty.");

    std::size_t index(0);
    while (writeBegin != writeEnd)
    {
        *writeBegin = *writeBegin ^ xorBuffer[index];
        index = (index + 1) % Size;

        ++writeBegin;
    }
}

template<class Uint8tIteratorWrite,
         class Uint8tIteratorRead>
void XorCode(
        Uint8tIteratorWrite writeBegin,
        Uint8tIteratorWrite writeEnd,
        const Uint8tIteratorRead& xorBuffer)
{
    if (!xorBuffer.empty())
    {
        std::size_t codeSize    = xorBuffer.size();
        std::size_t index       = 0;

        while (writeBegin != writeEnd)
        {
            *writeBegin = *writeBegin ^ xorBuffer[index];
            index = (index + 1) % codeSize;

            ++writeBegin;
        }
    }
}
}}} // namespace

#endif // XORCODE_H
