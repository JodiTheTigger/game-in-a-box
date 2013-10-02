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

#ifndef VECTORSTORE_HPP
#define VECTORSTORE_HPP

#include <array>
#include <type_traits>

namespace GameInABox { namespace State { namespace Implementation {

// Used to store vectors into memory
struct alignas(16) VectorAligned
{
    std::array<float, 4> values;

    VectorAligned() = default;
    VectorAligned(float x) : values({x,0.0f,0.0f,0.0f}) {}
};

// Thanks C++11, now this is portable :-)
//using VectorAligned = std::aligned_storage<sizeof(VectorUnaligned),alignof(VectorUnaligned)>::type;

// Thanks MSVC for only supporting a max align of 8, meaning 16 is an "over-aligned" value and only
// supported by compiler specific extentions! I'll add hacky #ifdefs to use _declspec(align(16)) when I support MSVC.
// http://stackoverflow.com/questions/15459316/strange-behaviour-of-stdaligned-storage
// http://stackoverflow.com/questions/15788947/where-can-i-use-alignas-in-c11
// RAM: TODO: Shhh, this fails :-( static_assert(alignof(VectorAligned) != 16, "VectorStore isn't aligned to a 16 bytes boundary.");

// ///////////////////
// Comparison Operators
// ///////////////////
inline bool operator==(const VectorAligned& lhs, const VectorAligned& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const VectorAligned& lhs, const VectorAligned& rhs){return  !operator==(lhs,rhs);}
inline bool operator< (const VectorAligned& lhs, const VectorAligned& rhs){return lhs.values<rhs.values;}
inline bool operator> (const VectorAligned& lhs, const VectorAligned& rhs){return   operator< (rhs,lhs);}
inline bool operator<=(const VectorAligned& lhs, const VectorAligned& rhs){return  !operator> (lhs,rhs);}
inline bool operator>=(const VectorAligned& lhs, const VectorAligned& rhs){return  !operator< (lhs,rhs);}

// ///////////////////
// Access
// ///////////////////
inline constexpr float X(const VectorAligned& rhs){ return rhs.values[0]; }
inline constexpr float Y(const VectorAligned& rhs){ return rhs.values[1]; }
inline constexpr float Z(const VectorAligned& rhs){ return rhs.values[2]; }
inline constexpr float W(const VectorAligned& rhs){ return rhs.values[3]; }

}}} // namespace

#endif // VECTORSTORE_HPP
