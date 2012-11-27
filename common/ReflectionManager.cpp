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

ReflectionManager::ReflectionManager(std::map<std::string, float> defaultValues)
: myUnusedDefaultSettings(defaultValues)
{
    // TODO!
}

// Registers the class, and sets any default values already set by
// either the constructor's std::map.
void ReflectionManager::RegisterClass(std::shared_ptr<IReflected> reflectedClass)
{
    // TODO!
}

std::vector<std::string> ReflectionManager::GetListArguments(std::string containing) const
{
    // TODO!
}
std::vector<std::string> ReflectionManager::GetListMethods(std::string containing) const
{
    // TODO!
}
std::map<std::string, float> ReflectionManager::GetListValues(std::string containing)
{
    // TODO!
}

// Gets the value for the argument passed in. Returns 0.0 if the argument does
// not match anything registered.
float ReflectionManager::ValueGet(std::string argument) const
{
    // TODO!
}

// Sets the value of the argument passed in. Silently ignores arguments that
// do not match anything registered.
void ReflectionManager::ValueSet(std::string argument, float newValue)
{
    // TODO!
}

// Calls the method passed in. If it doesn't exist, then it is silently ignored.
void ReflectionManager::CallMethod(std::string method)
{
    // TODO!
}

