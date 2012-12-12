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

bool AutoComplete::Node::IsEmpty() const
{
    return (myString.size() == 0);
}

size_t AutoComplete::Node::MatchingCharacters(std::string toMatch) const
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
            if (myChildren.size() == 0)
            {
                // easy! just add as a child
                myChildren.push_back(AutoComplete::Node(toInsert));
            }
            else
            {
                Node* bestMatch;
                size_t bestMatchCount;
                             
                // find the best match of the children and choose them
                bestMatch = this;
                bestMatchCount = 0;
                
                for (auto dude : myChildren)
                {
                    size_t match;
                    
                    match = dude.MatchingCharacters(toInsert);
                    if (match > bestMatchCount)
                    {
                        bestMatch = &dude;
                    }
                }
                
                if (bestMatchCount == 0)
                {
                    // add as another child
                    myChildren.push_back(AutoComplete::Node(toInsert));
                }
                else
                {
                    bestMatch->Insert(toInsert);
                }
            }
        }
        else
        {
            // need to split?
            if (myChildren.size() == 0)
            {
                // yup, split
                myString = toInsert.substr(0, matchCount);                
                Insert(toInsert.substr(matchCount));
            }
            else
            {
                // matches any children?
                Insert(toInsert.substr(matchCount));
            }
            // try again with the substring!
            // RAM: TODO! Splits!
            
        }
    }
}

AutoComplete::Node* AutoComplete::Node::BestMatchChild(std::string toMatch) const
{
    // RAM: TODO
    return nullptr;
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

std::vector<std::string> AutoComplete::GetMatchList(std::string)
{
    // TODO!
    return vector<string>();
}

std::string AutoComplete::GetNextBestMatch(std::string)
{
    // TODO!
    return "";
}