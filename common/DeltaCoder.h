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

#ifndef IDELTACODER_H
#define IDELTACODER_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

// Argh Includes
#include "DeltaMapItem.h"

// forward delcaration
class IGameStateObject;

class DeltaCoder
{
public:
    DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        std::unique_ptr<IGameStateObject> identity
        bool researchEncodeZeros,
        bool researchEncodeXorDeltas);
    //bool DeltaDecode(const IGameStateObject& base, IGameStateObject& result, BitStreamReadOnly& data) const;
    //bool DeltaEncode(const IGameStateObject& base, const IGameStateObject& toDelta, BitStream& data) const;

private:
    const std::vector<DeltaMapItem> myDeltaMap;    
    const std::unique_ptr<IGameStateObject> myIdentityObject;
    const bool myResearchEncodeZeros;
    const bool myResearchEncodeXorDeltas;

    bool DeltaDecodeItem(
        const IGameStateObject& base,
         IGameStateObject& result, 
         BitStreamReadOnly& dataIn) const;

    bool DeltaEncodeItem(
        const IGameStateObject& base, 
        const IGameStateObject& toDelta, 
        BitStream& dataOut) const;
};

#endif // IDELTACODER_H