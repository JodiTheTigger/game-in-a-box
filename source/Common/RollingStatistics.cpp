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

#ifndef USING_PRECOMPILED_HEADERS
#include <vector>
#include <cmath>
#include <algorithm>
#endif

#include "RollingStatistics.hpp"

// RAM: TODO! ARGH! MSVC WART! Need to have a pre-compiled header to put all the warts into
// but for now, this will have to do.
#ifdef _MSC_VER
#define roundf(dbl) dbl >= 0.0 ? (int)(dbl + 0.5) : ((dbl - (double)(int)dbl) <= -0.5 ? (int)dbl : (int)(dbl - 0.5))
#endif

using namespace std;
using namespace GameInABox::Common;

RollingStatistics::RollingStatistics(uint32_t rollingBufferSizeInSamples)
    : mySamples(new deque<float>())
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
	if (mySampleSizeMaximum != 0)
	{
		if (mySamples->size() == mySampleSizeMaximum)
		{
			mySamples->pop_back();
		}    

		mySamples->push_front(value);
	}
}

void RollingStatistics::Calculate()
{
    vector<float> sorted;
    double bigSum;
    double bigVariance;
    
    // If you can optimise this for speed (with timings proof), please do!
    
    if (1 > mySamples->size())
    {
        return;
    }
    
    // Three copies:
    // 1. Copy deque to a vector, sum, average.
    // 2. Std Deviation
    // 3. Sort.
    
    // 1.
    sorted.reserve(mySamples->size());
    bigSum = 0;
    for (float toCopy : *mySamples)
    {
        bigSum += toCopy;
        sorted.push_back(toCopy);
    }
    
    myAverage = static_cast<float>(bigSum / sorted.size());
    mySum = static_cast<float>(bigSum);
    
    // 2.
    bigVariance = 0;
    for (float toVariance : sorted)
    {
        bigVariance += (toVariance - myAverage) * (toVariance - myAverage);
    }
    myVariance = static_cast<float>(bigVariance / mySamples->size());
    myStandardDeviation = static_cast<float>(std::sqrt(bigVariance / mySamples->size()));
    
    // 3.
    sort(sorted.begin(), sorted.end());
    myMin = sorted.front();
    myMax = sorted.back();
    
    myQuartile25 = sorted[static_cast<uint32_t>(roundf((sorted.size() - 1) * 0.25))];
    myQuartile75 = sorted[static_cast<uint32_t>(roundf((sorted.size() - 1) * 0.75))];
    
    if (sorted.size() == 1)
    {
        myMedian = sorted[0];
    }
    else
    {
        if ((sorted.size() % 2) == 0)
        {
            myMedian = sorted[sorted.size() / 2] * 0.5f;
            myMedian += sorted[(sorted.size() / 2) - 1] * 0.5f;
        }
        else
        {
            myMedian = sorted[sorted.size() / 2];
        }    
    }
}
