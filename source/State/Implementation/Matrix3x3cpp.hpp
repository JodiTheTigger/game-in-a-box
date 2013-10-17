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

#ifndef MATRIX3X3CPP_HPP
#define MATRIX3X3CPP_HPP

#include "Vector3cpp.hpp"
#include "QuaternionCpp.hpp"

#define Matrix3x3cppData	Matrix3x3cppFloatData

namespace GameInABox { namespace State { namespace Implementation {

struct alignas(16) Matrix3x3cpp
{
    std::array<Vector3cpp, 3> values;

    constexpr explicit Matrix3x3cpp(std::array<Vector3cpp, 3> vectors)
        : values(vectors) {}

    // RAM: TODO:
    constexpr explicit Matrix3x3cpp(const QuaternionCpp& rotation)
        : values() {}

    constexpr Matrix3x3cpp(Vector3cpp x, Vector3cpp y, Vector3cpp z)
        : values{{x, y, z}} {}

    // row major
    constexpr Matrix3x3cpp(
                float xx, float xy, float xz,
                float yx, float yy, float yz,
                float zx, float zy, float zz)
        : values{{
              Vector3cpp(xx,xy,xz),
              Vector3cpp(yx,yy,yz),
              Vector3cpp(zx,zy,zz)}} {}

    constexpr Matrix3x3cpp(
                float xx, float xy, float xz,
                float yx, float yy, float yz,
                float zx, float zy, float zz)
        : values{{
              Vector3cpp(xx,xy,xz),
              Vector3cpp(yx,yy,yz),
              Vector3cpp(zx,zy,zz)}} {}
};

// ///////////////////
// Operators
// ///////////////////
// Taken from http://stackoverflow.com/questions/4421706/operator-overloading/4421719
// However all "inclass" operators changed to out of class.

// ///////////////////
// Comparison Operators
// ///////////////////

inline bool operator==(const Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs){return lhs.values==rhs.values;}
inline bool operator!=(const Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs){return  !operator==(lhs,rhs);}

// ///////////////////
// Simple Maths
// ///////////////////
inline Matrix3x3cpp& operator+=(Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs)
{
    lhs.values[0] += rhs.values[0];
    lhs.values[1] += rhs.values[1];
    lhs.values[2] += rhs.values[2];
    return lhs;
}

inline Matrix3x3cpp& operator-=(Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs)
{
    lhs.values[0] -= rhs.values[0];
    lhs.values[1] -= rhs.values[1];
    lhs.values[2] -= rhs.values[2];
    return lhs;
}

inline Matrix3x3cpp& operator*=(Matrix3x3cpp& lhs, const Matrix3x3cpp& rhs)
{
    auto& l         = lhs.values;
    const auto& r   = rhs.values;

    auto result = Matrix3x3cpp
    {
            (l[0].values[0] * r[0].values[0]) + (l[0].values[1] * r[1].values[0]) + (l[0].values[2] * r[2].values[0]),
            (l[0].values[0] * r[0].values[1]) + (l[0].values[1] * r[1].values[1]) + (l[0].values[2] * r[2].values[1]),
            (l[0].values[0] * r[0].values[2]) + (l[0].values[1] * r[1].values[2]) + (l[0].values[2] * r[2].values[2]),

            (l[1].values[0] * r[0].values[0]) + (l[1].values[1] * r[1].values[0]) + (l[1].values[2] * r[2].values[0]),
            (l[1].values[0] * r[0].values[1]) + (l[1].values[1] * r[1].values[1]) + (l[1].values[2] * r[2].values[1]),
            (l[1].values[0] * r[0].values[2]) + (l[1].values[1] * r[1].values[2]) + (l[1].values[2] * r[2].values[2]),

            (l[2].values[0] * r[0].values[0]) + (l[2].values[1] * r[1].values[0]) + (l[2].values[2] * r[2].values[0]),
            (l[2].values[0] * r[0].values[1]) + (l[2].values[1] * r[1].values[1]) + (l[2].values[2] * r[2].values[1]),
            (l[2].values[0] * r[0].values[2]) + (l[2].values[1] * r[1].values[2]) + (l[2].values[2] * r[2].values[2]),
    };

    lhs = result;
    return lhs;
}

inline constexpr Matrix3x3cpp operator-(const Matrix3x3cpp& lhs)
{
    return Matrix3x3cpp
    {
        -lhs.values[0],
        -lhs.values[1],
        -lhs.values[2],
    };
}

inline Matrix3x3cpp operator+(Matrix3x3cpp lhs, const Matrix3x3cpp& rhs){ lhs += rhs;  return lhs; }
inline Matrix3x3cpp operator-(Matrix3x3cpp lhs, const Matrix3x3cpp& rhs){ lhs -= rhs;  return lhs; }
inline Matrix3x3cpp operator*(Matrix3x3cpp lhs, const Matrix3x3cpp& rhs){ lhs *= rhs;  return lhs; }

// ///////////////////
// Vector and Matrix maths.
// ///////////////////
inline Vector3cpp operator*(const Matrix3x3cpp& lhs, const Vector3cpp& rhs)
{
    // RAM: TODO: Verify maths.
    return Vector3cpp
    {
            Dot(lhs.values[0], rhs),
            Dot(lhs.values[1], rhs),
            Dot(lhs.values[2], rhs),
    };
}

// RAM: TODO: Verify Maths!
inline Vector3cpp operator*(const Vector3cpp& lhs, const Matrix3x3cpp& rhs)
{
    return Vector3cpp
    {
            rhs.values[0].values[0] * lhs.values[0] + rhs.values[1].values[0] * lhs.values[1] + rhs.values[2].values[0] * lhs.values[2] + rhs.values[3].values[0] * lhs.values[3],
            rhs.values[0].values[1] * lhs.values[0] + rhs.values[1].values[1] * lhs.values[1] + rhs.values[2].values[1] * lhs.values[2] + rhs.values[3].values[1] * lhs.values[3],
            rhs.values[0].values[2] * lhs.values[0] + rhs.values[1].values[2] * lhs.values[1] + rhs.values[2].values[2] * lhs.values[2] + rhs.values[3].values[2] * lhs.values[3],
    };
}




// Rotation Matrix only, doesn't support scaling.
struct alignas(16) Matrix3x3cpp
{
    ///Data storage for the matrix, each vector is a row of the matrix
    Vector3cpp values[3];

public:
    /** @brief No initializaion constructor */
    Matrix3x3cpp () {}

