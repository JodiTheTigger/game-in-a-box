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
    return (myChildren.size() == 0);
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
// RAM: debug
static int argh = 0;

void AutoComplete::Node::Insert(std::string toInsert)
{
    ++argh;
    
    std::cout << std::endl << " Insert (" << toInsert << "): " << argh;
    
    // ignore duplicates
    if (toInsert != myString)
    {
        size_t matchCount;
            
        matchCount = MatchingCharacters(toInsert);
        
        std::cout << " Match: " << matchCount << std::endl;
        
        if (matchCount == 0)
        {
            if (myChildren.size() == 0)
            {
                std::cout << " -> Add to Child(0)";
                
                // easy! just add as a child
                myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(toInsert)));
                
                cout << " Test (" << myChildren[0]->myString << ")";
            }
            else
            {
                auto bestChild = BestMatchChild(toInsert);
                
                if (bestChild == nullptr)
                {
                    std::cout << " -> Add as Child(1+)";
                    
                    // add as another child
                    myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(toInsert)));
                }
                else
                {
                    std::cout << " -> Insert to Child (" << (*bestChild)->myString << ")";
                    (*bestChild)->Insert(toInsert);
                }
            }
        }
        else
        {
            // need to split?
            if (myChildren.size() == 0)
            {               
                std::cout << 
                    " -> Split to " << 
                    myString.substr(matchCount) << 
                    " (child) " << 
                    toInsert.substr(0, matchCount) << 
                    "(myString)";
                    
                // insert the split off bit as a child.
                myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(myString.substr(matchCount))));
                myString = toInsert.substr(0, matchCount); 
                
                    std::cout << " -> Split and Add (" << toInsert.substr(matchCount) << ") Recursive";
                    
                Insert(toInsert.substr(matchCount));
            }
            else
            {   
                string toInsertSubstring;
                
                toInsertSubstring = toInsert.substr(matchCount);
                
                // matches any children?                
                auto bestChild = BestMatchChild(toInsertSubstring);
                
                if (bestChild == nullptr)
                {
                    std::cout << " -> Split and Add (" << toInsertSubstring << ") as Child";
                    
                    // No, then just add the substring as a child.
                    myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(toInsertSubstring)));
                }
                else
                {
                    std::cout << " -> Split and Insert (" << toInsertSubstring << ") to Child(" << (*bestChild)->myString << ")";
                    (*bestChild)->Insert(toInsertSubstring);
                }                
            }
        }
    }
    std::cout << std::endl << "Done: " << argh << std::endl;
    --argh;
}

const unique_ptr<AutoComplete::Node>* AutoComplete::Node::BestMatchChild(const std::string& toMatch) const
{
    const unique_ptr<Node>* bestMatch;
    size_t bestMatchCount;
                    
    // find the best match of the children and choose them
    bestMatch = nullptr;
    bestMatchCount = 0;
    
    for (auto& dude : myChildren)
    {
        size_t match;
        
        match = dude->MatchingCharacters(toMatch);
        if (match > bestMatchCount)
        {
            bestMatch = &dude;
            bestMatchCount = match;
        }
    }
        
    return bestMatch;
}


std::string AutoComplete::Node::NextMatch(const std::string& toMatch) const
{
    std::string result;
    cout << "NextMatch (" << toMatch << ")";
    if (!toMatch.empty())
    {
        size_t matchCount;
        
        result = myString;
        matchCount = MatchingCharacters(toMatch);
        cout << ": " << matchCount;
        if ((matchCount == 0) && (!myString.empty()))
        {
            cout << " empty";
            result = "";
        }
        else
        {
            if (myString.size() == matchCount)
            {
                if ((!IsLeaf()) && (toMatch != myString))
                {
                    cout << " Children";
                    string shorter;
                    
                    shorter = toMatch.substr(matchCount);
                    
                    auto bestChild = BestMatchChild(shorter);
                    
                    result = myString + (*bestChild)->NextMatch(shorter);
                }
            }
        }
    }
    cout << " Done" << endl;
    return result;
}

std::vector<std::string> AutoComplete::Node::GetTails() const
{
    vector<string> result;
    
    if (IsLeaf())
    {
        result.push_back(myString);
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


std::vector<std::string> AutoComplete::Node::GetMatchList(const std::string& toMatch) const
{
    vector<string> result;
    
    if (!toMatch.empty())
    {
    
        size_t matchCount;
        
        matchCount = MatchingCharacters(toMatch);
        cout << "Match: " << toMatch << ": " << matchCount;
        
        if (matchCount >= myString.size())
        {
            if (matchCount == toMatch.size())
            {
                cout << " Tails straight";
                // means this node is an exact match, return the children.
                for (string tail : GetTails())
                {
                    result.push_back(myString + tail);
                }
            }
            else
            {    
                // implicit toMatch.size() > myString.size()        
                if (!IsLeaf())
                {
                    cout << " Tails children";
                    string shorter;
                    
                    shorter = toMatch.substr(matchCount);
                    
                    auto bestChild = BestMatchChild(shorter);
                    
                    for (string tail : (*bestChild)->GetTails())
                    {
                        result.push_back(myString + tail);
                    }
                }
                else
                {
                    if (myString.size() > 0)
                    {
                        cout << " Me only";
                        result.push_back(myString);
                    }
                }
            }
        }
    }
    cout << " Match Done " << endl;
    return result;
}

// /////
// Auto Complete
// /////
AutoComplete::AutoComplete(std::vector<std::string> wordList) : myRoot(Node(""))
{
    for (string newWord : wordList)
    {
        myRoot.Insert(newWord);
    }
}

std::vector<std::string> AutoComplete::GetMatchList(std::string toMatch)
{
    return myRoot.GetMatchList(toMatch);
}

std::string AutoComplete::GetNextBestMatch(std::string toMatch)
{
    return myRoot.NextMatch(toMatch);
}