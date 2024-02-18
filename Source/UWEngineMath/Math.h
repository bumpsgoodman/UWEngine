/*
* Math
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.22
*/

#pragma once

#include "UWEngineCommon/Defines.h"
#include "MathDefines.h"
#include "MathMisc.h"

ALIGN16 union ColorF
{
    struct
    {
        float Red;
        float Green;
        float Blue;
        float Alpha;
    };
    float RGBA[4];
    __m128 M_RGBA;
};

ALIGN16 struct Vector4
{
    union
    {
        struct
        {
            float X;
            float Y;
            float Z;
            float W;
        };
        float XYZW[4];
        __m128 M_XYZW;
    };

    Vector4() = default;
    Vector4(const float x, const float y, const float z, const float w);
    Vector4(const float* pXYZW);

    Vector4 __vectorcall operator+(const Vector4 v) const;
    Vector4 __vectorcall operator-(const Vector4 v) const;
    Vector4 __vectorcall operator*(const Vector4 v) const;
    Vector4 __vectorcall operator/(const Vector4 v) const;

    Vector4 __vectorcall operator+(const float scalar) const;
    Vector4 __vectorcall operator-(const float scalar) const;
    Vector4 __vectorcall operator*(const float scalar) const;
    Vector4 __vectorcall operator/(const float scalar) const;

    /*
    Vector4 __vectorcall GetNegate() const;
    float __vectorcall GetLength() const;
    Vector4 __vectorcall GetUnit() const;
    float __vectorcall Dot(const Vector4& v) const;
    float __vectorcall Cross(const Vector4& v) const;
    */
};

ALIGN16 struct Matrix44
{
    __m128 R0;
    __m128 R1;
    __m128 R2;
    __m128 R3;

    Matrix44() = default;
    Matrix44(const float r00, const float r01, const float r02, const float r03,
             const float r10, const float r11, const float r12, const float r13,
             const float r20, const float r21, const float r22, const float r23,
             const float r30, const float r31, const float r32, const float r33);
    Matrix44(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3);
    Matrix44(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3);

    Matrix44 __vectorcall operator*(const Matrix44 m);
};

// Vector4
static const Vector4 s_zero_vector4 = { 0.0f, 0.0f, 0.0f, 0.0f };
static const Vector4 s_one_vector4 = { 1.0f, 1.0f, 1.0f, 1.0f };

// Matrix44
static const Vector4 s_identity44_r0 = { 1.0f, 0.0f, 0.0f, 0.0f };
static const Vector4 s_identity44_r1 = { 0.0f, 1.0f, 0.0f, 0.0f };
static const Vector4 s_identity44_r2 = { 0.0f, 0.0f, 1.0f, 0.0f };
static const Vector4 s_identity44_r3 = { 0.0f, 0.0f, 0.0f, 1.0f };
static const Matrix44 s_identity44 = { s_identity44_r0, s_identity44_r1, s_identity44_r2, s_identity44_r3 };

// -------------------------------------------------------------------------------------
// Vector4

Vector4::Vector4(const float x, const float y, const float z, const float w)
    : M_XYZW(_mm_setr_ps(w, z, y, x))
{
}

Vector4::Vector4(const float* pXYZW)
    : M_XYZW(_mm_load_ps(pXYZW))
{
}

Vector4 __vectorcall Vector4::operator+(const Vector4 v) const
{
    Vector4 result;
    result.M_XYZW = _mm_add_ps(M_XYZW, v.M_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator-(const Vector4 v) const
{
    Vector4 result;
    result.M_XYZW = _mm_sub_ps(M_XYZW, v.M_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator*(const Vector4 v) const
{
    Vector4 result;
    result.M_XYZW = _mm_mul_ps(M_XYZW, v.M_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator/(const Vector4 v) const
{
    Vector4 result;
    result.M_XYZW = _mm_div_ps(M_XYZW, v.M_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator+(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.M_XYZW = _mm_add_ps(M_XYZW, temp);
    return result;
}

Vector4 __vectorcall Vector4::operator-(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.M_XYZW = _mm_sub_ps(M_XYZW, temp);
    return result;
}

Vector4 __vectorcall Vector4::operator*(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.M_XYZW = _mm_mul_ps(M_XYZW, temp);
    return result;
}


Vector4 __vectorcall Vector4::operator/(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.M_XYZW = _mm_div_ps(M_XYZW, temp);
    return result;
}

/*
float Vector4::GetLength() const
{
    float result = sqrt(X * X + Y * Y);
}
*/

Vector4 __vectorcall GetZeroVector4()
{
    return s_zero_vector4;
}

// -------------------------------------------------------------------------------------
// Matrix44

Matrix44::Matrix44(const float r00, const float r01, const float r02, const float r03,
                   const float r10, const float r11, const float r12, const float r13,
                   const float r20, const float r21, const float r22, const float r23,
                   const float r30, const float r31, const float r32, const float r33)
    : R0(_mm_setr_ps(r00, r01, r02, r03))
    , R1(_mm_setr_ps(r10, r11, r12, r13))
    , R2(_mm_setr_ps(r20, r21, r22, r23))
    , R3(_mm_setr_ps(r30, r31, r32, r33))
{
}

Matrix44::Matrix44(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3)
    : R0(r0.M_XYZW)
    , R1(r1.M_XYZW)
    , R2(r2.M_XYZW)
    , R3(r3.M_XYZW)
{
}

Matrix44::Matrix44(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3)
    : R0(r0)
    , R1(r1)
    , R2(r2)
    , R3(r3)
{
}

Matrix44 __vectorcall Matrix44::operator*(const Matrix44 m)
{

}

Vector4 __vectorcall GetIdentity44R0()
{
    return s_identity44_r0;
}

Vector4 __vectorcall GetIdentity44R1()
{
    return s_identity44_r1;
}

Vector4 __vectorcall GetIdentity44R2()
{
    return s_identity44_r2;
}

Vector4 __vectorcall GetIdentity44R3()
{
    return s_identity44_r3;
}

Matrix44 __vectorcall GetIdentityMatrix44()
{
    return s_identity44;
}