    /** @brief Get a column of the matrix as a vector
    *  @param i Column number 0 indexed */
    inline b3Vector3 getColumn(int i) const
    {
        return b3MakeVector3(values[0][i],values[1][i],values[2][i]);
    }


    /** @brief Get a row of the matrix as a vector
    *  @param i Row number 0 indexed */
    inline const b3Vector3& getRow(int i) const
    {
        b3FullAssert(0 <= i && i < 3);
        return values[i];
    }

    /** @brief Get a mutable reference to a row of the matrix as a vector
    *  @param i Row number 0 indexed */
    inline b3Vector3&  operator[](int i)
    {
        b3FullAssert(0 <= i && i < 3);
        return values[i];
    }

    /** @brief Get a const reference to a row of the matrix as a vector
    *  @param i Row number 0 indexed */
    inline const b3Vector3& operator[](int i) const
    {
        b3FullAssert(0 <= i && i < 3);
        return values[i];
    }

    /** @brief Set from the rotational part of a 4x4 OpenGL matrix
    *  @param m A pointer to the beginning of the array of scalars*/
    void setFromOpenGLSubMatrix(const float *m)
    {
        values[0].setValue(m[0],m[4],m[8]);
        values[1].setValue(m[1],m[5],m[9]);
        values[2].setValue(m[2],m[6],m[10]);

    }

