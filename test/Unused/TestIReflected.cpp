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

#include <algorithm>
#include <gtest/gtest.h>

#include <IReflected.hpp>

using namespace std;

namespace GameInABox { namespace Unused {

// I really should just use google mock.
class ReflectedTester : public IReflected
{
    REFLECTION_BOILERPLATE(ReflectedTester)
    
public:
    ReflectedTester()
        : REFLECTION_INIT_MEMBERS
        , myFirst(1)
        , mySecond(2)
        , myString1("Meh")
        , myString2("Bah")
    {
    }
    // floats
    float FirstPropertyGet() const {return myFirst;}
    void FirstPropertySet(float toSet) {myFirst = toSet;}
    
    float SecondPropertyGet() const {return mySecond;}
    void SecondPropertySet(float toSet) {mySecond = toSet;}
    
    // string
    std::string String1Get() const {return myString1;}
    void String1Set(std::string toSet) {myString1 = toSet;}
    
    std::string String2Get() const {return myString2;}
    void String2Set(std::string toSet) {myString2 = toSet;}
    
    // methods
    void ResetToZero() { myFirst = 0; mySecond = 0; myString1 = "No"; myString2 = "Way"; }     
    
    virtual ~ReflectedTester() {}; 
    
private:
    float myFirst;
    float mySecond;  
    std::string myString1;
    std::string myString2;
    
