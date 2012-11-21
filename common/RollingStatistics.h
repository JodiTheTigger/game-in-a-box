#ifndef ROLLINGSTATISTICS_H
#define ROLLINGSTATISTICS_H

#include <cstdint>
#include <memory>
#include <deque>

class RollingStatistics
{
public:
    RollingStatistics() : RollingStatistics(256) {}
    RollingStatistics(uint32_t rollingBufferSizeInSamples);

    // No calculations are done when adding samples.
    void AddSample(float value);
    uint32_t SampleCount() const { return mySamples->size(); }
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
    // Don't allow copying, I want the compiler to tell me if I'm accidentally doing a copy.
    RollingStatistics(const RollingStatistics&);
    RollingStatistics& operator=(const RollingStatistics&);

    std::unique_ptr<std::deque<float>> mySamples;
    
    float mySampleSizeMaximum;
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

#endif // ROLLINGSTATISTICS_H