    /** @brief Set the matrix from a quaternion
    *  @param q The Quaternion to match */
    void setRotation(const b3Quaternion& q)
    {
        float d = q.length2();
        b3FullAssert(d != float(0.0));
        float s = float(2.0) / d;

        float xs = q.getX() * s,   ys = q.getY() * s,   zs = q.getZ() * s;
        float wx = q.getW() * xs,  wy = q.getW() * ys,  wz = q.getW() * zs;
        float xx = q.getX() * xs,  xy = q.getX() * ys,  xz = q.getX() * zs;
        float yy = q.getY() * ys,  yz = q.getY() * zs,  zz = q.getZ() * zs;
        setValue(
            float(1.0) - (yy + zz), xy - wz, xz + wy,
            xy + wz, float(1.0) - (xx + zz), yz - wx,
            xz - wy, yz + wx, float(1.0) - (xx + yy));
    }


    /** @brief Set the matrix from euler angles using YPR around YXZ respectively
    *  @param yaw Yaw about Y axis
    *  @param pitch Pitch about X axis
    *  @param roll Roll about Z axis
    */
    void setEulerYPR(const float& yaw, const float& pitch, const float& roll)
    {
        setEulerZYX(roll, pitch, yaw);
    }

    /** @brief Set the matrix from euler angles YPR around ZYX axes
    * @param eulerX Roll about X axis
    * @param eulerY Pitch around Y axis
    * @param eulerZ Yaw aboud Z axis
    *
    * These angles are used to produce a rotation matrix. The euler
    * angles are applied in ZYX order. I.e a vector is first rotated
    * about X then Y and then Z
    **/
    void setEulerZYX(float eulerX,float eulerY,float eulerZ) {
        ///@todo proposed to reverse this since it's labeled zyx but takes arguments xyz and it will match all other parts of the code
        float ci ( b3Cos(eulerX));
        float cj ( b3Cos(eulerY));
        float ch ( b3Cos(eulerZ));
        float si ( b3Sin(eulerX));
        float sj ( b3Sin(eulerY));
        float sh ( b3Sin(eulerZ));
        float cc = ci * ch;
        float cs = ci * sh;
        float sc = si * ch;
        float ss = si * sh;

        setValue(cj * ch, sj * sc - cs, sj * cc + ss,
            cj * sh, sj * ss + cc, sj * cs - sc,
            -sj,      cj * si,      cj * ci);
    }

    /**@brief Fill the rotational part of an OpenGL matrix and clear the shear/perspective
    * @param m The array to be filled */
    void getOpenGLSubMatrix(float *m) const
    {
        m[0]  = float(values[0].getX());
        m[1]  = float(values[1].getX());
        m[2]  = float(values[2].getX());
        m[3]  = float(0.0);
        m[4]  = float(values[0].getY());
        m[5]  = float(values[1].getY());
        m[6]  = float(values[2].getY());
        m[7]  = float(0.0);
        m[8]  = float(values[0].getZ());
        m[9]  = float(values[1].getZ());
        m[10] = float(values[2].getZ());
        m[11] = float(0.0);
    }

    /**@brief Get the matrix represented as a quaternion
    * @param q The quaternion which will be set */
    void getRotation(b3Quaternion& q) const
    {
        float trace = values[0].getX() + values[1].getY() + values[2].getZ();

        float temp[4];

        if (trace > float(0.0))
        {
            float s = b3Sqrt(trace + float(1.0));
            temp[3]=(s * float(0.5));
            s = float(0.5) / s;

            temp[0]=((values[2].getY() - values[1].getZ()) * s);
            temp[1]=((values[0].getZ() - values[2].getX()) * s);
            temp[2]=((values[1].getX() - values[0].getY()) * s);
        }
        else
        {
            int i = values[0].getX() < values[1].getY() ?
                (values[1].getY() < values[2].getZ() ? 2 : 1) :
                (values[0].getX() < values[2].getZ() ? 2 : 0);
            int j = (i + 1) % 3;
            int k = (i + 2) % 3;

            float s = b3Sqrt(values[i][i] - values[j][j] - values[k][k] + float(1.0));
            temp[i] = s * float(0.5);
            s = float(0.5) / s;

            temp[3] = (values[k][j] - values[j][k]) * s;
            temp[j] = (values[j][i] + values[i][j]) * s;
            temp[k] = (values[k][i] + values[i][k]) * s;
        }
        q.setValue(temp[0],temp[1],temp[2],temp[3]);
    }

