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

#ifndef USING_PRECOMPILED_HEADERS
#include <utility>
#else
#include "Common/PrecompiledHeaders.hpp"
#endif

#include "IReflected.hpp"

namespace GameInABox { namespace Unused {
// NOTES:
//   I could have used std::pair for ReflectionKeyPrivate, but I wanted the Type() and 
//   Index() accessors as opposed to first and second from std::pair.
//
//   I could easily support dynamically named classes by having a derived class that you pass in
//   the class' name via the constructor and return that as opposed to a static string. Something
//   to do if the need arises, but not now as I don't need it. No point over-engineering.
class ReflectionKeyPrivate
{
public:
    ReflectionKeyPrivate(ReflectedType type, uint8_t index)
    : myType(type)
    , myIndexIntoType(index)
    {
    }
    
    ReflectedType Type() const
    {
        return myType;
    }
    
    uint8_t Index() const
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
    const ReflectedType myType;
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
IReflected::IReflected() 
    : myPrivateReflectionHasDoneInit(false)
    , myReflectionMap()
{
}

ReflectedType IReflected::ReflectionType(const ReflectionKey& key) const
{
    return  key->Type();
}

const std::string IReflected::ReflectionClassName() const 
{          
    return PrivateReflectionClassName();     
}

const std::map<std::string, ReflectionKey>& IReflected::ReflectionList()
{ 
    if (!myPrivateReflectionHasDoneInit) 
    { 
        uint8_t indexCount;
        
        InitReflection(); 
                
        // build my map (using auto to deal with the constant correctness)
        auto reflectionsFloat = PrivateReflectionListFloatVariables();
        auto reflectionsString = PrivateReflectionListStringVariables();
        auto reflectionsMethod = PrivateReflectionListMethods();
        
        indexCount = 0;
        for (auto name : reflectionsFloat)
        {
           myReflectionMap[name] = ReflectionKey(new ReflectionKeyPrivate(ReflectedType::Float, indexCount++));
        }
        
        indexCount = 0;
        for (auto name : reflectionsString)
        {
            myReflectionMap[name] = ReflectionKey(new ReflectionKeyPrivate(ReflectedType::String, indexCount++));
        }
        
        indexCount = 0;
        for (auto name : reflectionsMethod)
        {
            myReflectionMap[name] = ReflectionKey(new ReflectionKeyPrivate(ReflectedType::Method, indexCount++));
        }
        
        myPrivateReflectionHasDoneInit = true; 
    } 
    
    return myReflectionMap;     
}

bool IReflected::ReflectionSet(const ReflectionKey& key, float newValue)
{    
    // sanity checks
    if (key->Type() != ReflectedType::Float)
    {
        return false;
    }    
    
    return PrivateReflectionSet(key->Index(), newValue);
}

bool IReflected::ReflectionSet(const ReflectionKey& key, std::string newValue)
{
    // sanity checks
    if (key->Type() != ReflectedType::String)
    {
        return false;
    }    
    
    return PrivateReflectionSet(key->Index(), newValue);
}

bool IReflected::ReflectionGet(const ReflectionKey& key, float& updateValue) const
{
    // sanity checks
    if (key->Type() != ReflectedType::Float)
    {
        return false;
    }    
    
    return PrivateReflectionGet(key->Index(), updateValue);
}

bool IReflected::ReflectionGet(const ReflectionKey& key, std::string& updateValue) const
{
    // sanity checks
    if (key->Type() != ReflectedType::String)
    {
        return false;
    }    
    
    return PrivateReflectionGet(key->Index(), updateValue);
}

bool IReflected::ReflectionRun(const ReflectionKey& key)
{
    // sanity checks
    if (key->Type() != ReflectedType::Method)
    {
        return false;
    }    
    
    return PrivateReflectionRun(key->Index());
}

}} // namespace
