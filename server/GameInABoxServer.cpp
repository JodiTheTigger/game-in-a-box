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

#include "GameInABoxServer.h"

#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;

GameInABoxServer::GameInABoxServer(int32_t, uint8_t**)
: myQuitSemephore(false)
, myPeriodMainLoopInMs(16)
, myPeriodNetworkSendInMs(50)
{
    // TODO!
}

void GameInABoxServer::InitReflection()
{
}

void GameInABoxServer::Stop()
{
    myQuitSemephore = true;
}

void GameInABoxServer::Run()
{
    system_clock::time_point timeLoopMainStart;
    system_clock::time_point timeLoopNetworkStart;
    system_clock::time_point timeNow;
    uint32_t timeDeltaInMs;
    
    while (!myQuitSemephore)
    {
        // Check Network
        
        // Update Gamestate
        
        // //////////////////////
        // Send Network
        // //////////////////////
        timeNow = high_resolution_clock::now();
        timeDeltaInMs = duration_cast<milliseconds>(timeNow-timeLoopNetworkStart).count();
        
        if (timeDeltaInMs < myPeriodNetworkSendInMs)
        {
            // Send network stuff!
        }
        
        // //////////////////////
        // Sleep
        // //////////////////////
        // only sleep if we have the time to sleep
        timeNow = high_resolution_clock::now();
        timeDeltaInMs = duration_cast<milliseconds>(timeNow-timeLoopMainStart).count();
        
        if (timeDeltaInMs < myPeriodMainLoopInMs)
        {
            this_thread::sleep_for(std::chrono::milliseconds(myPeriodMainLoopInMs - timeDeltaInMs));
        }
    }
}

float GameInABoxServer::ServerTickMainLoopPeriodInMsGet() const
{
    return myPeriodMainLoopInMs;
}
void GameInABoxServer::ServerTickMainLoopPeriodInMsSet(float newValue)
{
    myPeriodMainLoopInMs = newValue;
}

float GameInABoxServer::ServerTickSendNetworkPeriodInMsGet() const
{
    return myPeriodMainLoopInMs;
}

void GameInABoxServer::ServerTickSendNetworkPeriodInMsSet(float newValue)
{
    myPeriodMainLoopInMs = newValue;
}

