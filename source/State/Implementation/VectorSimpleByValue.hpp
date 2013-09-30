#ifndef VectorSimpleByValue_HPP
#define VectorSimpleByValue_HPP

#include <array>
#include <cmath>

namespace GameInABox { namespace State { namespace Implementation {

struct VectorSimpleByValue
{
    std::array<float, 4> values;
};

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Increment / Decrement
// ///////////////////
inline VectorSimpleByValue operator++(VectorSimpleByValue rhs)
{
    return VectorSimpleByValue
    {{{
        ++(rhs.values[0]),
        ++(rhs.values[1]),
        ++(rhs.values[2]),
        ++(rhs.values[3]),
    }}};
}

inline VectorSimpleByValue operator++(VectorSimpleByValue lhs, int)
{
   return ++lhs;
}

inline VectorSimpleByValue operator--(VectorSimpleByValue rhs)
{
    return VectorSimpleByValue
    {{{
        --(rhs.values[0]),
        --(rhs.values[1]),
        --(rhs.values[2]),
        --(rhs.values[3]),
    }}};
}

inline VectorSimpleByValue operator--(VectorSimpleByValue lhs, int)
{
   return --lhs;
}

// ///////////////////
// Comparison Operators
// ///////////////////

inline bool operator==(VectorSimpleByValue lhs, VectorSimpleByValue rhs){return lhs.values==rhs.values;}
inline bool operator!=(VectorSimpleByValue lhs, VectorSimpleByValue rhs){return  !operator==(lhs,rhs);}
inline bool operator< (VectorSimpleByValue lhs, VectorSimpleByValue rhs){return lhs.values<rhs.values;}
inline bool operator> (VectorSimpleByValue lhs, VectorSimpleByValue rhs){return   operator< (rhs,lhs);}
inline bool operator<=(VectorSimpleByValue lhs, VectorSimpleByValue rhs){return  !operator> (lhs,rhs);}
inline bool operator>=(VectorSimpleByValue lhs, VectorSimpleByValue rhs){return  !operator< (lhs,rhs);}

// ///////////////////
// Simple Maths
// ///////////////////
inline constexpr VectorSimpleByValue operator-(const VectorSimpleByValue& lhs)
{
    return VectorSimpleByValue
    {{{
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
        -lhs.values[3]
    }}};
}

inline VectorSimpleByValue operator+(VectorSimpleByValue lhs, VectorSimpleByValue rhs)
{
    return VectorSimpleByValue
    {{{
            lhs.values[0] + rhs.values[0],
            lhs.values[1] + rhs.values[1],
            lhs.values[2] + rhs.values[2],
            lhs.values[3] + rhs.values[3]
    }}};
}

inline VectorSimpleByValue operator-(VectorSimpleByValue lhs, VectorSimpleByValue rhs)
{
    return VectorSimpleByValue
    {{{
            lhs.values[0] - rhs.values[0],
            lhs.values[1] - rhs.values[1],
            lhs.values[2] - rhs.values[2],
            lhs.values[3] - rhs.values[3]
    }}};
}

inline VectorSimpleByValue operator*(VectorSimpleByValue lhs, VectorSimpleByValue rhs)
{
    return VectorSimpleByValue
    {{{
            lhs.values[0] * rhs.values[0],
            lhs.values[1] * rhs.values[1],
            lhs.values[2] * rhs.values[2],
            lhs.values[3] * rhs.values[3]
    }}};
}

inline VectorSimpleByValue operator/(VectorSimpleByValue lhs, VectorSimpleByValue rhs)
{
    return VectorSimpleByValue
    {{{
            lhs.values[0] / rhs.values[0],
            lhs.values[1] / rhs.values[1],
            lhs.values[2] / rhs.values[2],
            lhs.values[3] / rhs.values[3]
    }}};
}

inline VectorSimpleByValue& operator+=(VectorSimpleByValue& lhs, const VectorSimpleByValue& rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

inline VectorSimpleByValue& operator-=(VectorSimpleByValue& lhs, const VectorSimpleByValue& rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

inline VectorSimpleByValue& operator*=(VectorSimpleByValue& lhs, const VectorSimpleByValue& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

inline VectorSimpleByValue& operator/=(VectorSimpleByValue& lhs, const VectorSimpleByValue& rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

// ///////////////////
// Complicated Maths
// ///////////////////
inline VectorSimpleByValue Sqrt(VectorSimpleByValue rhs)
{
    return VectorSimpleByValue
    {{{
            std::sqrt(rhs.values[0]),
            std::sqrt(rhs.values[1]),
            std::sqrt(rhs.values[2]),
            std::sqrt(rhs.values[3])
    }}};
}

inline VectorSimpleByValue Dot(VectorSimpleByValue lhs, VectorSimpleByValue rhs)
{
    lhs *= rhs;
    auto sum = lhs.values[0] + lhs.values[1] + lhs.values[2] + lhs.values[3];
    return VectorSimpleByValue
    {{{
         sum,
         sum,
         sum,
         sum
    }}};
}

inline VectorSimpleByValue Length(VectorSimpleByValue rhs)
{
    float length = sqrt(rhs.values[0] + rhs.values[1] + rhs.values[2] + rhs.values[3]);
    return VectorSimpleByValue
    {{{
         length,
         length,
         length,
         length
    }}};
}

inline VectorSimpleByValue LengthSquared(VectorSimpleByValue rhs)
{
    auto sum = rhs.values[0] + rhs.values[1] + rhs.values[2] + rhs.values[3];
    return VectorSimpleByValue
    {{{
         sum,
         sum,
         sum,
         sum
    }}};
}

inline VectorSimpleByValue Mad(VectorSimpleByValue lhs, VectorSimpleByValue rhs, VectorSimpleByValue add)
{
    return VectorSimpleByValue
    {{{
        fmaf(lhs.values[0], rhs.values[0], add.values[0]),
        fmaf(lhs.values[1], rhs.values[1], add.values[1]),
        fmaf(lhs.values[2], rhs.values[2], add.values[2]),
        fmaf(lhs.values[3], rhs.values[3], add.values[3])
    }}};
}

inline VectorSimpleByValue NormaliseFast(VectorSimpleByValue lhs)
{
    // RAM: TODO!
    return lhs;
}

inline VectorSimpleByValue NormaliseAccurate(VectorSimpleByValue lhs)
{
    // RAM: TODO!
    return lhs;
}

// ///////////////////
// Access
// ///////////////////
inline float X(VectorSimpleByValue rhs){ return rhs.values[0]; }
inline float Y(VectorSimpleByValue rhs){ return rhs.values[1]; }
inline float Z(VectorSimpleByValue rhs){ return rhs.values[2]; }
inline float W(VectorSimpleByValue rhs){ return rhs.values[3]; }

}}} // namespace

#endif // VectorSimpleByValue_HPP
