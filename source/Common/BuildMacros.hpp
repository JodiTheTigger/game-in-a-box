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

#ifndef BUILDMACROS_H
#define BUILDMACROS_H

// Don't allow copying, I want the compiler to tell me if I'm accidentally doing a copy. 
// Effective c++: don't use macros when the compiler can do it. Remove this macro
// and use a baseclass where the copy and assign operator is private (or was that protected, can't remember).
// HOWEVER! Weffc++ doesn't pick up on that so throws warnings. This macro is staying.
// RAM: TODO: I've removed Weffc++ because of false positives, plesae revisit.
// RAM: TODO: Make an "access.h" and add the classes: NoCopy, NoAssign, NoMove.
#define CLASS_NOCOPY_ASSIGN_MOVE(CLASSNAME) private: \
    CLASSNAME(const CLASSNAME &) = delete; \
    CLASSNAME &operator=(const CLASSNAME &) = delete; \
    CLASSNAME(CLASSNAME&&) = delete; \
    CLASSNAME &operator=(CLASSNAME&&) = delete;

#endif // BUILDMACROS_H
