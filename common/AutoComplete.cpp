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
AutoComplete::Node::Node(std::string item, bool isWordEnd) 
: myStringIsWordEnd(isWordEnd)
, myString(item) 
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
            if (IsLeaf())
            {
                std::cout << " -> Add to Child(0)";
                
                // easy! just add as a child
                myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(toInsert, true)));
            }
            else
            {
                size_t bestChild;
                
                bestChild = BestMatchChildIndex(toInsert);
                
                if (bestChild == myChildren.size())
                {
                    std::cout << " -> Add as Child(1+)";
                    
                    // add as another child
                    myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(toInsert, true)));
                }
                else
                {
                    std::cout << " -> Insert to Child (" << myChildren[bestChild]->myString << ")";
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
                    std::cout << " -> Insert tail (" << tail << ") as Child";
                    
                    // add as a child, easy
                    myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(tail, true)));
                }
                else
                {
                    size_t match;
                    
                    // test against the childen, and add to them, or if
                    // no one matches, add as another child.
                    match = BestMatchChildIndex(tail);
                    
                    if (match < myChildren.size())
                    {
                        std::cout << " -> Insert tail (" << tail << ") to Child";
                        myChildren[match]->Insert(tail);
                    }
                    else
                    {
                        std::cout << " -> Insert tail (" << tail << ") as another Child";
                        myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(tail, true)));                        
                    }                       
                }
            }
            else
            {
                // Bah, have to split my string, move nodes, and stuff.
                unique_ptr<Node> mrSplit;
                string oldTail;
                                
                oldTail = myString.substr(matchCount);                
                mrSplit.reset(new Node(oldTail, true)); 
                
                std::cout << " -> Insert as split! (" << myString.substr(0, matchCount) << ")";    
                
                // move the children across
                while (!myChildren.empty())
                {
                    mrSplit->myChildren.push_back(move(myChildren.back()));
                }
                
                // change my string to the smaller common one, and add the new as it's first child.
                myString = myString.substr(0, matchCount);
                myStringIsWordEnd = false;
                myChildren.push_back(unique_ptr<Node>(new AutoComplete::Node(tail, true)));    
                
                // Don't forget the old tail
                myChildren.push_back(move(mrSplit));
            }
        }
    }
                    
    std::cout << std::endl << "Done: " << argh << std::endl;
    --argh;
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

std::string AutoComplete::Node::NextMatchWithChildren(const std::string& toMatch) const
{
    string result;
    
    // empty match, and only one child means match against that child.
    if (toMatch.empty() && (myChildren.size() == 1))
    {
        return myChildren[0]->myString;
    }
    
    auto bestChild = BestMatchChildIndex(toMatch);
            
    // no child match == no match
    if (bestChild < myChildren.size())
    {
        string tailMatch;
        
        tailMatch = myChildren[bestChild]->NextMatch(toMatch);        
        
        cout << " ChildMatch (" << tailMatch << ")";
        
        // only match if they do
        if (!tailMatch.empty())
        {
            result = myString + tailMatch;
        }
    }
    
    return result;
}


std::string AutoComplete::Node::ArghMatch(const std::string& toMatch, const std::string& base)
{
    size_t matchCount;
    
    matchCount = MatchingCharacters(toMatch);
    
    if ((matchCount == toMatch.size()) && (matchCount <= myString.size()))
    {
        return base + myString;
    }
    else 
    {
        if (myString.empty() || (matchCount < toMatch.size() && matchCount >= myString.size())) 
        {
            string beginning = toMatch.substr(0, matchCount);
            string ending = toMatch.substr(matchCount);
            size_t childIndex;           
            
            childIndex = BestMatchChildIndex(ending);
            
            if (childIndex < myChildren.size())
            {
                return myChildren[childIndex]->ArghMatch(ending, base + beginning);
            }
        }
    }
    
    return "";
}


