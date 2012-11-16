#include <iostream>
#include <chrono>
#include "common/BitStream.h"
#include <thread>

int main(int argc, char **argv) 
{
  // ///////////////////////////
  // The Ugly
  // ///////////////////////////
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
    
    return 0;
}
