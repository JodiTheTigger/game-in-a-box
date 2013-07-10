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
#include <Common/AutoComplete.hpp>

#include <algorithm>

using namespace std;

namespace GameInABox { namespace Common {

// Class definition!
class TestAutoComplete : public ::testing::Test 
{
};

TEST_F(TestAutoComplete, EmptyList) 
{
    AutoComplete toTest = AutoComplete(vector<string>());
        
    EXPECT_EQ("", toTest.GetNextBestMatch("Harry"));
    EXPECT_EQ("", toTest.GetNextBestMatch(""));
    EXPECT_EQ(0, toTest.GetMatchList("").size());
}

TEST_F(TestAutoComplete, OneWordInList) 
{
    AutoComplete toTest = AutoComplete(vector<string> {"OneWordInList.Happy()"});
    
    // next best match
    EXPECT_EQ("", toTest.GetNextBestMatch("Harry"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Word"));
    EXPECT_EQ("", toTest.GetNextBestMatch("."));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch(""));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("One"));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("OneWordInList."));
    EXPECT_EQ("", toTest.GetNextBestMatch("OneWordInList.."));
    
    // match list
    EXPECT_EQ(0, toTest.GetMatchList("Harry").size());
    EXPECT_EQ(0, toTest.GetMatchList("Word").size());
    EXPECT_EQ(0, toTest.GetMatchList("OneWordInList..").size());
    ASSERT_EQ(1, toTest.GetMatchList("").size());
    ASSERT_EQ(1, toTest.GetMatchList("One").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.").size());
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("")[0]);
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("One")[0]);
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("OneWordInList.")[0]);
}

TEST_F(TestAutoComplete, TwoWordsInList) 
{    
    vector<string> results;
    AutoComplete toTest = AutoComplete(vector<string> 
    {
        "OneWordInList.Happy()",
        "TwoWrodsInList.Sad()"        
    } );
    
    // next best match
    EXPECT_EQ("", toTest.GetNextBestMatch("Harry"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Word"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Sad"));
    EXPECT_EQ("", toTest.GetNextBestMatch("."));
    EXPECT_EQ("", toTest.GetNextBestMatch(""));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("One"));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("OneWordInList."));
    EXPECT_EQ("TwoWrodsInList.Sad()", toTest.GetNextBestMatch("Two"));
    EXPECT_EQ("TwoWrodsInList.Sad()", toTest.GetNextBestMatch("TwoWrodsInList."));
    
    // match list
    EXPECT_EQ(0, toTest.GetMatchList("Harry").size());
    EXPECT_EQ(0, toTest.GetMatchList("Word").size());
    EXPECT_EQ(0, toTest.GetMatchList("Sad").size());
    EXPECT_EQ(2, toTest.GetMatchList("").size());
    
    results = toTest.GetMatchList("");
    
    EXPECT_NE(end(results), find(begin(results), end(results), "OneWordInList.Happy()"));
    EXPECT_NE(end(results), find(begin(results), end(results), "TwoWrodsInList.Sad()"));
    
    ASSERT_EQ(1, toTest.GetMatchList("One").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.").size());
    ASSERT_EQ(1, toTest.GetMatchList("TwoW").size());
    ASSERT_EQ(1, toTest.GetMatchList("TwoWrodsInList.S").size());
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("One")[0]);
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("OneWordInList.")[0]);
    EXPECT_EQ("TwoWrodsInList.Sad()", toTest.GetMatchList("TwoW")[0]);
    EXPECT_EQ("TwoWrodsInList.Sad()", toTest.GetMatchList("TwoWrodsInList.S")[0]);
}

TEST_F(TestAutoComplete, TwoWordsInListSimilar) 
{
    vector<string> results;
    AutoComplete toTest = AutoComplete(vector<string> 
    {
        "OneWordInList.Happy()",
        "OneWordInList.RealHappy"        
    } );
    
    // next best match
    EXPECT_EQ("", toTest.GetNextBestMatch("Harry"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Word"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Sad"));
    EXPECT_EQ("", toTest.GetNextBestMatch("."));
    EXPECT_EQ("OneWordInList.", toTest.GetNextBestMatch(""));
    EXPECT_EQ("OneWordInList.", toTest.GetNextBestMatch("One"));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("OneWordInList.H"));
    EXPECT_EQ("OneWordInList.RealHappy", toTest.GetNextBestMatch("OneWordInList.R"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Two"));
    EXPECT_EQ("", toTest.GetNextBestMatch("TwoWrodsInList."));
    
    // match list
    EXPECT_EQ(0, toTest.GetMatchList("Harry").size());
    EXPECT_EQ(0, toTest.GetMatchList("Word").size());
    EXPECT_EQ(0, toTest.GetMatchList("Sad").size());
    EXPECT_EQ(2, toTest.GetMatchList("").size());
    
    results = toTest.GetMatchList("");
    
    EXPECT_NE(end(results), find(begin(results), end(results), "OneWordInList.Happy()"));
    EXPECT_NE(end(results), find(begin(results), end(results), "OneWordInList.RealHappy"));
    
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.H").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.Hap").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.Re").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.Real").size());
    
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("OneWordInList.H")[0]);
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("OneWordInList.Hap")[0]);
    EXPECT_EQ("OneWordInList.RealHappy", toTest.GetMatchList("OneWordInList.Re")[0]);
    EXPECT_EQ("OneWordInList.RealHappy", toTest.GetMatchList("OneWordInList.Real")[0]);
}