    /**@brief Get the matrix represented as euler angles around YXZ, roundtrip with setEulerYPR
    * @param yaw Yaw around Y axis
    * @param pitch Pitch around X axis
    * @param roll around Z axis */
    void getEulerYPR(float& yaw, float& pitch, float& roll) const
    {

        // first use the normal calculus
        yaw = float(b3Atan2(values[1].getX(), values[0].getX()));
        pitch = float(b3Asin(-values[2].getX()));
        roll = float(b3Atan2(values[2].getY(), values[2].getZ()));

        // on pitch = +/-HalfPI
        if (b3Fabs(pitch)==B3_HALF_PI)
        {
            if (yaw>0)
                yaw-=B3_PI;
            else
                yaw+=B3_PI;

            if (roll>0)
                roll-=B3_PI;
            else
                roll+=B3_PI;
        }
    };


    /**@brief Get the matrix represented as euler angles around ZYX
    * @param yaw Yaw around X axis
    * @param pitch Pitch around Y axis
    * @param roll around X axis
    * @param solution_number Which solution of two possible solutions ( 1 or 2) are possible values*/
    void getEulerZYX(float& yaw, float& pitch, float& roll, unsigned int solution_number = 1) const
    {
        struct Euler
        {
            float yaw;
            float pitch;
            float roll;
        };

        Euler euler_out;
        Euler euler_out2; //second solution
        //get the pointer to the raw data

        // Check that pitch is not at a singularity
        if (b3Fabs(values[2].getX()) >= 1)
        {
            euler_out.yaw = 0;
            euler_out2.yaw = 0;

            // From difference of angles formula
            float delta = b3Atan2(values[0].getX(),values[0].getZ());
            if (values[2].getX() > 0)  //gimbal locked up
            {
                euler_out.pitch = B3_PI / float(2.0);
                euler_out2.pitch = B3_PI / float(2.0);
                euler_out.roll = euler_out.pitch + delta;
                euler_out2.roll = euler_out.pitch + delta;
            }
            else // gimbal locked down
            {
                euler_out.pitch = -B3_PI / float(2.0);
                euler_out2.pitch = -B3_PI / float(2.0);
                euler_out.roll = -euler_out.pitch + delta;
                euler_out2.roll = -euler_out.pitch + delta;
            }
        }
        else
        {
            euler_out.pitch = - b3Asin(values[2].getX());
            euler_out2.pitch = B3_PI - euler_out.pitch;

            euler_out.roll = b3Atan2(values[2].getY()/b3Cos(euler_out.pitch),
                values[2].getZ()/b3Cos(euler_out.pitch));
            euler_out2.roll = b3Atan2(values[2].getY()/b3Cos(euler_out2.pitch),
                values[2].getZ()/b3Cos(euler_out2.pitch));

            euler_out.yaw = b3Atan2(values[1].getX()/b3Cos(euler_out.pitch),
                values[0].getX()/b3Cos(euler_out.pitch));
            euler_out2.yaw = b3Atan2(values[1].getX()/b3Cos(euler_out2.pitch),
                values[0].getX()/b3Cos(euler_out2.pitch));
        }

        if (solution_number == 1)
        {
            yaw = euler_out.yaw;
            pitch = euler_out.pitch;
            roll = euler_out.roll;
        }
        else
        {
            yaw = euler_out2.yaw;
            pitch = euler_out2.pitch;
            roll = euler_out2.roll;
        }
    }

    /**@brief Create a scaled copy of the matrix
    * @param s Scaling vector The elements of the vector will scale each column */