bool AutoComplete::Node::ArghMatchMap(const string& toMatch, std::deque< size_t >& treeMap)
{
    cout << "FIRSTARG: (" << toMatch << ")";
    // special cases
    if (myString.empty() && toMatch.empty())
    {
        if (myChildren.size() == 1)
        {
            treeMap.push_front(0);
        }
        
        return true;
    }
    /* above is generalistaion of this special case.
    if (myString.empty() && toMatch.empty() && myChildren.size() == 1)
    {
        cout << "ARGH:Special.";
        treeMap.push_front(0);
        return true;
    }*/
    
    size_t matchCount;
    
    matchCount = MatchingCharacters(toMatch);
    
    if ((matchCount == toMatch.size()) && (matchCount < myString.size()))
    {
        cout << "ARGH:This (" << toMatch << ")" << endl;
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
            
                cout << "ARGH:Try Child (" << ending << ": " << childIndex << ").";
                
            if (childIndex < myChildren.size())
            {
                cout << "ARGH:Got Child.";
                if (myChildren[childIndex]->ArghMatchMap(ending, treeMap))
                {
                    cout << "ARGH:Child (" << childIndex << ")";
                    //auto copied = childIndex;
                    treeMap.push_front(childIndex);
                    cout << "Again (" << treeMap.front() << ":" << treeMap.back() << "=" << treeMap.size() << ")";
                    return true;
                }
            }
            else
            {
                // RAM: debug
                for (size_t i = 0; i < myChildren.size(); i++)
                {
                    cout << "i: " << i << "==" << myChildren[i]->myString[0] << " ";
                }
            }
        }
    }
    
    cout << "ARGH:NO "<<matchCount <<" (" << toMatch << ":" << myString << ")" << endl;
    return false;
}

