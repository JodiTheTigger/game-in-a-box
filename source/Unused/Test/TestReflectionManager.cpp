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

#include <gtest/gtest.h>

#include <algorithm>

#include <Common/IReflected.hpp>
#include <Common/ReflectionManager.hpp>

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

class ReflectedTesterMethodsOnly : public IReflected
{
    REFLECTION_BOILERPLATE(ReflectedTesterMethodsOnly)
    
public:
    ReflectedTesterMethodsOnly()
        : REFLECTION_INIT_MEMBERS
        , myFirst(1)
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
    
    vector<string> things;
    
    float testValue;
    
    // get all the stuff
    things = toTest.GetList();
    
    // call functions, not expecting any exceptions
    testValue = 22;
    EXPECT_FALSE(toTest.ValueGet("Nothing", testValue));
    EXPECT_FALSE(toTest.ValueSet("nothing", 44.2f));
    EXPECT_FALSE(toTest.CallMethod("44"));
    
    EXPECT_EQ(0, things.size());
    EXPECT_EQ(22, testValue);
}

TEST_F(TestReflectionManager, TestAddOneClassList) 
{
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);
    
    things = toTest.GetList();

    // call non-existing functions, not expecting any exceptions    
    EXPECT_EQ(5, things.size());
    EXPECT_NE(end(things), find(begin(things), end(things), "ReflectedTester.FirstProperty"));
    EXPECT_NE(end(things), find(begin(things), end(things), "ReflectedTester.SecondProperty"));
    EXPECT_NE(end(things), find(begin(things), end(things), "ReflectedTester.String1"));
    EXPECT_NE(end(things), find(begin(things), end(things), "ReflectedTester.String2"));
    EXPECT_NE(end(things), find(begin(things), end(things), "ReflectedTester.ResetToZero"));
}

TEST_F(TestReflectionManager, TestAddOneClassTypes) 
{
    ReflectionManager toTest;
    ReflectedType itemType;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);
    
    things = toTest.GetList();

    // call non-existing functions, not expecting any exceptions    
    EXPECT_EQ(5, things.size());
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", itemType));
    EXPECT_EQ(itemType, ReflectedType::Float);
    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", itemType));
    EXPECT_EQ(itemType, ReflectedType::Float);
    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", itemType));
    EXPECT_EQ(itemType, ReflectedType::String);
    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", itemType));
    EXPECT_EQ(itemType, ReflectedType::String);
    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.ResetToZero", itemType));
    EXPECT_EQ(itemType, ReflectedType::Method);
}

TEST_F(TestReflectionManager, TestAddOneClassAllValues) 
{
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);
    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    EXPECT_EQ(1, value1);
    EXPECT_EQ(2, value2);
    EXPECT_EQ("Meh", string1);
    EXPECT_EQ("Bah", string2);    
}

TEST_F(TestReflectionManager, TestAddOneClassSet) 
{
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);
    
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.FirstProperty", 22));    
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.SecondProperty", 44));
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.String1", "Yum"));
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.String2", "Fun"));
    
    // now get the values again
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    EXPECT_EQ(22, value1);
    EXPECT_EQ(44, value2);
    EXPECT_EQ("Yum", string1);
    EXPECT_EQ("Fun", string2); 
}

TEST_F(TestReflectionManager, TestAddOneClassSetMethodGet) 
{
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);
    
    EXPECT_TRUE(toTest.CallMethod("ReflectedTester.ResetToZero")); 
    
    // now get the values again
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    EXPECT_EQ(0, value1);
    EXPECT_EQ(0, value2);
    EXPECT_EQ("No", string1);
    EXPECT_EQ("Way", string2); 
}

TEST_F(TestReflectionManager, TestAddOneClassGetInvalid) 
{
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);
    
    value1 = -8;
    value2 = 4;
    string1 = "No";
    string2 = "Way";
    
    // shouldn't do anything
    EXPECT_FALSE(toTest.CallMethod("ReflectedTester.FirstProperty"));   
    
    // now get the values again
    EXPECT_FALSE(toTest.ValueGet("ReflectedTester.FirstProperty", string1));    
    EXPECT_FALSE(toTest.ValueGet("ReflectedTester.SecondProperty", string2));
    EXPECT_FALSE(toTest.ValueGet("ReflectedTester.String1", value1));
    EXPECT_FALSE(toTest.ValueGet("ReflectedTester.String2", value2));
    
    // values shouldn't have been changed.
    EXPECT_EQ(-8, value1);
    EXPECT_EQ(4, value2);
    EXPECT_EQ("No", string1);
    EXPECT_EQ("Way", string2); 
}

TEST_F(TestReflectionManager, TestAddOneClassSetInvalid) 
{
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    toTest.RegisterClass(firstClass);
    
    // shouldn't do anything
    EXPECT_FALSE(toTest.CallMethod("ReflectedTester.FirstProperty")); 
    
    // now get the values again
    EXPECT_FALSE(toTest.ValueSet("ReflectedTester.FirstProperty", "help"));    
    EXPECT_FALSE(toTest.ValueSet("ReflectedTester.SecondProperty", "me"));
    EXPECT_FALSE(toTest.ValueSet("ReflectedTester.String1", 1));
    EXPECT_FALSE(toTest.ValueSet("ReflectedTester.String2", 2));
        
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    // values shouldn't have been changed.
    EXPECT_EQ(1, value1);
    EXPECT_EQ(2, value2);
    EXPECT_EQ("Meh", string1);
    EXPECT_EQ("Bah", string2); 
}

