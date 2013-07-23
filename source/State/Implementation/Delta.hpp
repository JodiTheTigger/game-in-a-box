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


/*
void Code(
     uint32_t base,
     uint32_t target,
     const DeltaMapItem& map,
     GameInABox::Common::BitStream& out,
     bool doZeros,
     bool doXor);*/

// /////////////////////
// Delta Coder Interface
// /////////////////////
/*template<class OBJECT>
class DeltaCoder
{
public:
    DeltaCoder(
        std::vector<DeltaMapItem> deltaMap,
        OBJECT identity,
        bool researchEncodeZeros,
        bool researchEncodeXorDeltas);

 // if base == nullptr use the identity
     void DeltaDecode(
         const OBJECT* base,
         OBJECT& result,
         GameInABox::Common::BitStreamReadOnly& dataIn) const;

     // if base == nullptr use the identity
     // Returns true if base == toDelta, otherwise false.
     bool DeltaEncode(
         const OBJECT* base,
         const OBJECT& toDelta,
         GameInABox::Common::BitStream& dataOut) const;

private:
    const std::vector<DeltaMapItem> myDeltaMap;
    const OBJECT myIdentityObject;
    const bool myResearchEncodeZeros;
    const bool myResearchEncodeXorDeltas;
};*/


#endif // DELTA_HPP
