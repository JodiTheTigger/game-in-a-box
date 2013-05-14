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
    along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "AutoComplete.hpp"

#ifndef USING_PRECOMPILED_HEADERS
#include <algorithm>
#endif

using namespace std;
using namespace GameInABox::Common;

// AutoComplete Notes:
// ===================
//
// I was going to implement as a Trie (http://login2win.blogspot.co.nz/2011/06/c-tries.html) but
// decided on a radix tree instead (http://en.wikipedia.org/wiki/Radix_tree). I insert at creation
// and don't allow deletes to make my life easier. This class took me longer that I wanted but it
// finally works. I loosely based my working code from someone else's work:
// http://code.google.com/p/radixtree/source/browse/trunk/RadixTree/src/ds/tree/
//
// I created the Map function so the code can be shared by GetMatchList and GetNextBestMatch
// where in the implementation I based my work on wrote similar code twice.

// /////////////////////
// Public Auto Complete
// /////////////////////
AutoComplete::AutoComplete(std::vector<std::string> wordList)
: AutoComplete()
{
    for (string newWord : wordList)
    {
        Insert(newWord);
    }
}

std::vector<std::string> AutoComplete::GetMatchList(std::string toMatch) const
{
    std::vector<std::string> result;
    deque<size_t> theMap;
    
    if (MatchMap(toMatch, theMap))
    {
        result = MapToStringAndTails(theMap);
    }
    
    return result;
}

std::string AutoComplete::GetNextBestMatch(std::string toMatch) const
{
    string mapResult;
    
    deque<size_t> theMap;
        
    if (MatchMap(toMatch, theMap))
    {
        mapResult = MapToString(theMap);
    }
    
    return mapResult;
}

// /////////////////////
// Private Auto Complete
// /////////////////////

AutoComplete::AutoComplete()
: AutoComplete("")
{
}

AutoComplete::AutoComplete(std::string word)
: myString(word)
, myChildren()
{
}

void AutoComplete::Insert(std::string toInsert)
{
    // ignore duplicates
    if (toInsert != myString)
    {
        size_t matchCount;
            
        matchCount = MatchingCharacters(toInsert);
        
        if (matchCount == 0)
        {
            if (IsLeaf())
            {
                // easy! just add as a child
                myChildren.push_back(unique_ptr<AutoComplete>(new AutoComplete(toInsert)));
            }
            else
            {
                size_t bestChild;
                
                bestChild = BestMatchChildIndex(toInsert);
                
                if (bestChild == myChildren.size())
                {
                    // add as another child
                    myChildren.push_back(unique_ptr<AutoComplete>(new AutoComplete(toInsert)));
                }
                else
                {
                    myChildren[bestChild]->Insert(toInsert);
                }
            }
        }
        else
        {
            string tail;
            
            tail = toInsert.substr(matchCount);
            
            // Split, or just add as a child
            if (matchCount == myString.size())
            {                    
                if (IsLeaf())
                {        
                    // add as a child, easy
                    myChildren.push_back(unique_ptr<AutoComplete>(new AutoComplete(tail)));
                }
                else
                {
                    size_t match;
                    
                    // test against the childen, and add to them, or if
                    // no one matches, add as another child.
                    match = BestMatchChildIndex(tail);
                    
                    if (match < myChildren.size())
                    {
                        myChildren[match]->Insert(tail);
                    }
                    else
                    {
                        myChildren.push_back(unique_ptr<AutoComplete>(new AutoComplete(tail)));                        
                    }                       
                }
            }
            else
            {
                // Bah, have to split my string, move nodes, and stuff.
                unique_ptr<AutoComplete> mrSplit;
                string oldTail;        
                
                oldTail = myString.substr(matchCount);                
                mrSplit.reset(new AutoComplete(oldTail)); 
                
                // move the children across
                while (!myChildren.empty())
                {
                    mrSplit->myChildren.push_back(move(myChildren.back()));
                }
                
                // change my string to the smaller common one, and add the new as it's first child.
                myString = myString.substr(0, matchCount);
                myChildren.push_back(unique_ptr<AutoComplete>(new AutoComplete(tail)));    
                
                // Don't forget the old tail
                myChildren.push_back(move(mrSplit));
            }
        }
    }
}

size_t AutoComplete::MatchingCharacters(const std::string& toMatch) const
{
    size_t result;
    size_t max;
    
    result = 0;
    max = min(myString.size(), toMatch.size());
        
    while (result < max)
    {
        if (myString[result] != toMatch[result])
        {
            break;
        }
        
        ++result;
    }
    
    return result;
}

// Returns myChildren.size() is none found.
size_t AutoComplete::BestMatchChildIndex(const std::string& toMatch) const
{
    if (!toMatch.empty())
    {   
        for (size_t index = 0; index < myChildren.size(); index++)
        {
            if (toMatch[0] == myChildren[index]->myString[0])
            {
                return index;
            }
        }
    }
        
    return myChildren.size();
}

bool AutoComplete::MatchMap(const string& toMatch, std::deque< size_t >& treeMap) const
{   
    size_t matchCount;
 
    // special cases
    if (myString.empty() && toMatch.empty())
    {
        if (myChildren.size() == 1)
        {
            treeMap.push_front(0);
        }
        
        return true;
    }
    
    matchCount = MatchingCharacters(toMatch);
    
    if ((matchCount == toMatch.size()) && (matchCount <= myString.size()))
    {
        return true;
    }
    else 
    {
        if (
                myString.empty() ||
                (
                    (matchCount < toMatch.size()) &&
                    (matchCount >= myString.size())
                )
            )
        {
            string ending = toMatch.substr(matchCount);
            size_t childIndex;           
            
            childIndex = BestMatchChildIndex(ending);
              
            if (childIndex < myChildren.size())
            {
                if (myChildren[childIndex]->MatchMap(ending, treeMap))
                {
                    treeMap.push_front(childIndex);
                    return true;
                }
            }
        }
    }
 
    return false;
}

std::string AutoComplete::MapToString(std::deque< size_t >& treeMap) const
{
    if (IsLeaf() || treeMap.empty())
    {
        return myString;
    }
    else
    {
        size_t childIndex;
        
        childIndex = treeMap.front();
        treeMap.pop_front();
        
        return myString + myChildren[childIndex]->MapToString(treeMap);
    }
}


std::vector<std::string> AutoComplete::MapToStringAndTails(std::deque<size_t>& treeMap) const
{
    vector<string> result;
    
    if (IsLeaf() || treeMap.empty())
    {
        result = GetTails();
    }
    else
    {
        size_t childIndex;
        
        childIndex = treeMap.front();
        treeMap.pop_front();
        
        result = myChildren[childIndex]->MapToStringAndTails(treeMap);
        
        for (auto& dude : result)
        {
            dude = myString + dude;
        }
    }
    
    return result;
}

std::vector<std::string> AutoComplete::GetTails() const
{
    vector<string> result;
    
    if (IsLeaf())
    {
        if (!myString.empty())
        {
            result.push_back(myString);
        }
    }
    else
    {
        for (auto& child : myChildren)
        {
            for (auto tail : child->GetTails())
            {
                result.push_back(myString + tail);
            }
        }
    }
    
    return result;
}

bool AutoComplete::IsLeaf() const
{
    return (myChildren.empty());
}
