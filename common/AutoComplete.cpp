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

#include "AutoComplete.h"

#include <algorithm>

// RAM: debug
#include <iostream>

using namespace std;

// RAM: NOTE implemented as a Trie: http://login2win.blogspot.co.nz/2011/06/c-tries.html
// RAM: NOTE implemented as a Radix http://code.google.com/p/radixtree/source/browse/trunk/RadixTree/src/ds/tree/
// http://en.wikipedia.org/wiki/Radix_tree

// /////
// Node
// /////
AutoComplete::Node::Node(std::string item) 
: myString(item) 
{
    
}

bool AutoComplete::Node::IsLeaf() const
{
    return (myChildren.empty());
}

size_t AutoComplete::Node::MatchingCharacters(const std::string& toMatch) const
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

void AutoComplete::Node::Insert(std::string toInsert)
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
                myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(toInsert)));
            }
            else
            {
                size_t bestChild;
                
                bestChild = BestMatchChildIndex(toInsert);
                
                if (bestChild == myChildren.size())
                {
                    // add as another child
                    myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(toInsert)));
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
                    myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(tail)));
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
                        myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(tail)));                        
                    }                       
                }
            }
            else
            {
                // Bah, have to split my string, move nodes, and stuff.
                unique_ptr<Node> mrSplit;
                string oldTail;        
                
                oldTail = myString.substr(matchCount);                
                mrSplit.reset(new Node(oldTail)); 
                
                // move the children across
                while (!myChildren.empty())
                {
                    mrSplit->myChildren.push_back(move(myChildren.back()));
                }
                
                // change my string to the smaller common one, and add the new as it's first child.
                myString = myString.substr(0, matchCount);
                myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(tail)));    
                
                // Don't forget the old tail
                myChildren.push_back(move(mrSplit));
            }
        }
    }
}

size_t AutoComplete::Node::BestMatchChildIndex(const std::string& toMatch) const
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

bool AutoComplete::Node::MatchMap(const string& toMatch, std::deque< size_t >& treeMap)
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
            string beginning = toMatch.substr(0, matchCount);
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

std::string AutoComplete::Node::MapToString(std::deque< size_t >& treeMap)
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


std::vector<std::string> AutoComplete::Node::MapToStringAndTails(std::deque<size_t>& treeMap)
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

std::vector<std::string> AutoComplete::Node::GetTails() const
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


// /////
// Auto Complete
// /////
AutoComplete::AutoComplete(std::vector<std::string> wordList) : myRoot(Node())
{
    for (string newWord : wordList)
    {
        myRoot.Insert(newWord);
    }
}

std::vector<std::string> AutoComplete::GetMatchList(std::string toMatch)
{
    std::vector<std::string> result;
    deque<size_t> theMap;
    
    if (myRoot.MatchMap(toMatch, theMap))
    {
        result = myRoot.MapToStringAndTails(theMap);
    }
    
    return result;
}

std::string AutoComplete::GetNextBestMatch(std::string toMatch)
{
    string mapResult;
    
    deque<size_t> theMap;
        
    if (myRoot.MatchMap(toMatch, theMap))
    {
        mapResult = myRoot.MapToString(theMap);
    }
    
    return mapResult;
}