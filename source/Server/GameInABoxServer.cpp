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
#include <vector>

#include "Common/Network/NetworkProvider.h"
#include "Common/Network/INetworkManager.h"
#include "Common/Network/NetworkManagerServer.h"
#include "Common/IStateManager.h"
#include "Common/GameSimple.h"
#include "Common/ReflectionManager.h"

using namespace std;
using namespace std::chrono;
using namespace GameInABox::Common;
using namespace GameInABox::Server;
using namespace GameInABox::Common::Network;

GameInABoxServer::GameInABoxServer(int32_t, uint8_t**)
: REFLECTION_INIT_MEMBERS
, myQuitSemephore(false)
, myPeriodMainLoopInMs(16)
, myPeriodNetworkSendInMs(50)
, myNetworkSource()
, myNetworkState()
, myGame()
, myMirror(new ReflectionManager())
{
    // TODO!
}


GameInABoxServer::~GameInABoxServer()
{
    // Nothing.
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
    
    // startup
    // Can have different game types, for now choose GameSimple.
    myGame = shared_ptr<IStateManager>(new GameSimple());
    myNetworkSource = shared_ptr<NetworkProvider>(new NetworkProvider());
    
    // RAM: TODO!
    // RAM: use make_shared!
    //myNetworkState = shared_ptr<INetworkManager>(
    //            new NetworkManagerServer(vector<unique_ptr<NetworkProvider>>(), myGame));
    
    // register
    
    while (!myQuitSemephore)
    {
        // Check Network
        
        // Update Gamestate
        myNetworkState->ProcessIncomming();
        
        // //////////////////////
        // Send Network
        // //////////////////////
        timeNow = high_resolution_clock::now();
        timeDeltaInMs = duration_cast<milliseconds>(timeNow-timeLoopNetworkStart).count();
        
        if (timeDeltaInMs < myPeriodNetworkSendInMs)
        {
            myNetworkState->SendState();
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