    Matrix3x3cpp scaled(const b3Vector3& s) const
    {
        return Matrix3x3cpp(
            values[0].getX() * s.getX(), values[0].getY() * s.getY(), values[0].getZ() * s.getZ(),
            values[1].getX() * s.getX(), values[1].getY() * s.getY(), values[1].getZ() * s.getZ(),
            values[2].getX() * s.getX(), values[2].getY() * s.getY(), values[2].getZ() * s.getZ());
    }

    /**@brief Return the determinant of the matrix */
    float            determinant() const;
    /**@brief Return the adjoint of the matrix */
    Matrix3x3cpp adjoint() const;
    /**@brief Return the matrix with all values non negative */
    Matrix3x3cpp absolute() const;
    /**@brief Return the transpose of the matrix */
    Matrix3x3cpp transpose() const;
    /**@brief Return the inverse of the matrix */
    Matrix3x3cpp inverse() const;

    Matrix3x3cpp transposeTimes(const Matrix3x3cpp& m) const;
    Matrix3x3cpp timesTranspose(const Matrix3x3cpp& m) const;

    inline float tdotx(const b3Vector3& v) const
    {
        return values[0].getX() * v.getX() + values[1].getX() * v.getY() + values[2].getX() * v.getZ();
    }
    inline float tdoty(const b3Vector3& v) const
    {
        return values[0].getY() * v.getX() + values[1].getY() * v.getY() + values[2].getY() * v.getZ();
    }
    inline float tdotz(const b3Vector3& v) const
    {
        return values[0].getZ() * v.getX() + values[1].getZ() * v.getY() + values[2].getZ() * v.getZ();
    }



