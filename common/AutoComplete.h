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

#ifndef AUTOCOMPLETE_H
#define AUTOCOMPLETE_H

#include <vector>
#include <string>
#include <memory>

class AutoComplete
{
public:
    AutoComplete(std::vector<std::string> wordList);
    
    // Returns a list of all complete strings with the prefix of the searchString
    // Can be empty if nothing matches, or only contain one if there is only
    // one possible match (yay auto-complete to that).
    std::vector<std::string> GetMatchList(std::string searchString);
    
    // Returns the next best match for the string. If there is more
    // than one string that matches the base string, then the base
    // string is returned. If nothing matches, then an empty string.
    // is returned.
    std::string GetNextBestMatch(std::string baseString);

private:
    class Node
    {
    public:
        Node(char item) : Node(item, false) {}
        Node(char item, bool isEndOfWord);
        
        bool IsOnlyBranch();
        void AddChild(Node child);
        
    private:
        std::vector<std::unique_ptr<Node>> myChildren;
    };
    
    std::unique_ptr<Node> myRoot;
};

#endif // AUTOCOMPLETE_H
