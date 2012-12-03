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

#include <cstdint>
#include "GameInABoxServer.h"

// For the Scratch code
#include <iostream>
#include <chrono>
#include "common/BitStream.h"
#include <thread>

#include "common/BuildMacros.h"

#include "common/IReflected.h"
#include "common/ReflectedAgain.h"

class ScratchClass : public IReflected
{
    REFLECTION_BOILERPLATE(ScratchClass)
    
public:
    float FirstPropertyGet() const {return myFirst;}
    void FirstPropertySet(float toSet) {myFirst = toSet;}
    float SecondPropertyGet() const {return mySecond;}
    void SecondPropertySet(float toSet) {mySecond = toSet;}
    void anyOldMethod() {/*nada*/}    
    
private:
    float myFirst = 0;
    float mySecond = 0;        
    
    void InitReflection() override
    {
        REFLECTION_VARIABLE(ScratchClass, FirstProperty);
        REFLECTION_VARIABLE(ScratchClass, SecondProperty);
        REFLECTION_METHOD(ScratchClass, anyOldMethod);  
    }   
};

class ScratchClass2 : public ReflectedAgain
{    
public:
    float FirstPropertyGet() const {return myFirst;} 
    void FirstPropertySet(float toSet) {myFirst = toSet;}
    float SecondPropertyGet() const {return mySecond;} 
    void SecondPropertySet(float toSet) {mySecond = toSet;}
    void ResetToZero() {myFirst = 0; mySecond=0;}
    std::string BahGet() const {return argh;}
    void BahSet(std::string &newy){argh = newy;}
    
private:
    float myFirst = 1;
    float mySecond = 2;   
    std::string argh = "argh";
    
    void InitReflection() override
    {
          
    }   

private:
    bool myPrivateReflectionHasDoneInit = false; 
    
    typedef void (ScratchClass2::*ReflectionFloatSetter)(float);
    typedef float (ScratchClass2::*ReflectionFloatGetter)(void) const;
    typedef void (ScratchClass2::*ReflectionStringSetter)(std::string);
    typedef std::string (ScratchClass2::*ReflectionStringGetter)(void) const;
    typedef void (ScratchClass2::*ReflectionRunner)(void);
    
    std::vector<ReflectionFloatSetter> reflectionFloatSetters;
    std::vector<ReflectionFloatGetter> reflectionFloatGetters; 
    std::vector<ReflectionStringSetter> reflectionStringSetters;
    std::vector<ReflectionStringGetter> reflectionStringGetters; 
    std::vector<ReflectionRunner> reflectionRunners; 

    std::vector<std::string> reflectionNamesFloatVariable;
    std::vector<std::string> reflectionNamesStringVariable;
    std::vector<std::string> reflectionNamesMethods;       
    
    const std::string PrivateReflectionClassName() const override
    {
        return std::string("ScratchClass2");
    }
    
    const std::vector<std::string>& PrivateReflectionListFloatVariables() const override
    {
        return reflectionNamesFloatVariable;
    }
    
    const std::vector<std::string>& PrivateReflectionListStringVariables() const override
    {
        return reflectionNamesStringVariable;
    }
    
    const std::vector<std::string>& PrivateReflectionListMethods() const override
    {
        return reflectionNamesMethods;
    }
    
    bool PrivateReflectionSet(const uint8_t index, const float newValue) override 
    {
        if (index < reflectionFloatSetters.size())
        {
            (*this.*reflectionFloatSetters[index])(newValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool PrivateReflectionSet(const uint8_t index, const std::string newValue) override 
    {
        if (index < reflectionStringSetters.size())
        {
            (*this.*reflectionStringSetters[index])(newValue);
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool PrivateReflectionGet(const uint8_t index, float& updateValue) const override
    {
        if (index < reflectionFloatGetters.size())
        {
            updateValue = (*this.*reflectionFloatGetters[index])();
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool PrivateReflectionGet(const uint8_t index, std::string& updateValue) const override
    {
        if (index < reflectionStringGetters.size())
        {
            updateValue = (*this.*reflectionStringGetters[index])();
            return true;
        }
        else
        {
            return false;
        }
    }
    
    bool PrivateReflectionRun(uint8_t index) override
    {
        if (index < reflectionRunners.size())
        {
            (*this.*reflectionRunners[index])();
            return true;    
        }
        else
        {
            return false;
        }
    }
};

void Scratch();

int main(int argc, char** argv)
{
    // call testing scratch code
    Scratch();
    
    GameInABoxServer theGame(argc, (uint8_t**)argv);
    
    // runs until theGame.Stop() is called.
    theGame.Run();
    
    return 0;
}

void Scratch() 
{
  // ///////////////////////////
  // The Ugly
  // ///////////////////////////
  
  
  // quasi reflection play
  ScratchClass fred;
  
  //fred.InitReflection();
  fred.ReflectionGet(0);
  fred.ReflectionSet(1, 69);
  std::cout << fred.ReflectionGet(1) << std::endl;
  
  
  
  
  
  
  // This is scratch code at the moment. Testing ideas and the C++11 std libraries.
    std::cout << "Hello, world!" << std::endl;
    
    BitStream moo(10);
    
    // testing high res timers
    {
      using namespace std::chrono;
      
      system_clock::time_point t1 = high_resolution_clock::now();
      system_clock::time_point t2 = high_resolution_clock::now();
      
      std::cout << t1.time_since_epoch().count() << " " << t2.time_since_epoch().count() << std::endl;
      
      //auto duration = high_resolution_clock::now().time_since_epoch();
      auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
      
      while ((std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count()) < 16)
      {
	// wait
	t2 = high_resolution_clock::now();
      }
      
      millis = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
      std::cout << "Time: " << millis << std::endl;
      
      
      typedef std::chrono::high_resolution_clock Clock;
      auto tt1 = Clock::now();
      auto tt2 = Clock::now();
      auto result = tt2 - tt1;
      //std::cout << t2 << " " << t1 << '\n';
    }
    
    std::cout << " lets try thread sleeping " << std::endl;
    
    for (int i = 0; i < 100 ; i++)
    {
      using namespace std::chrono;
      
      system_clock::time_point ttt1 = high_resolution_clock::now();
      
      //boost::this_thread::sleep( boost::posix_time::milliseconds(16) );
      std::this_thread::sleep_for( std::chrono::milliseconds(16) );
      
      system_clock::time_point ttt2 = high_resolution_clock::now();
      
      std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(ttt2-ttt1).count() << std::endl;
      
    }
}
