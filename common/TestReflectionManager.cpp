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
    ReflectedTester()
        : myFirst(1)
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

class ReflectedTesterMethodsOnly : public IReflected
{
    REFLECTION_BOILERPLATE(ReflectedTesterMethodsOnly)
    
public:
    ReflectedTesterMethodsOnly()
        : myFirst(1)
        , mySecond(2)
    {
    }
    
    void ResetToZero() { myFirst = 0; mySecond = 0;} 
    void ResetToOne() { myFirst = 1; mySecond = 1;}
    void UpdateValues(float& first, float& second) {first = myFirst; second = mySecond;}    
    
    virtual ~ReflectedTesterMethodsOnly() {}; 
    
private:
    float myFirst;
    float mySecond;        
    
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
    testValue = 22;
    EXPECT_FALSE(toTest.ValueGet("Nothing", testValue));
    EXPECT_FALSE(toTest.ValueSet("nothing", 44.2f));
    EXPECT_FALSE(toTest.CallMethod("44"));
    
    EXPECT_EQ(0, arguments.size());
    EXPECT_EQ(0, methods.size());
    EXPECT_EQ(0, values.size());
    EXPECT_EQ(22, testValue);
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
    string testValue3;
    string testValue4;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);

    // call non-existing functions, not expecting any exceptions
    testValue = 22;
    EXPECT_FALSE(toTest.ValueGet("Nothing", testValue));
    EXPECT_FALSE(toTest.ValueSet("nothing", 44.2f));
    EXPECT_FALSE(toTest.CallMethod("44"));
    
    EXPECT_EQ(2, arguments.size());
    EXPECT_EQ(1, methods.size());
    EXPECT_EQ(2, values.size());
    EXPECT_EQ(22, testValue);
    
    // WTF? This function doesn't make sense anymore! It returns floats and strings.
    values = toTest.GetListValues("");
    
    EXPECT_EQ(4, values.size());
    EXPECT_NE(values.end(), values.find("ReflectedTester.FirstProperty"));
    EXPECT_NE(values.end(), values.find("ReflectedTester.SecondProperty"));
    //EXPECT_NE(values.end(), values.find("ReflectedTester.String1"));
    //EXPECT_NE(values.end(), values.find("ReflectedTester.String2"));     
     /*   
    values = toTest.GetListMethods("");    

    EXPECT_EQ(1, values);
    EXPECT_NE(values.end(), values.find("ReflectedTester.ResetToZero"));
  *//*
    EXPECT_EQ(values[string("ReflectedTester.FirstProperty")], 1.0);
    EXPECT_EQ(values[string("ReflectedTester.SecondProperty")], 2.0);
    EXPECT_EQ(values[string("ReflectedTester.String1")], "Meh");
    EXPECT_EQ(values[string("ReflectedTester.String2")], "Bah");

    /*
    EXPECT_TRUE(toTest.ValueGet("FirstProperty", testValue));
    EXPECT_TRUE(toTest.ValueGet("SecondProperty", testValue2));
    EXPECT_TRUE(toTest.ValueGet("String1", testValue3));
    EXPECT_TRUE(toTest.ValueGet("String2", testValue4));

    EXPECT_EQ(1, testValue);
    EXPECT_EQ(2, testValue2);
    EXPECT_EQ("Meh", testValue3);
    EXPECT_EQ("Bah", testValue4);

    toTest.ValueSet("SecondProperty", 44.2f);

    EXPECT_TRUE(toTest.ValueGet("FirstProperty", testValue));
    EXPECT_TRUE(toTest.ValueGet("SecondProperty", testValue2));

    EXPECT_EQ(1, testValue);
    EXPECT_EQ(44.2, testValue2);

    toTest.CallMethod("ResetToZero");

    EXPECT_TRUE(toTest.ValueGet("FirstProperty", testValue));
    EXPECT_TRUE(toTest.ValueGet("SecondProperty", testValue2));

    EXPECT_EQ(0, testValue);
    EXPECT_EQ(0, testValue2);
    */
    // TODO! test the strings, and split this to its component test cases.
  EXPECT_EQ(0, 1);
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