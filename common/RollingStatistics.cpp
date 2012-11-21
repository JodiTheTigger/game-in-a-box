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

#include "RollingStatistics.h"

using namespace std;

RollingStatistics::RollingStatistics(uint32_t rollingBufferSizeInSamples) 
    : mySamples(new deque<float>()>)
    , mySampleSizeMaximum(rollingBufferSizeInSamples)
    , mySum(0)
    , myAverage(0)
    , myMedian(0)
    , myQuartile25(0)
    , myQuartile75(0)
    , myMin(0)
    , myMax(0)
    , myVariance(0)
    , myStandardDeviation(0) 
{
}

void RollingStatistics::AddSample(float value)
{
    if (mySamples->size() == mySampleSizeMaximum)
    {
        mySamples->pop_back();
    }
    
    mySamples->push_front(value);
}

void RollingStatistics::Calculate()
{
    // TODO!
}

