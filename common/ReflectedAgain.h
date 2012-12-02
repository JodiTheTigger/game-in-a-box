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

#ifndef REFLECTEDAGAIN_H
#define REFLECTEDAGAIN_H

#include <string>
#include <map>
#include <vector>

// forward declarations:
class ReflectionKey;

class ReflectedAgain
{
public:
    const std::string ReflectionClassName() const;
    const std::map<const ReflectionKey, std::string> ReflectionList();
    
    // Returns true if successful, false if the key doesn't match a known functions
    // with the correct data type.
    bool ReflectionSet(const ReflectionKey key, const float newValue);
    bool ReflectionSet(const ReflectionKey key, const std::string newValue);
    bool ReflectionGet(const ReflectionKey key, float& updateValue) const;
    bool ReflectionGet(const ReflectionKey key, std::string& updateValue) const;
    bool ReflectionRun(const ReflectionKey key);
    
    // I could be using reference counted pointers to keep track
    // of IReflected classes, therefore it means the class could be
    // deleted via it's base class pointer (IReflected). So following that
    // I need a public virtual destructor.
    // http://www.gotw.ca/publications/mill18.htm
    virtual ~ReflectedAgain() {};
    
private:
    bool myPrivateReflectionHasDoneInit = false;
    std::map<const ReflectionKey, std::string> myReflectionMap;
    
    virtual void InitReflection() = 0;
    virtual const std::string PrivateReflectionClassName() const = 0;
    virtual const std::vector<std::string> PrivateReflectionListFloatVariables() const = 0;
    virtual const std::vector<const std::string> PrivateReflectionListStringVariables() const = 0;
    virtual const std::vector<const std::string> PrivateReflectionListMethods() const = 0;
    
    virtual bool PrivateReflectionSet(const uint8_t index, const float newValue) = 0;
    virtual bool PrivateReflectionSet(const uint8_t index, const std::string newValue) = 0;
    virtual bool PrivateReflectionGet(const uint8_t index, float& updateValue) const = 0;
    virtual bool PrivateReflectionGet(const uint8_t index, std::string& updateValue) const = 0;
    virtual bool PrivateReflectionRun(const uint8_t index) = 0; 
};

#endif // REFLECTEDAGAIN_H
