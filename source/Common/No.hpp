/*
    Game-in-a-box. Simple First Person Shooter Network Game.
    Copyright (C) 2012-2013 Richard Maxwell <jodi.the.tigger@gmail.com>
    
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

#ifndef NO_H
#define NO_H

class NoCopy
{
protected:
    NoCopy(const NoCopy&) = delete;

    NoCopy() = default;
    //NoCopy(const NoCopy&) = default;
    NoCopy(NoCopy&&) = default;
    NoCopy& operator=(const NoCopy&) = default;
    NoCopy& operator=(NoCopy&&) = default;
    ~NoCopy() = default;
};

class NoAssign
{
protected:
    NoAssign &operator=(const NoAssign &) = delete;

    NoAssign() = default;
    NoAssign(const NoAssign&) = default;
    NoAssign(NoAssign&&) = default;
    //NoAssign& operator=(const NoAssign&) = default;
    NoAssign& operator=(NoAssign&&) = default;
    ~NoAssign() = default;
};

class NoMove
{
protected:
    NoMove(NoMove&&) = delete;
    NoMove &operator=(NoMove&&) = delete;

    NoMove() = default;
    NoMove(const NoMove&) = default;
    //NoMove(NoMove&&) = default;
    NoMove& operator=(const NoMove&) = default;
    //NoMove& operator=(NoMove&&) = default;
    ~NoMove() = default;
};

#endif // NO_H