TEST_F(TestReflectionManager, TestRegisterNullClass) 
{    
    float value1;
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(nullptr);

    toTest.RegisterClass(firstClass);    
    
    things = toTest.GetList();
    value1 = 42;
    EXPECT_FALSE(toTest.ValueGet("Nothing", value1));
    EXPECT_FALSE(toTest.ValueSet("nothing", 44.2f));
    EXPECT_FALSE(toTest.CallMethod("44"));
    
    EXPECT_EQ(0, things.size());
}

TEST_F(TestReflectionManager, TestAddSameClassTwice) 
{    
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());

    // First class should get dropped for the 2nd.
    toTest.RegisterClass(firstClass);
    toTest.RegisterClass(firstClass);
    
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.FirstProperty", 22));    
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.SecondProperty", 44));
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.String1", "Yum"));
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.String2", "Fun"));
    
    // now get the values again
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    EXPECT_EQ(22, value1);
    EXPECT_EQ(44, value2);
    EXPECT_EQ("Yum", string1);
    EXPECT_EQ("Fun", string2); 
}

TEST_F(TestReflectionManager, TestAddSameClassTwiceDifferentInstances) 
{
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstDuplicate;
    shared_ptr<IReflected> secondDuplicate;

    vector<string> things;

    // Let's do it.
    firstDuplicate = shared_ptr<IReflected>(new ReflectedTester());
    secondDuplicate = shared_ptr<IReflected>(new ReflectedTester());

    // last one wins please.
    toTest.RegisterClass(firstDuplicate); 
    toTest.RegisterClass(secondDuplicate); 
    
    // get all the stuff
    things = toTest.GetList();
    
    EXPECT_EQ(5, things.size());
    
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.FirstProperty", 22));    
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.SecondProperty", 44));
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.String1", "Yum"));
    EXPECT_TRUE(toTest.ValueSet("ReflectedTester.String2", "Fun"));
    
    // now get the values again
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    EXPECT_EQ(22, value1);
    EXPECT_EQ(44, value2);
    EXPECT_EQ("Yum", string1);
    EXPECT_EQ("Fun", string2); 
    
    // now make sure the first class didn't get touched.
    auto rawList = firstDuplicate->ReflectionList();
    EXPECT_TRUE(firstDuplicate->ReflectionGet(rawList["FirstProperty"], value1));    
    EXPECT_TRUE(firstDuplicate->ReflectionGet(rawList["SecondProperty"], value2));
    EXPECT_TRUE(firstDuplicate->ReflectionGet(rawList["String1"], string1));
    EXPECT_TRUE(firstDuplicate->ReflectionGet(rawList["String2"], string2));
    
    EXPECT_EQ(1, value1);
    EXPECT_EQ(2, value2);
    EXPECT_EQ("Meh", string1);
    EXPECT_EQ("Bah", string2); 
}

TEST_F(TestReflectionManager, TestAddTwoClasses) 
{
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;
    shared_ptr<ReflectedTesterMethodsOnly> secondClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());
    secondClass = shared_ptr<ReflectedTesterMethodsOnly>(new ReflectedTesterMethodsOnly());

    toTest.RegisterClass(firstClass);
    toTest.RegisterClass(secondClass);
    
    EXPECT_TRUE(toTest.CallMethod("ReflectedTester.ResetToZero")); 
    
    // now get the values again
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    EXPECT_EQ(0, value1);
    EXPECT_EQ(0, value2);
    EXPECT_EQ("No", string1);
    EXPECT_EQ("Way", string2); 
    
    secondClass->UpdateValues(value1, value2);
    
    EXPECT_EQ(1, value1);
    EXPECT_EQ(2, value2);
    
    EXPECT_TRUE(toTest.CallMethod("ReflectedTesterMethodsOnly.ResetToZero"));
    secondClass->UpdateValues(value1, value2);
    
    EXPECT_EQ(0, value1);
    EXPECT_EQ(0, value2);
    
    EXPECT_TRUE(toTest.CallMethod("ReflectedTesterMethodsOnly.ResetToOne"));
    secondClass->UpdateValues(value1, value2);
    
    EXPECT_EQ(1, value1);
    EXPECT_EQ(1, value2);
}

TEST_F(TestReflectionManager, TestAddOneClassDeletePointer) 
{   
    float value1;
    float value2;
    string string1;
    string string2;
    
    ReflectionManager toTest;
    shared_ptr<IReflected> firstClass;

    vector<string> things;

    // Let's do it.
    firstClass = shared_ptr<IReflected>(new ReflectedTester());
    string1 = "Yet";
    string2 = "Again";

    toTest.RegisterClass(firstClass);
    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.FirstProperty", value1));    
    EXPECT_TRUE(toTest.ValueGet("ReflectedTester.SecondProperty", value2));
    
    firstClass.reset();
    
    EXPECT_FALSE(toTest.ValueGet("ReflectedTester.String1", string1));
    EXPECT_FALSE(toTest.ValueGet("ReflectedTester.String2", string2));
    
    EXPECT_EQ(1, value1);
    EXPECT_EQ(2, value2);
    EXPECT_EQ("Yet", string1);
    EXPECT_EQ("Again", string2);    
    
}

}} // namespace