    void InitReflection() override
    {
        REFLECTION_VARIABLE_FLOAT(ReflectedTester, FirstProperty);
        REFLECTION_VARIABLE_FLOAT(ReflectedTester, SecondProperty);
        
        REFLECTION_VARIABLE_STRING(ReflectedTester, String1);
        REFLECTION_VARIABLE_STRING(ReflectedTester, String2);
        
        REFLECTION_METHOD(ReflectedTester, ResetToZero);  
    }   
};

class TestIReflected : public ::testing::Test 
{
};

TEST_F(TestIReflected, TestClassName) 
{
  ReflectedTester toTest;
  string theName;
  
  theName = toTest.ReflectionClassName(); 
  
  EXPECT_EQ(string("ReflectedTester"), theName);
}

TEST_F(TestIReflected, TestList) 
{
  ReflectedTester toTest;
  auto relfectionMap = toTest.ReflectionList();
  
  EXPECT_EQ(5, relfectionMap.size());
  EXPECT_NE(end(relfectionMap), relfectionMap.find("FirstProperty"));
  EXPECT_NE(end(relfectionMap), relfectionMap.find("SecondProperty"));
  EXPECT_NE(end(relfectionMap), relfectionMap.find("String1"));
  EXPECT_NE(end(relfectionMap), relfectionMap.find("String2"));
  EXPECT_NE(end(relfectionMap), relfectionMap.find("ResetToZero"));
}

TEST_F(TestIReflected, TestTypes) 
{
  ReflectedTester toTest;
  auto relfectionMap = toTest.ReflectionList();
  
  EXPECT_EQ(ReflectedType::Float, toTest.ReflectionType(relfectionMap["FirstProperty"]));
  EXPECT_EQ(ReflectedType::Float, toTest.ReflectionType(relfectionMap["SecondProperty"]));
  EXPECT_EQ(ReflectedType::String, toTest.ReflectionType(relfectionMap["String1"]));
  EXPECT_EQ(ReflectedType::String, toTest.ReflectionType(relfectionMap["String2"]));
  EXPECT_EQ(ReflectedType::Method, toTest.ReflectionType(relfectionMap["ResetToZero"]));
}

TEST_F(TestIReflected, TestGet) 
{
    ReflectedTester toTest;
    float result1;
    float result2;
    std::string string1;
    std::string string2;
    
    auto relfectionMap = toTest.ReflectionList();
    
    toTest.ReflectionGet(relfectionMap["FirstProperty"], result1);
    toTest.ReflectionGet(relfectionMap["SecondProperty"], result2);
    toTest.ReflectionGet(relfectionMap["String1"], string1);
    toTest.ReflectionGet(relfectionMap["String2"], string2);
    
    EXPECT_EQ(result1, 1.0f);
    EXPECT_EQ(result2, 2.0f);  
    EXPECT_EQ(string1, "Meh");  
    EXPECT_EQ(string2, "Bah");  
}

TEST_F(TestIReflected, TestSetThenGet) 
{
    ReflectedTester toTest;  
    float result1;
    float result2; 
    std::string string1;
    std::string string2;
    
    auto relfectionMap = toTest.ReflectionList();
    
    toTest.ReflectionSet(relfectionMap["FirstProperty"], 5);
    toTest.ReflectionSet(relfectionMap["SecondProperty"], 10);
    toTest.ReflectionSet(relfectionMap["String1"], "yea!");
    toTest.ReflectionSet(relfectionMap["String2"], "dude!");
    
    toTest.ReflectionGet(relfectionMap["FirstProperty"], result1);
    toTest.ReflectionGet(relfectionMap["SecondProperty"], result2);
    toTest.ReflectionGet(relfectionMap["String1"], string1);
    toTest.ReflectionGet(relfectionMap["String2"], string2);
    
    EXPECT_EQ(result1, 5.0f);
    EXPECT_EQ(result2, 10.0f);
    EXPECT_EQ(string1, "yea!");  
    EXPECT_EQ(string2, "dude!");  
}

TEST_F(TestIReflected, TestSetMethodGet) 
{
    ReflectedTester toTest;  
    float result1;
    float result2;  
    std::string string1;
    std::string string2;
    
    auto relfectionMap = toTest.ReflectionList();
    
    toTest.ReflectionSet(relfectionMap["FirstProperty"], 5);
    toTest.ReflectionSet(relfectionMap["SecondProperty"], 10);
    toTest.ReflectionSet(relfectionMap["String1"], "yea!");    
    toTest.ReflectionSet(relfectionMap["String2"], "dude!");
    
    toTest.ReflectionRun(relfectionMap["ResetToZero"]);
    
    toTest.ReflectionGet(relfectionMap["String1"], string1);
    toTest.ReflectionGet(relfectionMap["String2"], string2);
    
    toTest.ReflectionGet(relfectionMap["FirstProperty"], result1);
    toTest.ReflectionGet(relfectionMap["SecondProperty"], result2);
    
    EXPECT_EQ(result1, 0.0f);
    EXPECT_EQ(result2, 0.0f); 
    EXPECT_EQ(string1, "No");  
    EXPECT_EQ(string2, "Way");  
}

TEST_F(TestIReflected, TestOutOfBounds) 
{
    ReflectedTester toTest;
    float result1;
    float result2;
    std::string string1;
    std::string string2;
        
    auto relfectionMap = toTest.ReflectionList(); 
    
    // we don't handle nullptr for the ReflectionKey reference, and I'm not going to test for them either.
    
    // getting the wrong data types
    EXPECT_EQ(toTest.ReflectionGet(relfectionMap["FirstProperty"], string1), false);
    EXPECT_EQ(toTest.ReflectionGet(relfectionMap["SecondProperty"], string2), false);
    EXPECT_EQ(toTest.ReflectionGet(relfectionMap["String1"], result1), false);
    EXPECT_EQ(toTest.ReflectionGet(relfectionMap["String2"], result2), false);
    
    EXPECT_EQ(toTest.ReflectionSet(relfectionMap["FirstProperty"], string1), false);
    EXPECT_EQ(toTest.ReflectionSet(relfectionMap["SecondProperty"], string2), false);
    EXPECT_EQ(toTest.ReflectionSet(relfectionMap["String1"], result1), false);
    EXPECT_EQ(toTest.ReflectionSet(relfectionMap["String2"], result2), false);
    EXPECT_EQ(toTest.ReflectionRun(relfectionMap["String2"]), false);
    EXPECT_EQ(toTest.ReflectionRun(relfectionMap["FirstProperty"]), false);    
    
    
    toTest.ReflectionGet(relfectionMap["FirstProperty"], result1);
    toTest.ReflectionGet(relfectionMap["SecondProperty"], result2);
    toTest.ReflectionGet(relfectionMap["String1"], string1);
    toTest.ReflectionGet(relfectionMap["String2"], string2);
    
    
    EXPECT_EQ(result1, 1.0f);
    EXPECT_EQ(result2, 2.0f);  
    EXPECT_EQ(string1, "Meh");  
    EXPECT_EQ(string2, "Bah"); 
}

}} // namespace
