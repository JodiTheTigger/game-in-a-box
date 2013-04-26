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
#include <tuple>

#include "BuildMacros.hpp"

namespace GameInABox { namespace Common {

// forward declarations
class IReflected;
class ReflectionKey;
enum class ReflectedType;

class ReflectionManager
{
    CLASS_NOCOPY_ASSIGN_MOVE(ReflectionManager)
    
public:
    ReflectionManager();
    
    // If the same type of class gets registered twice the last one wins.
    void RegisterClass(std::weak_ptr<IReflected> reflectedClass);    
    
    std::vector<std::string> GetList() const;
        
    // Gets the value for the argument passed in. Returns false if nothing matches
    // the argument + data type (and leaves value untouched).
    bool ValueGet(const std::string& argument, float& value) const;
    bool ValueGet(const std::string& argument, std::string& value) const;
    bool ValueGet(const std::string& argument, ReflectedType& argumentType) const;
    
    // Sets the value of the argument passed in. Returns true if set
    // false if the argument doesn't match.
    bool ValueSet(const std::string& argument, float newValue);
    bool ValueSet(const std::string& argument, std::string newValue);
    
    // Calls the method passed in. If it doesn't exist, then returns false, otherwise true.
    bool CallMethod(const std::string& method);
    
private:
    std::map<std::string, std::tuple<std::weak_ptr<IReflected>, ReflectionKey>> myStringToClassAndKey;
    
    bool CheckAndGetClassAndKey(
        const std::string& argument,
        std::shared_ptr<IReflected>& reflected, 
        ReflectionKey& key) const;
};

}} // namespace

#endif // REFLECTIONMANAGER_H
