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

#include "gtest/gtest.h"

#include "IReflected.h"
#include "ReflectionManager.h"

using namespace std;

// I really should just use google mock.
class ReflectedTester : public IReflected
{
    REFLECTION_BOILERPLATE(ReflectedTester)
    
public:
    float FirstPropertyGet() const {return myFirst;}
    void FirstPropertySet(float toSet) {myFirst = toSet;}
    float SecondPropertyGet() const {return mySecond;}
    void SecondPropertySet(float toSet) {mySecond = toSet;}
    void ResetToZero() { myFirst = 0; mySecond = 0;}   
    
    virtual ~ReflectedTester() {}; 
    
private:
    float myFirst = 1;
    float mySecond = 2;        
    
    void InitReflection() override
    {
        REFLECTION_VARIABLE(ReflectedTester, FirstProperty);
        REFLECTION_VARIABLE(ReflectedTester, SecondProperty);
        REFLECTION_METHOD(ReflectedTester, ResetToZero);  
    }   
};

class ReflectedTesterMethodsOnly : public IReflected
{
    REFLECTION_BOILERPLATE(ReflectedTesterMethodsOnly)
    
public:
    void ResetToZero() { myFirst = 0; mySecond = 0;} 
    void ResetToOne() { myFirst = 1; mySecond = 1;}
    void UpdateValues(float& first, float& second) {first = myFirst; second = mySecond;}    
    
    virtual ~ReflectedTesterMethodsOnly() {}; 
    
private:
    float myFirst = 1;
    float mySecond = 2;        
    
    void InitReflection() override
    {
        REFLECTION_METHOD(ReflectedTesterMethodsOnly, ResetToZero);  
        REFLECTION_METHOD(ReflectedTesterMethodsOnly, ResetToOne);  
    }   
};

class TestReflectionManager : public ::testing::Test 
{
};


TEST_F(TestReflectionManager, TestEmptyManager) 
{
    ReflectionManager toTest;
    
    vector<string> arguments;
    vector<string> methods;
    map<string, float> values;
    
    float testValue;
    
    // get all the stuff
    arguments = toTest.GetListArguments(string(""));
    methods = toTest.GetListMethods(string(""));
    values = toTest.GetListValues(string(""));
    
    // call functions, not expecting any exceptions
    testValue = toTest.ValueGet("Nothing");
    toTest.ValueSet("nothing", 44.2f);
    toTest.CallMethod("44");
    
    EXPECT_EQ(0, arguments.size());
    EXPECT_EQ(0, methods.size());
    EXPECT_EQ(0, values.size());
    EXPECT_EQ(0, testValue);
}

TEST_F(TestReflectionManager, TestAddOneClass) 
{
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> arguments;
    vector<string> methods;
    map<string, float> values;
    
    float testValue;
    float testValue2;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);

    // call non-existing functions, not expecting any exceptions
    testValue = toTest.ValueGet("Nothing");
    toTest.ValueSet("nothing", 44.2f);
    toTest.CallMethod("44");
    
    EXPECT_EQ(2, arguments.size());
    EXPECT_EQ(1, methods.size());
    EXPECT_EQ(2, values.size());
    EXPECT_EQ(0, testValue);

    EXPECT_EQ(string("ReflectedTester.FirstProperty"), arguments[0]);
    EXPECT_EQ(string("ReflectedTester.SecondProperty"), arguments[1]);
    EXPECT_EQ(string("ReflectedTester.ResetToZero"), methods[0]);

    EXPECT_EQ(values[string("ReflectedTester.FirstProperty")], 1.0);
    EXPECT_EQ(values[string("ReflectedTester.SecondProperty")], 2.0);

    testValue = toTest.ValueGet("FirstProperty");
    testValue2 = toTest.ValueGet("SecondProperty");

    EXPECT_EQ(1, testValue);
    EXPECT_EQ(2, testValue2);

    toTest.ValueSet("SecondProperty", 44.2f);

    testValue = toTest.ValueGet("FirstProperty");
    testValue2 = toTest.ValueGet("SecondProperty");

    EXPECT_EQ(1, testValue);
    EXPECT_EQ(44.2, testValue2);

    toTest.CallMethod("ResetToZero");

    testValue = toTest.ValueGet("FirstProperty");
    testValue2 = toTest.ValueGet("SecondProperty");

    EXPECT_EQ(0, testValue);
    EXPECT_EQ(0, testValue2);
}

TEST_F(TestReflectionManager, TestRegisterNullClass) 
{
    // is that even possible with shared pointers?
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestAddSameClassTwice) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestAddSameClassTwiceDifferentInstances) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestSetupDefaultsThenRegister) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestAddTwoClasses) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestSuggestionsEmptyString) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestSuggestionsOneChar) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestSuggestionsOneCharNoMatch) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestSuggestionsTwoChar) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestSuggestionsTwoCharNoMatch) 
{
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestSuggestionsOnlyOneMatch) 
{
  EXPECT_EQ(0, 1);
}