std::string AutoComplete::Node::MapToString(std::deque< size_t >& treeMap)
{
    if (IsLeaf() || treeMap.empty())
    {
        cout << endl;
        return myString;
    }
    else
    {
        size_t childIndex;
        
        childIndex = treeMap.front();
        treeMap.pop_front();
        
        cout << "MS: " << childIndex;
        
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

void AutoComplete::Node::BestMatchMap(const std::string& toMatch, std::vector<size_t>& map) const
{
    size_t matchCount;
    size_t matchCountChild;
    size_t childIndex;    
    string tail;
    
    cout << "MAP(" << toMatch << ")"<< flush;
    
    if (IsLeaf())
    {
    cout << "LEAF." << flush;
        return;
    }
    
    if (toMatch.empty())
    {
        
    cout << "EMPTY." << flush;
        return;
    }
    
    matchCount = MatchingCharacters(toMatch);    
    
    if (matchCount < myString.size())
    {
        
    cout << "NOT FULL STRING." << flush;
        return;
    }
    
    childIndex = BestMatchChildIndex(toMatch);
    
    if (childIndex == myChildren.size())
    {
        
    cout << "NO CHILD MATCH." << flush;
        return;
    }
    
    tail = toMatch.substr(matchCount);
    matchCountChild = myChildren[childIndex]->MatchingCharacters(tail);
    
    if ((matchCountChild == tail.size()) || (matchCountChild ==  myChildren[childIndex]->myString.length()))
    {
    cout << "child (" <<childIndex << ")." << flush;
        map.push_back(childIndex);
        myChildren[childIndex]->BestMatchMap(tail, map);
    }
    cout << "DONE" << flush;
    
    
    
    
/*
    
    if (myString.empty())
    {
        cout << "ROOT." << flush;
        // root! special case.
        if (myChildren.size() == 1)
        {
    cout << "ROOTCHILD." << flush;
            map.push_back(0);
            myChildren[0]->BestMatchMap(toMatch, map);
        }
        
        return;
    }
    
    
    if (matchCount == 0)
    {
    cout << "NOMATCH." << flush;
        return;
    }
  */  
}

std::string AutoComplete::Node::NextMatch(const std::string& toMatch) const
{
    std::string result;
    
    cout << " NextMatch (" << toMatch << ")" << flush;
    
    // if im the root, then pass it off to the children
    if (myString.empty())
    {
        if (!IsLeaf())
        {
            cout << " root" << flush;
            result = NextMatchWithChildren(toMatch);
        }
    }
    else
    {                        
        if (!toMatch.empty())
        {
            size_t matchCount;
            
            //result = myString;
            matchCount = MatchingCharacters(toMatch);
            cout << ": " << matchCount << flush;
            if (matchCount == 0)
            {
                cout << " empty" << flush;
                result = "";
            }
            else
            {
                // the string matches and is smaller than myString, then
                // expand out to my string
                if (matchCount == toMatch.size())
                {
                    if (myString.size() >= toMatch.size())
                    {
                            cout << " my string expand" << flush;
                        result = myString;
                    }
                }
                else
                {
                    // implies that toMath.size() > myString.size()
                    // because if it isn't then there isn't a match
                    // and we return nothing.
                    if (myString.size() == matchCount)
                    {
                        // string matches, but is too long so no match at all really.
                        if (!IsLeaf())
                        {                            
                            cout << " Children" << flush;
                            string tail;
                            
                            tail = toMatch.substr(matchCount);                               
                            result = NextMatchWithChildren(tail);
                        }
                    }
                }
            }
        }
        else
        {
                    cout << " My String" << flush;
            result = myString;
        }
    }
    cout << " Done (" << result << ")" << endl << flush;
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


std::vector<std::string> AutoComplete::Node::GetMatchList(const std::string& toMatch) const
{
    vector<string> result;
    
    if (!toMatch.empty())
    {    
        size_t matchCount;
        
        matchCount = MatchingCharacters(toMatch);
        cout << "Match: " << toMatch << ": " << matchCount << flush;
        
        if (matchCount == myString.size())
        {
            if (matchCount == toMatch.size())
            {
                cout << " Tails straight" << flush;
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
                    cout << " Tails children" << flush;
                    string shorter;
                    
                    shorter = toMatch.substr(matchCount);
                    
                    auto bestChild = BestMatchChildIndex(shorter);
                    
                    if (bestChild < myChildren.size())
                    {                    
                        for (string tail : myChildren[bestChild]->GetMatchList(shorter))
                        {
                    cout << " Tails children (" << tail << ")" << flush;
                            result.push_back(myString + tail);
                        }
                    }
                }
                else
                {
                    if (myString.size() > 0)
                    {
                        cout << " Me only" << flush;
                        result.push_back(myString);
                    }
                }
            }
        }
        else
        {
            if (matchCount == toMatch.size())
            {
                cout << " Substring" << flush;
                result.push_back(myString);
            }
        }
    }
    else
    {
        // empty match, match all!
        cout << " MATCH ALL!" << flush;
        for (string tail : GetTails())
        {
            cout << " tail (" << tail << ")" << flush;
            result.push_back(myString + tail);
        }
    }
    cout << " Match Done (" << result.size() << ")" << endl << flush;
    return result;
}

void AutoComplete::Node::PrintTree()
{
    if (IsLeaf())
    {
        cout << myString << endl;
    }
    else
    {
        for (auto& child : myChildren)
        {
            cout << myString;
            child->PrintTree();
        }
    }
}

// /////
// Auto Complete
// /////
AutoComplete::AutoComplete(std::vector<std::string> wordList) : myRoot(Node())
{
    for (string newWord : wordList)
    {
        myRoot.Insert(newWord);
        cout << " ============== " << endl;
        myRoot.PrintTree();
    }
        cout << " ======xxx===== " << endl;
    
}

std::vector<std::string> AutoComplete::GetMatchList(std::string toMatch)
{
    std::vector<std::string> result;
    deque<size_t> theMap;
    
    if (myRoot.ArghMatchMap(toMatch, theMap))
    {
        cout << "MAP IS: ";
        for (auto point : theMap)
        {
            cout << point << "<-";
        }
        cout << "start" << endl << flush;
        result = myRoot.MapToStringAndTails(theMap);
    }
    
    return result;
    
    //return myRoot.GetMatchList(toMatch);
}

std::string AutoComplete::GetNextBestMatch(std::string toMatch)
{
    /*
    {
        vector<size_t> theMap;
        
        myRoot.BestMatchMap(toMatch, theMap);
        
        cout << endl << "map: " << toMatch << ": ";
        for (auto stone : theMap)
        {
            cout << "-> " << stone;
        }
        cout << "." << endl;
    }
    
    return myRoot.NextMatch(toMatch);
    */
    string mapResult;
    
    deque<size_t> theMap;
        
    if (myRoot.ArghMatchMap(toMatch, theMap))
    {
        if (!theMap.empty())
        {
        cout << "frontback:" << theMap.front() << ":" << theMap.back() << endl;
        cout << endl << "MAP IS: ";
        for (size_t point : theMap)
        {
            cout << point << " <- ";
        }
        cout << "start" << endl << flush;
        }
        mapResult = myRoot.MapToString(theMap);
        string result = myRoot.ArghMatch(toMatch, "");
        cout << "ARGH@! " << toMatch << " -> " << result << ":" << mapResult << endl; 
    }
    return mapResult;
    //return myRoot.ArghMatch(toMatch, "");
}