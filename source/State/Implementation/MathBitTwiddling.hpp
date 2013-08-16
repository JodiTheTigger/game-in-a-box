#ifndef MATHBITTWIDDLING_HPP
#define MATHBITTWIDDLING_HPP

#include "Units.hpp"

namespace GameInABox { namespace State { namespace Implementation {

constexpr unsigned BitsNeeded(unsigned value, unsigned bits)
{
    return ((1U << bits) > value) ? bits : BitsNeeded(value, bits + 1);
}

constexpr Bits BitsNeeded(unsigned value)
{
    return Bits{static_cast<signed>(BitsNeeded(value, 1))};
}

template<typename T>
class BitsForEnum
{
public:
    static constexpr Bits Result()
    {
        return BitsNeeded(static_cast<unsigned>(T::MaxValue));
    }
};

}}} // namespace

#endif // MATHBITTWIDDLING_HPP
