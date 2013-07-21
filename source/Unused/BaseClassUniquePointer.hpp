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

#ifndef BASSCLASSUNIQUEPOINTER_HPP
#define BASSCLASSUNIQUEPOINTER_HPP

#include <memory>

namespace GameInABox { namespace Unused {

// /////////////////////
// Unique pointer that properly deletes a derived class
// when stored as its base class.
//
// Based off:
// http://stackoverflow.com/questions/8274451/well-how-does-the-custom-deleter-of-stdunique-ptr-work
//
// Interface at the bottom of the file
//
// /////////////////////

template<typename DerivedType>
void DeleteBaseClassAsDerived(void* baseClass)
{
    delete static_cast<DerivedType*>(baseClass);
}

class BoundDeleter
{
public:
    template<typename DerivedType>
    BoundDeleter(DerivedType* derived)
        : myDeleter(&DeleteBaseClassAsDerived<DerivedType>)
        , myDerivedObject(derived)
    {
    }

    BoundDeleter(BoundDeleter&& other)
        : myDeleter(std::move(other.myDeleter))
        , myDerivedObject(std::move(other.myDerivedObject))
    {
    }

    void operator() (void*) const
    {
        myDeleter(myDerivedObject);
    }

private:
    typedef void (*Deleter)(void*);

    Deleter  myDeleter;
    void*    myDerivedObject;

};

// /////////////////////
// Interface
// /////////////////////
template<typename BaseType>
class BaseClassUniquePointer final : protected std::unique_ptr<BaseType, BoundDeleter>
{
public:
    typedef std::unique_ptr<BaseType, BoundDeleter> Base;

    using Base::operator->;
    using Base::operator*;
    using Base::operator bool;
    using Base::get;

    template<typename ActualType>
    BaseClassUniquePointer(ActualType* derived)
        : Base(derived, BoundDeleter(derived))
    {
    }

    template<class Other>
    BaseClassUniquePointer(BaseClassUniquePointer<Other>&& other)
        : Base(std::move(other))
    {
    }
};

template<typename BaseType, typename DerivedType, typename... Args>
BaseClassUniquePointer<BaseType> make_baseclass_unique(Args&&... args)
{
    return BaseClassUniquePointer<BaseType>(new DerivedType(std::forward<Args>(args)...));
}

}}

#endif // BASSCLASSUNIQUEPOINTER_HPP
