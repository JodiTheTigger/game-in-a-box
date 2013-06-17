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

#ifndef BUFFERSERIALISATION_H
#define BUFFERSERIALISATION_H

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <type_traits>
#include <iterator>
#endif
// RAM: TODO: Use static cast, and do random access iterator pull.
namespace GameInABox { namespace Common { namespace Network {

// =======================
// Interface at the bottom
// =======================

// Endian Tags
struct BigEndian {};
struct LittleEndian {};

// Inplementation
// Anonymous namespace to keep implmentation local
namespace
{
// Big endian random access
template <typename Iterator>
void Push(Iterator buffer, uint8_t value, std::random_access_iterator_tag, BigEndian)
{
    buffer[0] = value;
}

template <typename Iterator>
void Push(Iterator buffer, uint16_t value, std::random_access_iterator_tag, BigEndian)
{
    buffer[0] = uint8_t(value >> 8);
    buffer[1] = uint8_t(value);
}

template <typename Iterator>
void Push(Iterator buffer, uint32_t value, std::random_access_iterator_tag, BigEndian)
{
    buffer[0] = uint8_t(value >> 24);
    buffer[1] = uint8_t(value >> 16);
    buffer[2] = uint8_t(value >> 8);
    buffer[3] = uint8_t(value);
}

template <typename Iterator>
void Pull(Iterator buffer, uint8_t& result, std::random_access_iterator_tag, BigEndian)
{
    result = buffer[0];
}

template <typename Iterator>
void Pull(Iterator buffer, uint16_t& result, std::random_access_iterator_tag, BigEndian)
{
    result = uint16_t(
            (buffer[0] << 8) |
            buffer[1]);
}

template <typename Iterator>
void Pull(Iterator buffer, uint32_t& result, std::random_access_iterator_tag, BigEndian)
{
    result = uint32_t(
            (buffer[0] << 24) |
            (buffer[1] << 16) |
            (buffer[2] << 8) |
            buffer[3]);
}

// Big endian general
template <typename Iterator>
void Push(Iterator buffer, uint8_t value, std::output_iterator_tag, BigEndian)
{
    *buffer = value;
}

template <typename Iterator>
void Push(Iterator buffer, uint16_t value, std::output_iterator_tag, BigEndian)
{
    *buffer++ = uint8_t(value >> 8);
    *buffer++ = uint8_t(value);
}

template <typename Iterator>
void Push(Iterator buffer, uint32_t value, std::output_iterator_tag, BigEndian)
{
    *buffer++ = uint8_t(value >> 24);
    *buffer++ = uint8_t(value >> 16);
    *buffer++ = uint8_t(value >> 8);
    *buffer++ = uint8_t(value);
}

template <typename Iterator>
void Pull(Iterator buffer, uint8_t& result, std::input_iterator_tag, BigEndian)
{
    result = *buffer;
}

template <typename Iterator>
void Pull(Iterator buffer, uint16_t& result, std::input_iterator_tag, BigEndian)
{
    result = uint16_t(
            (*buffer++ << 8) |
            *buffer);
}

template <typename Iterator>
void Pull(Iterator buffer, uint32_t& result, std::input_iterator_tag, BigEndian)
{
    result = uint32_t(
            (*buffer++ << 24) |
            (*buffer++ << 16) |
            (*buffer++ << 8) |
            *buffer);
}

} // namespace

// =======================
// Interface
// =======================
template <typename Iterator, typename Datum>
void Push(Iterator start, Datum value)
{
    typedef typename std::iterator_traits<Iterator>::iterator_category category;
    Push(start, value, category(), BigEndian());
}

template <typename Iterator, typename Datum>
void Pull(Iterator start, Datum& value)
{
    typedef typename std::iterator_traits<Iterator>::iterator_category category;
    Pull(start, value, category(), BigEndian());
}

}}} // namespace

#endif // BUFFERSERIALISATION_H
