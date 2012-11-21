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

