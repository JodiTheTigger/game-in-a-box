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

#ifndef IREFLECTED_H
#define IREFLECTED_H

#include <string>
#include <map>
#include <vector>
#include <memory>

// I want ReflectionKey values for my interface,
// therefore I cannot use a forward reference.
#include "ReflectionKey.h"

// Reflection notes:
// =================
// class ReflectionKey;
//    Use the pimpl idiom for the reflection key. It's basically a handle that contains data useful for the reflection class.
//
// virtual ~IReflected() {};
//    I was going to make this private, as making a smart pointer
//    of the derived class (but as IReflected) will still delete
//    everything properly. However since I cannot enforce that here
//    I have to be safe and allow it to be a virtual destructor
//    so that the base class can be deleted.
//    http://www.gotw.ca/publications/mill18.htm
//    http://www.cs.brown.edu/~jwicks/boost/libs/smart_ptr/sp_techniques.html#abstract
//
// bool ReflectionSet(const ReflectionKey key, float newValue);
//    When passing in values that are going to be copied, don't pass in
//    const, or const references. C++11's move semantic makes stuff fast
//    again.
//    http://cpptruths.blogspot.co.nz/2012/03/rvalue-references-in-constructor-when.html
//    http://stackoverflow.com/questions/10231349/are-the-days-of-passing-const-stdstring-as-a-parameter-over (first two answers)

class IReflected
{
public:
	IReflected();
    const std::string ReflectionClassName() const;
    const std::map<std::string, ReflectionKey> ReflectionList();
    
    // Returns true if successful, false if the key doesn't have a match for that data type.
    bool ReflectionSet(const ReflectionKey& key, float newValue);
    bool ReflectionSet(const ReflectionKey& key, std::string newValue);
    bool ReflectionGet(const ReflectionKey& key, float& updateValue) const;
    bool ReflectionGet(const ReflectionKey& key, std::string& updateValue) const;
    bool ReflectionRun(const ReflectionKey& key);

    virtual ~IReflected() {};
    
private:
    bool myPrivateReflectionHasDoneInit;
    std::map<std::string, ReflectionKey> myReflectionMap;
    
    virtual void InitReflection() = 0;
    virtual const std::string PrivateReflectionClassName() const = 0;
    virtual const std::vector<std::string>& PrivateReflectionListFloatVariables() const = 0;
    virtual const std::vector<std::string>& PrivateReflectionListStringVariables() const = 0;
    virtual const std::vector<std::string>& PrivateReflectionListMethods() const = 0;
    
    virtual bool PrivateReflectionSet(const uint8_t index, float newValue) = 0;
    virtual bool PrivateReflectionSet(const uint8_t index, std::string newValue) = 0;
    virtual bool PrivateReflectionGet(const uint8_t index, float& updateValue) const = 0;
    virtual bool PrivateReflectionGet(const uint8_t index, std::string& updateValue) const = 0;
    virtual bool PrivateReflectionRun(const uint8_t index) = 0; 
};

// ///////////////////
// Reflection Macros
// ///////////////////
#define REFLECTION_VARIABLE_FLOAT(CLASS, NAME_VAR)          \
reflectionFloatSetters.push_back(&CLASS::NAME_VAR##Set);     \
reflectionFloatGetters.push_back(&CLASS::NAME_VAR##Get);     \
reflectionNamesFloatVariable.push_back(#NAME_VAR);

#define REFLECTION_VARIABLE_STRING(CLASS, NAME_VAR)          \
reflectionStringSetters.push_back(&CLASS::NAME_VAR##Set);     \
reflectionStringGetters.push_back(&CLASS::NAME_VAR##Get);     \
reflectionNamesStringVariable.push_back(#NAME_VAR);

#define REFLECTION_METHOD(CLASS, NAME_METHOD)          \
reflectionRunners.push_back(&CLASS::NAME_METHOD);     \
reflectionNamesMethods.push_back(#NAME_METHOD);      

#define REFLECTION_BOILERPLATE(CLASS)   \
private:   \
    typedef void (CLASS::*ReflectionFloatSetter)(float);   \
    typedef float (CLASS::*ReflectionFloatGetter)(void) const;   \
    typedef void (CLASS::*ReflectionStringSetter)(std::string);   \
    typedef std::string (CLASS::*ReflectionStringGetter)(void) const;   \
    typedef void (CLASS::*ReflectionRunner)(void);   \
       \
    std::vector<ReflectionFloatSetter> reflectionFloatSetters;   \
    std::vector<ReflectionFloatGetter> reflectionFloatGetters;    \
    std::vector<ReflectionStringSetter> reflectionStringSetters;   \
    std::vector<ReflectionStringGetter> reflectionStringGetters;    \
    std::vector<ReflectionRunner> reflectionRunners;    \
   \
    std::vector<std::string> reflectionNamesFloatVariable;   \
    std::vector<std::string> reflectionNamesStringVariable;   \
    std::vector<std::string> reflectionNamesMethods;          \
       \
    const std::string PrivateReflectionClassName() const override   \
    {   \
        return std::string(#CLASS);   \
    }   \
       \
    const std::vector<std::string>& PrivateReflectionListFloatVariables() const override   \
    {   \
        return reflectionNamesFloatVariable;   \
    }   \
       \
    const std::vector<std::string>& PrivateReflectionListStringVariables() const override   \
    {   \
        return reflectionNamesStringVariable;   \
    }   \
       \
    const std::vector<std::string>& PrivateReflectionListMethods() const override   \
    {   \
        return reflectionNamesMethods;   \
    }   \
       \
    bool PrivateReflectionSet(const uint8_t index, const float newValue) override    \
    {   \
        if (index < reflectionFloatSetters.size())   \
        {   \
            (*this.*reflectionFloatSetters[index])(newValue);   \
            return true;   \
        }   \
        else   \
        {   \
            return false;   \
        }   \
    }   \
       \
    bool PrivateReflectionSet(const uint8_t index, const std::string newValue) override    \
    {   \
        if (index < reflectionStringSetters.size())   \
        {   \
            (*this.*reflectionStringSetters[index])(newValue);   \
            return true;   \
        }   \
        else   \
        {   \
            return false;   \
        }   \
    }   \
       \
    bool PrivateReflectionGet(const uint8_t index, float& updateValue) const override   \
    {   \
        if (index < reflectionFloatGetters.size())   \
        {   \
            updateValue = (*this.*reflectionFloatGetters[index])();   \
            return true;   \
        }   \
        else   \
        {   \
            return false;   \
        }   \
    }   \
       \
    bool PrivateReflectionGet(const uint8_t index, std::string& updateValue) const override   \
    {   \
        if (index < reflectionStringGetters.size())   \
        {   \
            updateValue = (*this.*reflectionStringGetters[index])();   \
            return true;   \
        }   \
        else   \
        {   \
            return false;   \
        }   \
    }   \
       \
    bool PrivateReflectionRun(uint8_t index) override   \
    {   \
        if (index < reflectionRunners.size())   \
        {   \
            (*this.*reflectionRunners[index])();   \
            return true;       \
        }   \
        else   \
        {   \
            return false;   \
        }   \
    }

#endif // IREFLECTED_H
