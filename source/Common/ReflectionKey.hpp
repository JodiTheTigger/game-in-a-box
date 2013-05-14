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

#ifndef IREFLECTIONKEY_H
#define IREFLECTIONKEY_H

#ifndef USING_PRECOMPILED_HEADERS
#include <memory>
#endif

namespace GameInABox { namespace Common {

// ReflectionKey notes:
// =================
// class ReflectionKey;
//    Use the pimpl idiom for the reflection key. It's basically a handle that contains data useful for the reflection class.
//    I have to define the constructor of ReflectionKey after ReflectionKeyPrivate has been
//    full defined, otherwise I get compile errors in the form:
//    invalid use of sizeof() of incomplete class
//    The solution is to have the constructor done after the Pimpl class is fully defined, so the compiler 
//    doesn't treat the class as fully complete in the header and therefore do sizeof stuff straight away.
//    http://stackoverflow.com/questions/5460186/preventing-header-explosion-in-c-or-c0x
//
//    I overload the -> operator so that accessing the private data isn't via a getter function. Looks prettier.

// forward declarations:
class ReflectionKeyPrivate;

class ReflectionKey
{
public:
    ReflectionKey();
    ReflectionKey(ReflectionKeyPrivate* data);
    
    const ReflectionKeyPrivate* operator->() const 
    {
        return myPimpl.get();
    }
    
    // for std::map
    bool operator<(const ReflectionKey& other) const 
    {
        return (this->myPimpl < other.myPimpl);
    }
    
    ~ReflectionKey();
private:
    std::shared_ptr<ReflectionKeyPrivate> myPimpl;
};

}} // namespace

#endif // IREFLECTIONKEY_H
