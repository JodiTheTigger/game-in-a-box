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

#ifndef CLIENTHANDLE_HPP
#define CLIENTHANDLE_HPP

#include <cstdint>

namespace GameInABox { namespace Network {

struct ClientHandle
{    
    std::uint32_t value;
};


// RAM: TODO: use COMPARISONS_AND_MATHS please.
inline bool operator==(const ClientHandle& lhs, const ClientHandle& rhs){return lhs.value==rhs.value;}
inline bool operator!=(const ClientHandle& lhs, const ClientHandle& rhs){return !operator==(lhs,rhs);}

}} // namespace

#endif // CLIENTHANDLE_HPP
