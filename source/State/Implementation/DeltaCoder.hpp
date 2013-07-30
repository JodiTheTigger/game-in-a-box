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

#ifndef DELTACODER_HPP
#define DELTACODER_HPP

#include "DeltaMapItem.hpp"
#include "DeltaCoderTools.hpp"

#include <cstdint>
#include <vector>

namespace GameInABox { namespace Common {
class BitStream;
class BitStreamReadOnly;
}}

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Delta Coder Interface
// /////////////////////
template<class OBJECT>
class DeltaCoder
{
    // type checking
    static_assert(std::is_standard_layout<OBJECT>::value,
                  "DeltaCoder requires a standard layout object (pod struct, class or union).");

    static_assert(sizeof(float) == sizeof(std::uint32_t),
                  "DeltaCoder will have undefined behaviour unless sizeof(float) == sizeof(uint32_t).");

    static_assert(sizeof(float) == 4,
                  "DeltaCoder will have undefined behaviour unless sizeof(float) == 4.");

    static_assert(sizeof(std::uint32_t) == 4,
                  "DeltaCoder will have undefined behaviour unless sizeof(std::uint32_t) == 4.");

public:
    DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        Research settings);

     void DeltaDecode(
         const OBJECT& base,
         OBJECT& result,
         GameInABox::Common::BitStreamReadOnly& dataIn) const;

     void DeltaEncode(
         const OBJECT& base,
         const OBJECT& toDelta,
         GameInABox::Common::BitStream& dataOut) const;

private:
    std::vector<DeltaMapItem> myDeltaMap;
    const Research myResearch;
};

}}} // namespace


// /////////////////////
// Implementation
// /////////////////////
#include "DeltaCoder.tpp"

#endif // DELTACODER_HPP
