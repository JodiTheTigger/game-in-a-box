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

using namespace std;

ReflectionManager::ReflectionManager()
: myStringToClassAndIndex(map<string, tuple<shared_ptr<IReflected>, uint8_t>>())
{
    // TODO!
    
}

// Registers the class, and sets any default values already set by
// either the constructor's std::map. Pointers to null are ignored.
void ReflectionManager::RegisterClass(shared_ptr<IReflected> reflectedClass)
{
    // TODO!
}

std::vector<std::string> ReflectionManager::GetList() const
{
    // TODO!
    return vector<string>();
}

bool ReflectionManager::ValueGet(const std::string& argument, float& value) const
{
    /* new reflection class: rewrite!
    map<string, std::tuple<shared_ptr<IReflected>, uint8_t>>::const_iterator findIterator;
    
    // using find instead of the [], as find can be const.
    findIterator = myStringToClassAndIndex.find(argument);
        
    if (findIterator != myStringToClassAndIndex.end())
    {        
        shared_ptr<IReflected> target;
        uint8_t targetIndex;
        
        tie(target, targetIndex) = (*findIterator).second;
        
        return target->ReflectionGet(targetIndex);
    }
    else*/
    {
        return false;
    }
}

bool ReflectionManager::ValueGet(const std::string& argument, std::string& value) const
{    
    // RAM: TODO!
    return false;
}

bool ReflectionManager::ValueGet(const std::string& argument, ReflectedType& argumentType) const
{    
    // RAM: TODO!
    return false;
}

bool ReflectionManager::ValueSet(const std::string& argument, float newValue)
{
    // TODO!
    return false;
}

bool ReflectionManager::ValueSet(const std::string& argument, std::string newValue)
{
    // TODO!
    return false;
}

// Calls the method passed in. If it doesn't exist, then it is silently ignored.
bool ReflectionManager::CallMethod(const std::string& method)
{
    // TODO!
    return false;
}

