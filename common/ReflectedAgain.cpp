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

#include "ReflectedAgain.h"

enum class ReflectionTypes 
{ 
    Method, 
    Float, 
    String     
};

class ReflectionKey
{
public:
    ReflectionKey(ReflectionTypes type, uint8_t index)
    : myType(type)
    , myIndexIntoType(index)
    {
    }
    
    const ReflectionTypes Type() const
    {
        return myType;
    }
    
    const uint8_t Index() const
    {
        return myIndexIntoType;
    }
    
private:
    const ReflectionTypes myType;
    const uint8_t myIndexIntoType;
};

const std::string ReflectedAgain::ReflectionClassName() const 
{          
    return PrivateReflectionClassName();     
}

const std::map<ReflectionKey, std::string> ReflectedAgain::ReflectionList()
{ 
    if (!myPrivateReflectionHasDoneInit) 
    { 
        uint8_t indexCount;
        std::vector<std::string> reflections;
        
        // build my map
        reflections = PrivateReflectionListFloatVariables();
        indexCount = 0;
        for (std::string &name : reflections)
        {
            myReflectionMap[ReflectionKey(ReflectionTypes::Float, indexCount++)] = name;
        }
        
        reflections = PrivateReflectionListStringVariables();
        indexCount = 0;
        for (std::string name : reflections)
        {
            myReflectionMap[ReflectionKey(ReflectionTypes::String, indexCount++)] = name;
        }
        
        reflections = PrivateReflectionListMethods();
        indexCount = 0;
        for (std::string name : reflections)
        {
            myReflectionMap[ReflectionKey(ReflectionTypes::Method, indexCount++)] = name;
        }
        
        InitReflection(); 
        myPrivateReflectionHasDoneInit = true; 
    } 
    
    return myReflectionMap;     
}

bool ReflectedAgain::ReflectionSet(const ReflectionKey key, const float newValue)
{
    // sanity checks
    if (key.Type() != ReflectionTypes::Float)
    {
        return false;
    }    
    
    return PrivateReflectionSet(key.Index(), newValue);
}

bool ReflectedAgain::ReflectionSet(const ReflectionKey key, const std::string newValue)
{
    // sanity checks
    if (key.Type() != ReflectionTypes::String)
    {
        return false;
    }    
    
    return PrivateReflectionSet(key.Index(), newValue);
}

bool ReflectedAgain::ReflectionGet(const ReflectionKey key, float& updateValue) const
{
    // sanity checks
    if (key.Type() != ReflectionTypes::Float)
    {
        return false;
    }    
    
    return PrivateReflectionGet(key.Index(), updateValue);
}

bool ReflectedAgain::ReflectionGet(const ReflectionKey key, std::string& updateValue) const
{
    // sanity checks
    if (key.Type() != ReflectionTypes::String)
    {
        return false;
    }    
    
    return PrivateReflectionGet(key.Index(), updateValue);
}
    bool ReflectedAgain::ReflectionRun(const ReflectionKey key)
{
    // sanity checks
    if (key.Type() != ReflectionTypes::Method)
    {
        return false;
    }    
    
    return PrivateReflectionRun(key.Index());
}