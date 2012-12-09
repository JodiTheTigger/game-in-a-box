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

#include "ReflectionManager.h"
#include "IReflected.h"

// NOTES: 
//   I've benchmarked using map:find vs [] for accessing elements. [] is faster up to
//   500k loops. However it isn't a const operations (grrr) so I use find for now. Could
//   change the map access code to a non-const operation for a speedup if it becomes a bottleneck.
//
//   The value set and get functions are pretty much duplicates, with only the data type
//   being the difference. I don't know a way to collapse all that duplicate code. I've
//   tried by exporting it to a helper function but I get the feeling it could be better.

using namespace std;

ReflectionManager::ReflectionManager()
: myStringToClassAndKey(map<string, tuple<shared_ptr<IReflected>, ReflectionKey>>())
{
    
}

// Registers the class, Pointers to null are ignored.
void ReflectionManager::RegisterClass(shared_ptr<IReflected> reflectedClass)
{
    std::map<std::string, ReflectionKey> reflectedData;
    
    if (reflectedClass.get() == nullptr)
    {
        return;
    }
    
    reflectedData = reflectedClass->ReflectionList();
    
    for (auto reflectedItem : reflectedData)
    {
        string nameToUse = reflectedClass->ReflectionClassName() + "." + reflectedItem.first;
        myStringToClassAndKey[nameToUse] = make_tuple(reflectedClass, reflectedItem.second);
    }
}

std::vector<std::string> ReflectionManager::GetList() const
{
    vector<string> result;
    
    for (auto data : myStringToClassAndKey)
    {
        result.push_back(data.first);
    }
    
    return result;
}

bool ReflectionManager::ValueGet(const std::string& argument, float& value) const
{
    ReflectionKey theKey;
    shared_ptr<IReflected> theReflected;
        
    if (CheckAndGetClassAndKey(argument, theReflected, theKey))
    {
        return theReflected->ReflectionGet(theKey, value);
    }
    else
    {
        return false;
    }
}

bool ReflectionManager::ValueGet(const std::string& argument, std::string& value) const
{ 
    ReflectionKey theKey;
    shared_ptr<IReflected> theReflected;
        
    if (CheckAndGetClassAndKey(argument, theReflected, theKey))
    {
        return theReflected->ReflectionGet(theKey, value);
    }
    else
    {
        return false;
    }
}

bool ReflectionManager::ValueGet(const std::string& argument, ReflectedType& argumentType) const
{        
    ReflectionKey theKey;
    shared_ptr<IReflected> theReflected;
        
    if (CheckAndGetClassAndKey(argument, theReflected, theKey))
    {
        argumentType = theReflected->ReflectionType(theKey);
        return true;
    }
    else
    {
        return false;
    }
}

bool ReflectionManager::ValueSet(const std::string& argument, float newValue)
{
    ReflectionKey theKey;
    shared_ptr<IReflected> theReflected;
        
    if (CheckAndGetClassAndKey(argument, theReflected, theKey))
    {
        return theReflected->ReflectionSet(theKey, newValue);
    }
    else
    {
        return false;
    }
}

bool ReflectionManager::ValueSet(const std::string& argument, std::string newValue)
{
    ReflectionKey theKey;
    shared_ptr<IReflected> theReflected;
        
    if (CheckAndGetClassAndKey(argument, theReflected, theKey))
    {
        return theReflected->ReflectionSet(theKey, newValue);
    }
    else
    {
        return false;
    }
}

bool ReflectionManager::CallMethod(const std::string& method)
{
    ReflectionKey theKey;
    shared_ptr<IReflected> theReflected;
        
    if (CheckAndGetClassAndKey(method, theReflected, theKey))
    {
        return theReflected->ReflectionRun(theKey);
    }
    else
    {
        return false;
    }
}

bool ReflectionManager::CheckAndGetClassAndKey(
    const std::string& argument,
    std::shared_ptr<IReflected>& theReflected, 
    ReflectionKey& theKey) const
{        
    if (myStringToClassAndKey.find(argument) == myStringToClassAndKey.end())
    {
        // no key
        return false;
    }
    
    // Argh, map[index] isn't const as it creates new items if the key isn't found!
    tie(theReflected, theKey) = myStringToClassAndKey.find(argument)->second;
    
    return true;
}