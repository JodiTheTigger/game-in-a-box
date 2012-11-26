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
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef IREFLECTED_H
#define IREFLECTED_H

#include <vector>
#include <string>

// RAM: TODO! Make set/get/run indexes typesafe somehow, maybe make the varible list a key/index thing
// to typesafe it all. DOn't like it atm when you just return an int. yuk.

class IReflected
{
public:
    std::string ReflectionClassName() const;
    std::vector<std::string> ReflectionListVariables() const;
    std::vector<std::string> ReflectionListMethods() const;
    
    void ReflectionSet(uint8_t index, float newValue);
    float ReflectionGet(uint8_t index) const;
    void ReflectionRun(uint8_t index);
    
    // TODO: deal with this!
    virtual ~IReflected() {};
    
private:
    
    virtual void InitReflection() = 0;
    virtual std::string PrivateReflectionClassName() const = 0;
    virtual std::vector<std::string> PrivateReflectionListVariables() const = 0;
    virtual std::vector<std::string> PrivateReflectionListMethods() const = 0;
    
    virtual void PrivateReflectionSet(uint8_t index, float newValue) = 0;
    virtual float PrivateReflectionGet(uint8_t index) const = 0;
    virtual void PrivateReflectionRun(uint8_t index) = 0; 
};

// ///////////////////
// Reflection Functions
// ///////////////////
std::string IReflected::ReflectionClassName() const 
{ 
    return PrivateReflectionClassName();     
}

std::vector<std::string> IReflected::ReflectionListVariables() const
{ 
    return PrivateReflectionListVariables();     
}

std::vector<std::string> IReflected::ReflectionListMethods() const 
{ 
    return PrivateReflectionListMethods();     
}

void IReflected::ReflectionSet(uint8_t index, float newValue) 
{ 
    PrivateReflectionSet(index, newValue);     
}

float IReflected::ReflectionGet(uint8_t index) const 
{ 
    return PrivateReflectionGet(index);     
}

void IReflected::ReflectionRun(uint8_t index) 
{ 
    PrivateReflectionRun(index);     
} 
    
// ///////////////////
// Reflection Macros
// ///////////////////
#define REFLECTION_VARIABLE(CLASS, NAME_VAR)          \
reflectionSetters.push_back(&CLASS::set##NAME_VAR);     \
reflectionGetters.push_back(&CLASS::get##NAME_VAR);     \
reflectionNamesVariable.push_back(#NAME_VAR);

#define REFLECTION_METHOD(CLASS, NAME_METHOD)          \
reflectionRunners.push_back(&CLASS::NAME_METHOD);      \
reflectionNamesMethods.push_back(#NAME_METHOD);

#define REFLECTION_BOILERPLATE(CLASS)   \
private:\
    \
    typedef void (CLASS::*ReflectionSetter)(float);\
    typedef float (CLASS::*ReflectionGetter)(void) const;\
    typedef void (CLASS::*ReflectionRunner)(void);\
    \
    std::vector<ReflectionSetter> reflectionSetters;\
    std::vector<ReflectionGetter> reflectionGetters; \
    std::vector<ReflectionRunner> reflectionRunners; \
    std::vector<std::string> reflectionNamesVariable;\
    std::vector<std::string> reflectionNamesMethods;         \
    \
    std::string PrivateReflectionClassName() const override\
    {\
        return std::string(#CLASS);\
    }\
    \
    float PrivateReflectionGet(uint8_t index) const override\
    {\
        if (index < reflectionGetters.size())\
        {\
            return (*this.*reflectionGetters[index])();\
        }\
        else\
        {\
            return 0.0;\
        }\
    }\
    \
    void PrivateReflectionSet(uint8_t index, float newValue) override \
    {\
        if (index < reflectionSetters.size())\
        {\
            (*this.*reflectionSetters[index])(newValue);\
        }\
    }\
    \
    void PrivateReflectionRun(uint8_t index) override\
    {\
        if (index < reflectionRunners.size())\
        {\
            (*this.*reflectionRunners[index])();    \
        }\
    }\
    \
    std::vector<std::string> PrivateReflectionListVariables() const override\
    {\
        return reflectionNamesVariable;\
    }\
    \
    std::vector<std::string> PrivateReflectionListMethods() const override\
    {\
        return reflectionNamesMethods;\
    }


#endif // IREFLECTED_H