TEST_F(TestAutoComplete, Duplicates)
{
    vector<string> results;
    AutoComplete toTest = AutoComplete(vector<string> 
    {
        "OneWordInList.Happy()",
        "OneWordInList.Happy()"        
    } );
    
    // next best match
    EXPECT_EQ("", toTest.GetNextBestMatch("Harry"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Word"));
    EXPECT_EQ("", toTest.GetNextBestMatch("."));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch(""));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("One"));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("OneWordInList."));
    EXPECT_EQ("", toTest.GetNextBestMatch("OneWordInList.."));
    
    // match list
    EXPECT_EQ(0, toTest.GetMatchList("Harry").size());
    EXPECT_EQ(0, toTest.GetMatchList("Word").size());
    EXPECT_EQ(0, toTest.GetMatchList("OneWordInList..").size());
    ASSERT_EQ(1, toTest.GetMatchList("").size());
    ASSERT_EQ(1, toTest.GetMatchList("One").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.").size());
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("")[0]);
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("One")[0]);
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("OneWordInList.")[0]);
}

TEST_F(TestAutoComplete, SimpleAutocompleteTests) 
{
    vector<string> results;
    AutoComplete toTest = AutoComplete(vector<string> 
    {
        "OneWordInList.Happy()",
        "OneWordInList.RealHappy",
        "OneWordInList.RealHappy",
        "TwoThings",
        "Tree.One()",
        "Tree.Two",
        "Tree.Tree.Trie"        
    } );
    
    // next best match
    EXPECT_EQ("", toTest.GetNextBestMatch("Harry"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Word"));
    EXPECT_EQ("", toTest.GetNextBestMatch("Sad"));
    EXPECT_EQ("", toTest.GetNextBestMatch("."));
    EXPECT_EQ("", toTest.GetNextBestMatch(""));
    EXPECT_EQ("OneWordInList.", toTest.GetNextBestMatch("One"));
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetNextBestMatch("OneWordInList.H"));
    EXPECT_EQ("OneWordInList.RealHappy", toTest.GetNextBestMatch("OneWordInList.R"));
    EXPECT_EQ("TwoThings", toTest.GetNextBestMatch("Two"));
    EXPECT_EQ("Tree.", toTest.GetNextBestMatch("Tree"));
    EXPECT_EQ("Tree.T", toTest.GetNextBestMatch("Tree.T"));
    EXPECT_EQ("Tree.Tree.Trie", toTest.GetNextBestMatch("Tree.Tr"));
    
    // match list
    EXPECT_EQ(0, toTest.GetMatchList("Harry").size());
    EXPECT_EQ(0, toTest.GetMatchList("Word").size());
    EXPECT_EQ(0, toTest.GetMatchList("Sad").size());
    EXPECT_EQ(6, toTest.GetMatchList("").size());
    EXPECT_EQ(3, toTest.GetMatchList("Tr").size());
    EXPECT_EQ(2, toTest.GetMatchList("Tree.T").size());
    EXPECT_EQ(2, toTest.GetMatchList("One").size());
    
    results = toTest.GetMatchList("");
    
    EXPECT_NE(end(results), find(begin(results), end(results), "OneWordInList.Happy()"));
    EXPECT_NE(end(results), find(begin(results), end(results), "OneWordInList.RealHappy"));
    EXPECT_NE(end(results), find(begin(results), end(results), "TwoThings"));
    EXPECT_NE(end(results), find(begin(results), end(results), "Tree.One()"));
    EXPECT_NE(end(results), find(begin(results), end(results), "Tree.Two"));
    EXPECT_NE(end(results), find(begin(results), end(results), "Tree.Tree.Trie"));
    
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.H").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.Hap").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.Re").size());
    ASSERT_EQ(1, toTest.GetMatchList("OneWordInList.Real").size());
    
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("OneWordInList.H")[0]);
    EXPECT_EQ("OneWordInList.Happy()", toTest.GetMatchList("OneWordInList.Hap")[0]);
    EXPECT_EQ("OneWordInList.RealHappy", toTest.GetMatchList("OneWordInList.Re")[0]);
    EXPECT_EQ("OneWordInList.RealHappy", toTest.GetMatchList("OneWordInList.Real")[0]);
}

}} // namespace
