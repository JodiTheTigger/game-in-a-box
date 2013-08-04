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

#ifndef DELTACODER_TPP
#define DELTACODER_TPP

#include <Common/Logging.hpp>

#include <cstring> // memcpy
#include <type_traits>

namespace GameInABox { namespace State { namespace Implementation {

// /////////////////////
// Delta Coder Implementation
// /////////////////////

template<class OBJECT>
DeltaCoder<OBJECT>::DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        Research settings)
    : myDeltaMap()
    , myResearch(settings)
{
    // Ignore ignored deltaMapItems and ones that buffer overrun.
    for (DeltaMapItem map : deltaMap)
    {
        if (map.type == DeltaMapItem::MapType::Ignore)
        {
            Common::Log(
                Common::LogLevel::Informational,
                "DeltaCoder: Map item '",
                map.offsetInfo.name.c_str(),
                "' set to ignore. Ignored.");

            continue;
        }

        if ((map.offsetInfo.offset.value + 4) > sizeof(OBJECT))
        {
            Common::Log(
                Common::LogLevel::Warning,
                "DeltaCoder: Map item '",
                map.offsetInfo.name.c_str(),
                "' accesses memory out of bounds. Ignored.");

            continue;
        }

        myDeltaMap.push_back(map);

        // RAM: TODO: Sort by offet!
    }
}

template<class OBJECT>
void DeltaCoder<OBJECT>::DeltaDecode(
    const OBJECT& base,
    OBJECT& result,
    GameInABox::Common::BitStreamReadOnly& dataIn) const
{
    for (const auto& map : myDeltaMap)
    {
        std::uint32_t baseValue;

        memcpy(
            &baseValue,
            reinterpret_cast<const char *>(&base) + map.offsetInfo.offset.value,
            sizeof(uint32_t));

        auto resultValue = DeltaParse(baseValue, map, dataIn, myResearch);

        memcpy(
            reinterpret_cast<char *>(&result) + map.offsetInfo.offset.value,
            &resultValue,
            sizeof(uint32_t));
    }
}

template<class OBJECT>
void DeltaCoder<OBJECT>::DeltaEncode(
    const OBJECT& base,
    const OBJECT& toDelta,
    GameInABox::Common::BitStream& dataOut) const
{

    for (const auto& map : myDeltaMap)
    {
        std::uint32_t baseValue;
        std::uint32_t toDeltaValue;

        memcpy(
            &baseValue,
            reinterpret_cast<const char *>(&base) + map.offsetInfo.offset.value,
            sizeof(uint32_t));

        memcpy(
            &toDeltaValue,
            reinterpret_cast<const char *>(&toDelta) + map.offsetInfo.offset.value,
            sizeof(uint32_t));

        DeltaCreate(baseValue, toDeltaValue, map, dataOut, myResearch);
    }
}

}}} // namespace

#endif // DELTACODER_TPP
