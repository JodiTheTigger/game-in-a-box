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


#ifndef GAMEINABOXSERVER_H
#define GAMEINABOXSERVER_H

#include <cstdint>
#include <mutex>
#include <condition_variable>

#include "common/BuildMacros.h"
#include "common/IReflected.h"

class GameInABoxServer : public IReflected
{
    CLASS_NOCOPY_NOASSIGN(GameInABoxServer)    
    REFLECTION_BOILERPLATE(GameInABoxServer)
    
public:
    GameInABoxServer(int32_t argc, uint8_t** argv);
    
    // Starts the game, runs the main game loop, keeps going until a fatal error
    // or Stop() has been called.
    void Run();
    
    // Tells the main loop to stop. call be called from any thread at any time.
    // If called before Run has been called, it will cause Run to exit immediately
    // when started.
    void Stop();
    
    // Reflection Properties
    float ServerTickMainLoopPeriodInMsGet() const;
    void ServerTickMainLoopPeriodInMsSet(float newValue);
    
    float ServerTickSendNetworkPeriodInMsGet() const;
    void ServerTickSendNetworkPeriodInMsSet(float newValue);
private:
    volatile bool myQuitSemephore; 
    uint16_t myPeriodMainLoopInMs;
    uint16_t myPeriodNetworkSendInMs;
    
    void InitReflection() override;    
};

#endif // GAMEINABOXSERVER_H
