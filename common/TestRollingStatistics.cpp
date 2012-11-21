#include "RollingStatistics.h"
#include "gtest/gtest.h"

using namespace std;

// Class definition!
class TestRollingStatistics : public ::testing::Test 
{
};

TEST_F(TestRollingStatistics, ZeroSize) 
{
    RollingStatistics zero(0);
    
    EXPECT_EQ(0, zero.SampleCount());
    EXPECT_EQ(0, zero.SampleCountMax());
    EXPECT_EQ(0, zero.GetSum());
    EXPECT_EQ(0, zero.GetAverage());
    EXPECT_EQ(0, zero.GetMedian());
    EXPECT_EQ(0, zero.GetQuartile25());
    EXPECT_EQ(0, zero.GetQuartile75());
    EXPECT_EQ(0, zero.GetMin());
    EXPECT_EQ(0, zero.GetMax());
    EXPECT_EQ(0, zero.GetVariance());
    EXPECT_EQ(0, zero.GetStandardDeviation());
    
    zero.Calculate();
    
    EXPECT_EQ(0, zero.SampleCount());
    EXPECT_EQ(0, zero.SampleCountMax());
    EXPECT_EQ(0, zero.GetSum());
    EXPECT_EQ(0, zero.GetAverage());
    EXPECT_EQ(0, zero.GetMedian());
    EXPECT_EQ(0, zero.GetQuartile25());
    EXPECT_EQ(0, zero.GetQuartile75());
    EXPECT_EQ(0, zero.GetMin());
    EXPECT_EQ(0, zero.GetMax());
    EXPECT_EQ(0, zero.GetVariance());
    EXPECT_EQ(0, zero.GetStandardDeviation());
    
    zero.AddSample(12);
    zero.Calculate();
    
    EXPECT_EQ(0, zero.SampleCount());
    EXPECT_EQ(0, zero.SampleCountMax());
    EXPECT_EQ(0, zero.GetSum());
    EXPECT_EQ(0, zero.GetAverage());
    EXPECT_EQ(0, zero.GetMedian());
    EXPECT_EQ(0, zero.GetQuartile25());
    EXPECT_EQ(0, zero.GetQuartile75());
    EXPECT_EQ(0, zero.GetMin());
    EXPECT_EQ(0, zero.GetMax());
    EXPECT_EQ(0, zero.GetVariance());
    EXPECT_EQ(0, zero.GetStandardDeviation());    
    
    zero.AddSample(12);    
    zero.AddSample(434);
    zero.AddSample(234252);
    zero.Calculate();
    
    EXPECT_EQ(0, zero.SampleCount());
    EXPECT_EQ(0, zero.SampleCountMax());
    EXPECT_EQ(0, zero.GetSum());
    EXPECT_EQ(0, zero.GetAverage());
    EXPECT_EQ(0, zero.GetMedian());
    EXPECT_EQ(0, zero.GetQuartile25());
    EXPECT_EQ(0, zero.GetQuartile75());
    EXPECT_EQ(0, zero.GetMin());
    EXPECT_EQ(0, zero.GetMax());
    EXPECT_EQ(0, zero.GetVariance());
    EXPECT_EQ(0, zero.GetStandardDeviation());    
}

TEST_F(TestBitStream, EntryOne) 
{
    RollingStatistics zero();    
    
    zero.AddSample(12); 
    zero.Calculate();
    
    EXPECT_EQ(1, zero.SampleCount());
    EXPECT_EQ(256, zero.SampleCountMax());
    EXPECT_EQ(12, zero.GetSum());
    EXPECT_EQ(12, zero.GetAverage());
    EXPECT_EQ(12, zero.GetMedian());
    EXPECT_EQ(12, zero.GetQuartile25());
    EXPECT_EQ(12, zero.GetQuartile75());
    EXPECT_EQ(12, zero.GetMin());
    EXPECT_EQ(12, zero.GetMax());
    EXPECT_EQ(0, zero.GetVariance());
    EXPECT_EQ(0, zero.GetStandardDeviation());
}

TEST_F(TestBitStream, EntryTwo)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestBitStream, EntryThree)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestBitStream, Entry100)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestBitStream, EntryPastMax)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestBitStream, MaxUint32)
{
    // TODO!
  EXPECT_EQ(0, 1);
}