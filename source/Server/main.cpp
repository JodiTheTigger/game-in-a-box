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
#include "GameInABoxServer.hpp"

// Linux
#include <signal.h>

// For the Scratch code
#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include "Common/BitStream.hpp"

#include "Common/IReflected.hpp"
#include "Common/AutoComplete.hpp"

using namespace GameInABox::Server;
using namespace GameInABox::Common;

// Globals for signal use.
GameInABoxServer* theGame = nullptr;

// Linux signal handlers
// TODO: Use boost asio to handle these.
extern "C"
{
    void SignalSigHup(int signal)
    {
        if (signal == SIGHUP)
        {
            if (theGame != nullptr)
            {
                // RAM: TODO! formalise this debug please
                std::cout << std::endl << "Received SIGHUP" << std::endl;
                std::cout << std::flush;
                theGame->Stop();
            }
        }
    }
    
    void SignalSigInt(int signal)
    {
        if (signal == SIGINT)
        {
            if (theGame != nullptr)
            {
                // RAM: TODO! formalise this debug please
                std::cout << std::endl << "Received SIGINT" << std::endl;
                std::cout << std::flush;
                theGame->Stop();
            }
        }
    }
}

void Scratch();

int main(int argc, char** argv)
{
    // call testing scratch code
    Scratch();
    
    // make the game
    theGame = new GameInABoxServer(argc, (uint8_t**)argv);

    // Register linux signal handlers
    signal(SIGHUP, SignalSigHup);
    signal(SIGINT, SignalSigInt);
    
    // runs until theGame.Stop() is called.
    theGame->Run();
    
    delete theGame;
    
    return 0;
}

void Scratch() 
{
  // ///////////////////////////
  // The Ugly
  // ///////////////////////////
  
      AutoComplete toTest = AutoComplete(std::vector<std::string> 
    {
        "OneWordInList.Happy()",
        "OneWordInList.RealHappy",
        "OneWordInList.RealHappy",
        "TwoThings",
        "Tree.One()",
        "Tree.Two",
        "Tree.Tree.Trie"        
    } );
      
      std::cout << "Tr -> " << toTest.GetNextBestMatch("Tr") << std::endl;
        
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
      
      
      //typedef std::chrono::high_resolution_clock Clock;
      //auto tt1 = Clock::now();
      //auto tt2 = Clock::now();
      //auto result = tt2 - tt1;
      //std::cout << t2 << " " << t1 << '\n';
    }
    
    std::cout << " lets try thread sleeping " << std::endl;
    
    for (int i = 0; i < 2 ; i++)
    {
      using namespace std::chrono;
      
      system_clock::time_point ttt1 = high_resolution_clock::now();
      
      //boost::this_thread::sleep( boost::posix_time::milliseconds(16) );
      std::this_thread::sleep_for( std::chrono::milliseconds(16) );
      
      system_clock::time_point ttt2 = high_resolution_clock::now();
      
      std::cout << "Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(ttt2-ttt1).count() << std::endl;
      
    }
}
