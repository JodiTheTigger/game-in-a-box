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

#ifndef DELTA_HPP
#define DELTA_HPP

#include <DeltaMapItem.hpp>

#include <Common/BitStream.hpp>
#include <Common/BitStreamReadOnly.hpp>

#include <vector>

namespace GameInABox { namespace State { namespace Implementation {

struct Research
{
    bool doZeros;
    bool doXor;
};

// /////////////////////
// Delta Coder Interface
// /////////////////////
/*
template<class OBJECT>
class DeltaCoder
{
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
    const std::vector<DeltaMapItemInternal> myDeltaMap;
    const Research myResearch;
};

// /////////////////////
// Delta Coder Implementation
// /////////////////////

#include <DeltaMapItemInternal.hpp>

#include <Common/Logging.hpp>

void DeltaCreate(
        std::uint32_t base,
        std::uint32_t target,
        const DeltaMapItemInternal& map,
        GameInABox::Common::BitStream& out,
        Research settings);

std::uint32_t DeltaParse(
        std::uint32_t base,
        const DeltaMapItemInternal& map,
        GameInABox::Common::BitStreamReadOnly& in,
        Research settings);

DeltaCoder::DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        Research settings)
    : myDeltaMap()
    , myResearch(Research)
{

}
*/

}}} // namespace

#endif // DELTA_HPP
