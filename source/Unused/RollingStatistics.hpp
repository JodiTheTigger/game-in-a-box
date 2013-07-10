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

#ifndef ROLLINGSTATISTICS_H
#define ROLLINGSTATISTICS_H

#ifndef USING_PRECOMPILED_HEADERS
#include <cstdint>
#include <memory>
#include <deque>
#endif

namespace GameInABox { namespace Common {

// TODO: Make this a templated class, and make a std::array
class RollingStatistics
{
public:
    RollingStatistics() : RollingStatistics(256) {}
    explicit RollingStatistics(uint32_t rollingBufferSizeInSamples);

    // No calculations are done when adding samples.
    void AddSample(float value);
    uint32_t SampleCount() const { return mySamples.size(); }
    uint32_t SampleCountMax() const { return mySampleSizeMaximum; }
    
    // Values are only valid since the last calculation
    void Calculate();
    
    float GetSum() const { return mySum; }
    float GetAverage() const { return myAverage; }
    float GetMedian() const { return myMedian; }
    float GetQuartile25() const { return myQuartile25; }
    float GetQuartile75() const { return myQuartile75; }
    float GetMin() const { return myMin; }
    float GetMax() const { return myMax; }
    float GetVariance() const { return myVariance; }
    float GetStandardDeviation() const { return myStandardDeviation; }
    
private:
    std::deque<float> mySamples;
    
    std::size_t mySampleSizeMaximum;
    float mySum;
    float myAverage;
    float myMedian;
    float myQuartile25;
    float myQuartile75;
    float myMin;
    float myMax;
    float myVariance;
    float myStandardDeviation;
};

}} // namespace

#endif // ROLLINGSTATISTICS_H
