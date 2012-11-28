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

#include "IReflected.h"
#include "gtest/gtest.h"

#include "IReflected.h"

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

TEST_F(TestIReflected, TestMethodList) 
{
  ReflectedTester toTest;
  vector<string> theList;
  
  theList = toTest.ReflectionListMethods(); 
  
  EXPECT_EQ(1, theList.size());
  EXPECT_EQ(string("ResetToZero"), theList[0]);
}

TEST_F(TestIReflected, TestVariableList) 
{
  ReflectedTester toTest;
  vector<string> theList;
  
  theList = toTest.ReflectionListVariables(); 
  
  EXPECT_EQ(2, theList.size());
  EXPECT_EQ(string("FirstProperty"), theList[0]);
  EXPECT_EQ(string("SecondProperty"), theList[1]);
}

TEST_F(TestIReflected, TestGet) 
{
    ReflectedTester toTest;
    
    // init by side effect.
    toTest.ReflectionListVariables();
    
    EXPECT_EQ(toTest.ReflectionGet(0), 1.0f);
    EXPECT_EQ(toTest.ReflectionGet(1), 2.0f);    
}

TEST_F(TestIReflected, TestSetThenGet) 
{
    ReflectedTester toTest;
    
    // init by side effect.
    toTest.ReflectionListVariables();
    
    toTest.ReflectionSet(0, 5);
    toTest.ReflectionSet(1, 10);
    
    EXPECT_EQ(toTest.ReflectionGet(0), 5.0f);
    EXPECT_EQ(toTest.ReflectionGet(1), 10.0f); 
}

TEST_F(TestIReflected, TestSetMethodGet) 
{
    ReflectedTester toTest;
    
    // init by side effect.
    toTest.ReflectionListVariables();
    
    toTest.ReflectionSet(0, 5);
    toTest.ReflectionSet(1, 10);
    toTest.ReflectionRun(0);
    
    EXPECT_EQ(toTest.ReflectionGet(0), 0.0f);
    EXPECT_EQ(toTest.ReflectionGet(1), 0.0f); 
}

TEST_F(TestIReflected, TestOutOfBounds) 
{
    ReflectedTester toTest;
    
    // init by side effect.
    toTest.ReflectionListVariables();
    
    toTest.ReflectionSet(2, 5);
    toTest.ReflectionSet(55, 10);
    toTest.ReflectionRun(4);
    
    EXPECT_EQ(toTest.ReflectionGet(0), 1.0f);
    EXPECT_EQ(toTest.ReflectionGet(1), 2.0f); 
}

TEST_F(TestIReflected, TestNoInit) 
{
    ReflectedTester toTest;
    
    // by not calling get arguments or get methods
    // nothing is initilised, so all calls are out of bounds.
    
    toTest.ReflectionSet(2, 5);
    toTest.ReflectionSet(55, 10);
    toTest.ReflectionRun(4);
    
    EXPECT_EQ(toTest.ReflectionGet(0), 0.0f);
    EXPECT_EQ(toTest.ReflectionGet(1), 0.0f); 
}