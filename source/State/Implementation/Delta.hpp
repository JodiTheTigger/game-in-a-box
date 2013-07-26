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
#include <Common/Logging.hpp>

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
    const std::vector<DeltaMapItem> myDeltaMap;
    const Research myResearch;
};

// /////////////////////
// Delta Coder Implementation
// /////////////////////

/*
#include <Common/Logging.hpp>

void DeltaCreate(
        std::uint32_t base,
        std::uint32_t target,
        const DeltaMapItem& map,
        GameInABox::Common::BitStream& out,
        Research settings);

std::uint32_t DeltaParse(
        std::uint32_t base,
        const DeltaMapItem& map,
        GameInABox::Common::BitStreamReadOnly& in,
        Research settings);

// RAM: TODO: Make sure the class is a pod (ofsetoff-able) class.
DeltaCoder<class OBJECT>::DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        Research settings)
    : myDeltaMap()
    , myResearch(Research)
{
    // Ignore ignored deltaMapItems and ones that buffer overrun.
    for (auto map : myDeltaMap)
    {
        if (map.type == DeltaMapItem::MapType::Ignore)
        {
            continue;
        }

        if ((map.offsetInfo.offset + 4) > sizeof(OBJECT))
        {
            Common::Log(
                Common::LogLevel::Warning,
                "DeltaCoder: Map item '",
                map.name.c_str(),
                "' accesses memory out of bounds. Ignored.");

            continue;
        }

        myDeltaMap.push_back(map);
    }
}

void DeltaCoder<class OBJECT>::DeltaDecode(
    const OBJECT& base,
    OBJECT& result,
    GameInABox::Common::BitStreamReadOnly& dataIn) const
{
    for (const auto& map : myDeltaMap)
    {
        uint32_t base;

        // RAM: TODO: Make sure uint32_t == float == 4.
        memcpy(&base, reinterpret_cast<const char *>(&base) + map.offsetInfo.offset, sizeof(uint32_t));

        auto result = DeltaParse(base, map, dataIn, myResearch);

        memcpy(reinterpret_cast<char *>(&result) + map.offsetInfo.offset, &result, sizeof(uint32_t));
    }
}
*/
}}} // namespace

#endif // DELTA_HPP
