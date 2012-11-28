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
  EXPECT_EQ(0, 1);
}

TEST_F(TestReflectionManager, TestAddOneClass) 
{
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