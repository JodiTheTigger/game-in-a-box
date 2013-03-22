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

#include <gtest/gtest.h>
#include <Common/RollingStatistics.h>
#include <cmath>

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

TEST_F(TestRollingStatistics, EntryOne) 
{
    RollingStatistics stats;    
    
    stats.AddSample(12); 
    stats.Calculate();
    
    EXPECT_EQ(1, stats.SampleCount());
    EXPECT_EQ(256, stats.SampleCountMax());
    EXPECT_EQ(12, stats.GetSum());
    EXPECT_EQ(12, stats.GetAverage());
    EXPECT_EQ(12, stats.GetMedian());
    EXPECT_EQ(12, stats.GetQuartile25());
    EXPECT_EQ(12, stats.GetQuartile75());
    EXPECT_EQ(12, stats.GetMin());
    EXPECT_EQ(12, stats.GetMax());
    EXPECT_EQ(0, stats.GetVariance());
    EXPECT_EQ(0, stats.GetStandardDeviation());
}

TEST_F(TestRollingStatistics, EntryTwo)
{    
    RollingStatistics stats;    
    
    stats.AddSample(12); 
    stats.AddSample(24); 
    stats.Calculate();
    
    EXPECT_EQ(2, stats.SampleCount());
    EXPECT_EQ(256, stats.SampleCountMax());
    EXPECT_EQ(36, stats.GetSum());
    EXPECT_EQ(18, stats.GetAverage());
    EXPECT_EQ(18, stats.GetMedian());
    EXPECT_EQ(12, stats.GetQuartile25());
    EXPECT_EQ(24, stats.GetQuartile75());
    EXPECT_EQ(12, stats.GetMin());
    EXPECT_EQ(24, stats.GetMax());
    EXPECT_EQ(36, stats.GetVariance());
    EXPECT_EQ(6, stats.GetStandardDeviation());
}

TEST_F(TestRollingStatistics, EntryThree)
{
    RollingStatistics stats;    
    
    stats.AddSample(12); 
    stats.AddSample(24); 
    stats.AddSample(18); 
    stats.Calculate();
    
    EXPECT_EQ(3, stats.SampleCount());
    EXPECT_EQ(256, stats.SampleCountMax());
    EXPECT_EQ(54, stats.GetSum());
    EXPECT_EQ(18, stats.GetAverage());
    EXPECT_EQ(18, stats.GetMedian());
    EXPECT_EQ(18, stats.GetQuartile25());
    EXPECT_EQ(24, stats.GetQuartile75());
    EXPECT_EQ(12, stats.GetMin());
    EXPECT_EQ(24, stats.GetMax());
    EXPECT_EQ((72.0f/3.0f), stats.GetVariance());
    EXPECT_EQ(sqrt(72.0f/3.0f), stats.GetStandardDeviation());
}

TEST_F(TestRollingStatistics, Entry100)
{ 
    RollingStatistics stats;    

    for (uint8_t i = 0; i < 100; i++)
    {
        stats.AddSample(i);
    }

    stats.Calculate();

    // calculated from excel
    EXPECT_EQ(100, stats.SampleCount());
    EXPECT_EQ(256, stats.SampleCountMax());
    EXPECT_EQ(4950, stats.GetSum());
    EXPECT_EQ(49.5, stats.GetAverage());
    EXPECT_EQ(49.5, stats.GetMedian());
    EXPECT_EQ(25, stats.GetQuartile25());
    EXPECT_EQ(74, stats.GetQuartile75());
    EXPECT_EQ(0, stats.GetMin());
    EXPECT_EQ(99, stats.GetMax());
    EXPECT_EQ(833.25, stats.GetVariance());
    EXPECT_EQ(28.86607005f, stats.GetStandardDeviation());
}

TEST_F(TestRollingStatistics, EntryPastMax)
{
    RollingStatistics stats(4);  

    stats.AddSample(2);
    stats.AddSample(2);
    stats.AddSample(2);
    stats.AddSample(2);
    stats.AddSample(4);
    stats.AddSample(4);

    stats.Calculate();

    EXPECT_EQ(4, stats.SampleCount());
    EXPECT_EQ(4, stats.SampleCountMax());
    EXPECT_EQ(12, stats.GetSum());
    EXPECT_EQ(3, stats.GetAverage());
    EXPECT_EQ(3, stats.GetMedian());
    EXPECT_EQ(2, stats.GetQuartile25());
    EXPECT_EQ(4, stats.GetQuartile75());
    EXPECT_EQ(2, stats.GetMin());
    EXPECT_EQ(4, stats.GetMax());
    EXPECT_EQ(1, stats.GetVariance());
    EXPECT_EQ(1, stats.GetStandardDeviation());
}
