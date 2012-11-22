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

TEST_F(TestRollingStatistics, EntryOne) 
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

TEST_F(TestRollingStatistics, EntryTwo)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestRollingStatistics, EntryThree)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestRollingStatistics, Entry100)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestRollingStatistics, EntryPastMax)
{
    // TODO!
  EXPECT_EQ(0, 1);
}

TEST_F(TestRollingStatistics, MaxUint32)
{
    // TODO!
  EXPECT_EQ(0, 1);
}