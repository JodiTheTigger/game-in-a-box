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

#ifndef REFLECTIONMANAGER_H
#define REFLECTIONMANAGER_H

#include <map>
#include <string>
#include <memory>
#include <vector>

#include "BuildMacros.h"

// forward declarations
class IReflected;

class ReflectionManager
{
    CLASS_NOCOPY_NOASSIGN(ReflectionManager)
    
public:
    //ReflectionManager() : ReflectionManager(std::map<std::string, float> empty) {}
    ReflectionManager(std::map<std::string, float> defaultValues);
    
    // Registers the class, and sets any default values already set by
    // either the constructor's std::map.
    void RegisterClass(std::shared_ptr<IReflected> reflectedClass);
    
    std::vector<std::string> GetListArguments(std::string containing) const;
    std::vector<std::string> GetListMethods(std::string containing) const;
    std::map<std::string, float> GetListValues(std::string containing);
    
    // Gets the value for the argument passed in. Returns 0.0 if the argument does
    // not match anything registered.
    float ValueGet(std::string argument) const;
    
    // Sets the value of the argument passed in. Silently ignores arguments that
    // do not match anything registered.
    void ValueSet(std::string argument, float newValue);
    
    // Calls the method passed in. If it doesn't exist, then it is silently ignored.
    void CallMethod(std::string method);
    
private:
    std::map<std::string, float> myUnusedDefaultSettings;
};

#endif // REFLECTIONMANAGER_H