    /**@brief Calculate the matrix cofactor
    * @param r1 The first row to use for calculating the cofactor
    * @param c1 The first column to use for calculating the cofactor
    * @param r1 The second row to use for calculating the cofactor
    * @param c1 The second column to use for calculating the cofactor
    * See http://en.wikipedia.org/wiki/Cofactor_(linear_algebra) for more details
    */
    float cofac(int r1, int c1, int r2, int c2) const
    {
        return values[r1][c1] * values[r2][c2] - values[r1][c2] * values[r2][c1];
    }

};


inline Matrix3x3cpp&
Matrix3x3cpp::operator*=(const Matrix3x3cpp& m)
{
    setValue(
        m.tdotx(values[0]), m.tdoty(values[0]), m.tdotz(values[0]),
        m.tdotx(values[1]), m.tdoty(values[1]), m.tdotz(values[1]),
        m.tdotx(values[2]), m.tdoty(values[2]), m.tdotz(values[2]));

    return *this;
}

inline Matrix3x3cpp&
Matrix3x3cpp::operator+=(const Matrix3x3cpp& m)
{
    setValue(
        values[0][0]+m.values[0][0],
        values[0][1]+m.values[0][1],
        values[0][2]+m.values[0][2],
        values[1][0]+m.values[1][0],
        values[1][1]+m.values[1][1],
        values[1][2]+m.values[1][2],
        values[2][0]+m.values[2][0],
        values[2][1]+m.values[2][1],
        values[2][2]+m.values[2][2]);

    return *this;
}

inline Matrix3x3cpp
operator*(const Matrix3x3cpp& m, const float & k)
{
    return Matrix3x3cpp(
        m[0].getX()*k,m[0].getY()*k,m[0].getZ()*k,
        m[1].getX()*k,m[1].getY()*k,m[1].getZ()*k,
        m[2].getX()*k,m[2].getY()*k,m[2].getZ()*k);
}

inline Matrix3x3cpp
operator+(const Matrix3x3cpp& m1, const Matrix3x3cpp& m2)
{
    return Matrix3x3cpp(
        m1[0][0]+m2[0][0],
        m1[0][1]+m2[0][1],
        m1[0][2]+m2[0][2],

        m1[1][0]+m2[1][0],
        m1[1][1]+m2[1][1],
        m1[1][2]+m2[1][2],

        m1[2][0]+m2[2][0],
        m1[2][1]+m2[2][1],
        m1[2][2]+m2[2][2]);
}

inline Matrix3x3cpp
operator-(const Matrix3x3cpp& m1, const Matrix3x3cpp& m2)
{
    return Matrix3x3cpp(
        m1[0][0]-m2[0][0],
        m1[0][1]-m2[0][1],
        m1[0][2]-m2[0][2],

        m1[1][0]-m2[1][0],
        m1[1][1]-m2[1][1],
        m1[1][2]-m2[1][2],

        m1[2][0]-m2[2][0],
        m1[2][1]-m2[2][1],
        m1[2][2]-m2[2][2]);
}


inline Matrix3x3cpp&
Matrix3x3cpp::operator-=(const Matrix3x3cpp& m)
{
    setValue(
    values[0][0]-m.values[0][0],
    values[0][1]-m.values[0][1],
    values[0][2]-m.values[0][2],
    values[1][0]-m.values[1][0],
    values[1][1]-m.values[1][1],
    values[1][2]-m.values[1][2],
    values[2][0]-m.values[2][0],
    values[2][1]-m.values[2][1],
    values[2][2]-m.values[2][2]);

    return *this;
}


inline float
Matrix3x3cpp::determinant() const
{
    return b3Triple((*this)[0], (*this)[1], (*this)[2]);
}


inline Matrix3x3cpp
Matrix3x3cpp::absolute() const
{
    return Matrix3x3cpp(
            b3Fabs(values[0].getX()), b3Fabs(values[0].getY()), b3Fabs(values[0].getZ()),
            b3Fabs(values[1].getX()), b3Fabs(values[1].getY()), b3Fabs(values[1].getZ()),
            b3Fabs(values[2].getX()), b3Fabs(values[2].getY()), b3Fabs(values[2].getZ()));
}

inline Matrix3x3cpp
Matrix3x3cpp::transpose() const
{
    return Matrix3x3cpp( values[0].getX(), values[1].getX(), values[2].getX(),
                        values[0].getY(), values[1].getY(), values[2].getY(),
                        values[0].getZ(), values[1].getZ(), values[2].getZ());
}

inline Matrix3x3cpp
Matrix3x3cpp::adjoint() const
{
    return Matrix3x3cpp(cofac(1, 1, 2, 2), cofac(0, 2, 2, 1), cofac(0, 1, 1, 2),
        cofac(1, 2, 2, 0), cofac(0, 0, 2, 2), cofac(0, 2, 1, 0),
        cofac(1, 0, 2, 1), cofac(0, 1, 2, 0), cofac(0, 0, 1, 1));
}

inline Matrix3x3cpp
Matrix3x3cpp::inverse() const
{
    b3Vector3 co = b3MakeVector3(cofac(1, 1, 2, 2), cofac(1, 2, 2, 0), cofac(1, 0, 2, 1));
    float det = (*this)[0].dot(co);
    b3FullAssert(det != float(0.0));
    float s = float(1.0) / det;
    return Matrix3x3cpp(co.getX() * s, cofac(0, 2, 2, 1) * s, cofac(0, 1, 1, 2) * s,
        co.getY() * s, cofac(0, 0, 2, 2) * s, cofac(0, 2, 1, 0) * s,
        co.getZ() * s, cofac(0, 1, 2, 0) * s, cofac(0, 0, 1, 1) * s);
}

inline Matrix3x3cpp
Matrix3x3cpp::transposeTimes(const Matrix3x3cpp& m) const
{
    return Matrix3x3cpp(
        values[0].getX() * m[0].getX() + values[1].getX() * m[1].getX() + values[2].getX() * m[2].getX(),
        values[0].getX() * m[0].getY() + values[1].getX() * m[1].getY() + values[2].getX() * m[2].getY(),
        values[0].getX() * m[0].getZ() + values[1].getX() * m[1].getZ() + values[2].getX() * m[2].getZ(),
        values[0].getY() * m[0].getX() + values[1].getY() * m[1].getX() + values[2].getY() * m[2].getX(),
        values[0].getY() * m[0].getY() + values[1].getY() * m[1].getY() + values[2].getY() * m[2].getY(),
        values[0].getY() * m[0].getZ() + values[1].getY() * m[1].getZ() + values[2].getY() * m[2].getZ(),
        values[0].getZ() * m[0].getX() + values[1].getZ() * m[1].getX() + values[2].getZ() * m[2].getX(),
        values[0].getZ() * m[0].getY() + values[1].getZ() * m[1].getY() + values[2].getZ() * m[2].getY(),
        values[0].getZ() * m[0].getZ() + values[1].getZ() * m[1].getZ() + values[2].getZ() * m[2].getZ());
}

inline Matrix3x3cpp
Matrix3x3cpp::timesTranspose(const Matrix3x3cpp& m) const
{
    return Matrix3x3cpp(
        values[0].dot(m[0]), values[0].dot(m[1]), values[0].dot(m[2]),
        values[1].dot(m[0]), values[1].dot(m[1]), values[1].dot(m[2]),
        values[2].dot(m[0]), values[2].dot(m[1]), values[2].dot(m[2]));
}

inline b3Vector3
operator*(const Matrix3x3cpp& m, const b3Vector3& v)
{
    return b3MakeVector3(m[0].dot(v), m[1].dot(v), m[2].dot(v));
}


inline b3Vector3
operator*(const b3Vector3& v, const Matrix3x3cpp& m)
{
    return b3MakeVector3(m.tdotx(v), m.tdoty(v), m.tdotz(v));
}

inline Matrix3x3cpp
operator*(const Matrix3x3cpp& m1, const Matrix3x3cpp& m2)
{
    return Matrix3x3cpp(
        m2.tdotx( m1[0]), m2.tdoty( m1[0]), m2.tdotz( m1[0]),
        m2.tdotx( m1[1]), m2.tdoty( m1[1]), m2.tdotz( m1[1]),
        m2.tdotx( m1[2]), m2.tdoty( m1[2]), m2.tdotz( m1[2]));
}

/*
inline Matrix3x3cpp b3MultTransposeLeft(const Matrix3x3cpp& m1, const Matrix3x3cpp& m2) {
return Matrix3x3cpp(
m1[0][0] * m2[0][0] + m1[1][0] * m2[1][0] + m1[2][0] * m2[2][0],
m1[0][0] * m2[0][1] + m1[1][0] * m2[1][1] + m1[2][0] * m2[2][1],
m1[0][0] * m2[0][2] + m1[1][0] * m2[1][2] + m1[2][0] * m2[2][2],
m1[0][1] * m2[0][0] + m1[1][1] * m2[1][0] + m1[2][1] * m2[2][0],
m1[0][1] * m2[0][1] + m1[1][1] * m2[1][1] + m1[2][1] * m2[2][1],
m1[0][1] * m2[0][2] + m1[1][1] * m2[1][2] + m1[2][1] * m2[2][2],
m1[0][2] * m2[0][0] + m1[1][2] * m2[1][0] + m1[2][2] * m2[2][0],
m1[0][2] * m2[0][1] + m1[1][2] * m2[1][1] + m1[2][2] * m2[2][1],
m1[0][2] * m2[0][2] + m1[1][2] * m2[1][2] + m1[2][2] * m2[2][2]);
}
*/

/**@brief Equality operator between two matrices
* It will test all elements are equal.  */
inline bool operator==(const Matrix3x3cpp& m1, const Matrix3x3cpp& m2)
{
    return
    (   m1[0][0] == m2[0][0] && m1[1][0] == m2[1][0] && m1[2][0] == m2[2][0] &&
        m1[0][1] == m2[0][1] && m1[1][1] == m2[1][1] && m1[2][1] == m2[2][1] &&
        m1[0][2] == m2[0][2] && m1[1][2] == m2[1][2] && m1[2][2] == m2[2][2] );
}



}}} // namespace

#endif // MATRIX3X3CPP_HPP
