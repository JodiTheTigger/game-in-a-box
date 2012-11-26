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

class ScratchClass;

typedef void (ScratchClass::*Setter)(float);
typedef float (ScratchClass::*Getter)(void) const;
typedef void (ScratchClass::*Runner)(void);

class IReflected
{
    //CLASS_VIRTUAL_COPYASSIGN(IReflected);
 
public:   
    virtual ~IReflected() {};
    
    virtual void InitReflection() = 0;
    virtual std::string ReflectionClassName() const = 0;
    virtual std::vector<std::string> ReflectionListVariables() const = 0;
    virtual std::vector<std::string> ReflectionListMethods() const = 0;
    
    virtual void ReflectionSet(uint8_t index, float newValue) = 0;
    virtual float ReflectionGet(uint8_t index) const = 0;
    virtual void ReflectionRun(uint8_t index) = 0;
};


class ScratchClass : public IReflected
{
public:
    float getAAA() const {return a;}
    void setAAA(float toSet) {a = toSet;}
    float getBBB() const {return b;}
    void setBBB(float toSet) {b = toSet;}
    void anyOldMethod() {/*nada*/}
    
    ScratchClass()
    {
        InitReflection();
    }
    
    ~ScratchClass()
    {
    }
    
    void InitReflection()
    {
        reflectionSetters.push_back(&ScratchClass::setAAA);
        reflectionGetters.push_back(&ScratchClass::getAAA);
        reflectionNamesVariable.push_back("AAA");
        
        reflectionSetters.push_back(&ScratchClass::setBBB);        
        reflectionGetters.push_back(&ScratchClass::getBBB);         
        reflectionNamesVariable.push_back("BBB");
        
        reflectionRunners.push_back(&ScratchClass::anyOldMethod);          
        reflectionNamesMethods.push_back("anyOldMethod");          
    }   
    
    std::string ReflectionClassName() const
    {
        return std::string("ScratchClass");
    }
    
    float ReflectionGet(uint8_t index) const
    {
        if (index < reflectionGetters.size())
        {
            return (*this.*reflectionGetters[index])();
        }
        else
        {
            // don't bother with exceptions. Just return zero.
            return 0.0;
        }
    }
    
    void ReflectionSet(uint8_t index, float newValue)
    {
        if (index < reflectionSetters.size())
        {
            (*this.*reflectionSetters[index])(newValue);
        }
    }
    
    void ReflectionRun(uint8_t index)
    {
        if (index < reflectionRunners.size())
        {
            (*this.*reflectionRunners[index])();    
        }
    }
    
    std::vector<std::string> ReflectionListVariables() const
    {
        // I assume it does a copy here.
        return reflectionNamesVariable;
    }
    
    std::vector<std::string> ReflectionListMethods() const
    {
        // I assume it does a copy here.
        return reflectionNamesMethods;
    }
    
    
private:
    float a = 0;
    float b = 0;
    
    std::vector<Setter> reflectionSetters;
    std::vector<Getter> reflectionGetters; 
    std::vector<Runner> reflectionRunners; 
    std::vector<std::string> reflectionNamesVariable;
    std::vector<std::string> reflectionNamesMethods;    
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
