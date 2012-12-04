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
#include <algorithm>

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
        REFLECTION_VARIABLE_FLOAT(ReflectedTester, FirstProperty);
        REFLECTION_VARIABLE_FLOAT(ReflectedTester, SecondProperty);
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
  
  EXPECT_EQ(3, relfectionMap.size());
  EXPECT_NE(relfectionMap.end(), relfectionMap.find("FirstProperty"));
  EXPECT_NE(relfectionMap.end(), relfectionMap.find("SecondProperty"));
  EXPECT_NE(relfectionMap.end(), relfectionMap.find("ResetToZero"));
}

TEST_F(TestIReflected, TestGet) 
{
    ReflectedTester toTest;
    float result1;
    float result2;
    
    auto relfectionMap = toTest.ReflectionList();
    
    toTest.ReflectionGet(relfectionMap["FirstProperty"], result1);
    toTest.ReflectionGet(relfectionMap["SecondProperty"], result2);
    
    EXPECT_EQ(result1, 1.0f);
    EXPECT_EQ(result2, 2.0f);    
}

TEST_F(TestIReflected, TestSetThenGet) 
{
    ReflectedTester toTest;  
    float result1;
    float result2;  
    
    auto relfectionMap = toTest.ReflectionList();
    
    toTest.ReflectionSet(relfectionMap["FirstProperty"], 5);
    toTest.ReflectionSet(relfectionMap["SecondProperty"], 10);
    
    toTest.ReflectionGet(relfectionMap["FirstProperty"], result1);
    toTest.ReflectionGet(relfectionMap["SecondProperty"], result2);
    
    EXPECT_EQ(result1, 5.0f);
    EXPECT_EQ(result2, 10.0f); 
}

TEST_F(TestIReflected, TestSetMethodGet) 
{
    ReflectedTester toTest;  
    float result1;
    float result2;  
    
    auto relfectionMap = toTest.ReflectionList();
    
    toTest.ReflectionSet(relfectionMap["FirstProperty"], 5);
    toTest.ReflectionSet(relfectionMap["SecondProperty"], 10);
    toTest.ReflectionRun(relfectionMap["ResetToZero"]);
    
    toTest.ReflectionGet(relfectionMap["FirstProperty"], result1);
    toTest.ReflectionGet(relfectionMap["SecondProperty"], result2);
    
    EXPECT_EQ(result1, 0.0f);
    EXPECT_EQ(result2, 0.0f); 
}

TEST_F(TestIReflected, TestOutOfBounds) 
{
    ReflectedTester toTest;
    
    auto relfectionMap = toTest.ReflectionList();
    
    // RAM: TODO!
    EXPECT_EQ(0, 1);
}

TEST_F(TestIReflected, TestNoInit) 
{
    ReflectedTester toTest;
    
    // by not calling get arguments or get methods
    // nothing is initilised, so all calls are out of bounds.
    
    // RAM: TODO!
    EXPECT_EQ(0, 1);
}