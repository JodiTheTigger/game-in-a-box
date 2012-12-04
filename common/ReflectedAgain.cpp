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

#include <utility>

enum class ReflectionTypes 
{ 
    Method, 
    Float, 
    String     
};

// I could have used std::pair, but I wanted the Type() and Index() accessors
// as opposed to first and second from std::pair.
class ReflectionKeyPrivate
{
public:
    ReflectionKeyPrivate(ReflectionTypes type, uint8_t index)
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
    
    // for std::map
    bool operator<(const ReflectionKeyPrivate& other) const 
    {
        if (this->myIndexIntoType == other.myIndexIntoType)
        {
            return (this->myType < other.myType);
        }
        else
        {
            return (this->myIndexIntoType < other.myIndexIntoType);
        }
    }
    
private:
    const ReflectionTypes myType;
    const uint8_t myIndexIntoType;
};

ReflectionKey::ReflectionKey() : ReflectionKey(nullptr)
{
    
}

ReflectionKey::ReflectionKey(ReflectionKeyPrivate* data) : myPimpl(std::shared_ptr<ReflectionKeyPrivate>(data))
{
    
}

ReflectionKey::~ReflectionKey()
{
    
}

const std::string ReflectedAgain::ReflectionClassName() const 
{          
    return PrivateReflectionClassName();     
}

const std::map<const ReflectionKey, std::string> ReflectedAgain::ReflectionList()
{ 
    if (!myPrivateReflectionHasDoneInit) 
    { 
        uint8_t indexCount;
                
        // build my map (using auto to deal with the constant correctness)
        auto reflectionsFloat = PrivateReflectionListFloatVariables();
        auto reflectionsString = PrivateReflectionListStringVariables();
        auto reflectionsMethod = PrivateReflectionListMethods();
        
        indexCount = 0;
        for (auto name : reflectionsFloat)
        {
           myReflectionMap[ReflectionKey(new ReflectionKeyPrivate(ReflectionTypes::Float, indexCount++))] = name;
        }
        
        indexCount = 0;
        for (auto name : reflectionsString)
        {
            myReflectionMap[ReflectionKey(new ReflectionKeyPrivate(ReflectionTypes::String, indexCount++))] = name;
        }
        
        indexCount = 0;
        for (auto name : reflectionsMethod)
        {
            myReflectionMap[ReflectionKey(new ReflectionKeyPrivate(ReflectionTypes::Method, indexCount++))] = name;
        }
        
        InitReflection(); 
        myPrivateReflectionHasDoneInit = true; 
    } 
    
    return myReflectionMap;     
}

bool ReflectedAgain::ReflectionSet(const ReflectionKey key, float newValue)
{    
    // sanity checks
    if (key->Type() == ReflectionTypes::Float)
    {
        return false;
    }    
    
    return PrivateReflectionSet(key->Index(), newValue);
}

bool ReflectedAgain::ReflectionSet(const ReflectionKey key, std::string newValue)
{
    // sanity checks
    if (key->Type() != ReflectionTypes::String)
    {
        return false;
    }    
    
    return PrivateReflectionSet(key->Index(), newValue);
}

bool ReflectedAgain::ReflectionGet(const ReflectionKey key, float& updateValue) const
{
    // sanity checks
    if (key->Type() != ReflectionTypes::Float)
    {
        return false;
    }    
    
    return PrivateReflectionGet(key->Index(), updateValue);
}

bool ReflectedAgain::ReflectionGet(const ReflectionKey key, std::string& updateValue) const
{
    // sanity checks
    if (key->Type() != ReflectionTypes::String)
    {
        return false;
    }    
    
    return PrivateReflectionGet(key->Index(), updateValue);
}

bool ReflectedAgain::ReflectionRun(const ReflectionKey key)
{
    // sanity checks
    if (key->Type() != ReflectionTypes::Method)
    {
        return false;
    }    
    
    return PrivateReflectionRun(key->Index());
}