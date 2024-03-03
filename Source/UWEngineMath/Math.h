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
    __m128 MM_RGBA;
};

ALIGN16 union Vector4
{
    struct
    {
        float X;
        float Y;
        float Z;
        float W;
    };
    float XYZW[4];
    __m128 MM_XYZW;

    Vector4() = default;
    Vector4(const float x, const float y, const float z, const float w);
    Vector4(const float* pXYZW);

    void __vectorcall Set(const float x, const float y, const float z, const float w);
    void __vectorcall Set(const float* pXYZW);
    void __vectorcall Set(const __m128 xyzw);

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

ALIGN16 union Matrix44
{
    struct
    {
        Vector4 R0;
        Vector4 R1;
        Vector4 R2;
        Vector4 R3;
    };
    struct
    {
        __m128 MM_R0;
        __m128 MM_R1;
        __m128 MM_R2;
        __m128 MM_R3;
    };
    __m128 MM_ROWS[4];
    Vector4 ROWS[4];
    float M[4][4];

    Matrix44() = default;
    Matrix44(const float r00, const float r01, const float r02, const float r03,
             const float r10, const float r11, const float r12, const float r13,
             const float r20, const float r21, const float r22, const float r23,
             const float r30, const float r31, const float r32, const float r33);
    Matrix44(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3);
    Matrix44(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3);

    void __vectorcall Set(const float r00, const float r01, const float r02, const float r03,
                          const float r10, const float r11, const float r12, const float r13,
                          const float r20, const float r21, const float r22, const float r23,
                          const float r30, const float r31, const float r32, const float r33);
    void __vectorcall Set(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3);
    void __vectorcall Set(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3);

    Matrix44 __vectorcall operator+(const Matrix44 mat);
    Matrix44 __vectorcall operator-(const Matrix44 mat);
    Matrix44 __vectorcall operator*(const Matrix44 mat);
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
    : MM_XYZW(_mm_set_ps(w, z, y, x))
{
}

Vector4::Vector4(const float* pXYZW)
    : MM_XYZW(_mm_load_ps(pXYZW))
{
}

void __vectorcall Vector4::Set(const float x, const float y, const float z, const float w)
{
    MM_XYZW = _mm_set_ps(w, z, y, x);
}

void __vectorcall Vector4::Set(const float* pXYZW)
{
    MM_XYZW = _mm_load_ps(pXYZW);
}

void __vectorcall Vector4::Set(const __m128 xyzw)
{
    MM_XYZW = xyzw;
}

Vector4 __vectorcall Vector4::operator+(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_add_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator-(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_sub_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator*(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_mul_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator/(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_div_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

Vector4 __vectorcall Vector4::operator+(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_add_ps(MM_XYZW, temp);
    return result;
}

Vector4 __vectorcall Vector4::operator-(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_sub_ps(MM_XYZW, temp);
    return result;
}

Vector4 __vectorcall Vector4::operator*(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_mul_ps(MM_XYZW, temp);
    return result;
}

Vector4 __vectorcall Vector4::operator/(const float scalar) const
{
    __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_div_ps(MM_XYZW, temp);
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
    : MM_R0(_mm_setr_ps(r00, r01, r02, r03))
    , MM_R1(_mm_setr_ps(r10, r11, r12, r13))
    , MM_R2(_mm_setr_ps(r20, r21, r22, r23))
    , MM_R3(_mm_setr_ps(r30, r31, r32, r33))
{
}

Matrix44::Matrix44(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3)
    : MM_R0(r0.MM_XYZW)
    , MM_R1(r1.MM_XYZW)
    , MM_R2(r2.MM_XYZW)
    , MM_R3(r3.MM_XYZW)
{
}

Matrix44::Matrix44(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3)
    : MM_R0(r0)
    , MM_R1(r1)
    , MM_R2(r2)
    , MM_R3(r3)
{
}

void __vectorcall Matrix44::Set(const float r00, const float r01, const float r02, const float r03,
                                const float r10, const float r11, const float r12, const float r13,
                                const float r20, const float r21, const float r22, const float r23,
                                const float r30, const float r31, const float r32, const float r33)
{
    MM_R0 =_mm_setr_ps(r00, r01, r02, r03);
    MM_R1 =_mm_setr_ps(r10, r11, r12, r13);
    MM_R2 =_mm_setr_ps(r20, r21, r22, r23);
    MM_R3 =_mm_setr_ps(r30, r31, r32, r33);
}

void __vectorcall Matrix44::Set(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3)
{
    MM_R0 = r0.MM_XYZW;
    MM_R1 = r1.MM_XYZW;
    MM_R2 = r2.MM_XYZW;
    MM_R3 = r3.MM_XYZW;
}

void __vectorcall Matrix44::Set(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3)
{
    MM_R0 = r0;
    MM_R1 = r1;
    MM_R2 = r2;
    MM_R3 = r3;
}

Matrix44 __vectorcall Matrix44::operator+(const Matrix44 mat)
{
    Matrix44 result;
    result.MM_R0 = _mm_add_ps(MM_R0, mat.MM_R0);
    result.MM_R1 = _mm_add_ps(MM_R1, mat.MM_R1);
    result.MM_R2 = _mm_add_ps(MM_R2, mat.MM_R2);
    result.MM_R3 = _mm_add_ps(MM_R3, mat.MM_R3);
    return result;
}

Matrix44 __vectorcall Matrix44::operator-(const Matrix44 mat)
{
    Matrix44 result;
    result.MM_R0 = _mm_sub_ps(MM_R0, mat.MM_R0);
    result.MM_R1 = _mm_sub_ps(MM_R1, mat.MM_R1);
    result.MM_R2 = _mm_sub_ps(MM_R2, mat.MM_R2);
    result.MM_R3 = _mm_sub_ps(MM_R3, mat.MM_R3);
    return result;
}

Matrix44 __vectorcall Matrix44::operator*(const Matrix44 mat)
{
    Matrix44 result;

    float x;
    float y;
    float z;
    float w;

    x = M[0][0];
    y = M[0][1];
    z = M[0][2];
    w = M[0][3];
    result.M[0][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    result.M[0][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    result.M[0][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    result.M[0][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    x = M[1][0];
    y = M[1][1];
    z = M[1][2];
    w = M[1][3];
    result.M[1][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    result.M[1][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    result.M[1][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    result.M[1][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    x = M[2][0];
    y = M[2][1];
    z = M[2][2];
    w = M[2][3];
    result.M[2][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    result.M[2][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    result.M[2][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    result.M[2][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    x = M[3][0];
    y = M[3][1];
    z = M[3][2];
    w = M[3][3];
    result.M[3][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    result.M[3][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    result.M[3][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    result.M[3][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    return result;
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

Matrix44 __vectorcall RotatePitch(const float angleRad)
{
    float sin;
    float cos;
    GetSinCos(angleRad, &sin, &cos);

    Matrix44 result(1.0f, 0.0f, 0.0f, 0.0f,
                    0.0f, cos, sin, 0.0f,
                    0.0f, -sin, cos, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    return result;
}

Matrix44 __vectorcall RotateYaw(const float angleRad)
{
    float sin;
    float cos;
    GetSinCos(angleRad, &sin, &cos);

    Matrix44 result(cos, 0.0f, -sin, 0.0f,
                    0.0f, 1.0f, 0.0f, 0.0f,
                    sin, 0.0f, cos, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    return result;
}

Matrix44 __vectorcall RotateRoll(const float angleRad)
{
    float sin;
    float cos;
    GetSinCos(angleRad, &sin, &cos);

    Matrix44 result(cos, sin, 0.0f, 0.0f,
                    -sin, cos, 0.0f, 0.0f,
                    0.0f, 0.0f, 1.0f, 0.0f,
                    0.0f, 0.0f, 0.0f, 1.0f);
    return result;
}

Matrix44 __vectorcall RotateRollPitchYaw(const float rollRad, const float pitchRad, const float yawRad)
{
    float sr;
    float cr;
    GetSinCos(rollRad, &sr, &cr);

    float sp;
    float cp;
    GetSinCos(pitchRad, &sp, &cp);

    float sy;
    float cy;
    GetSinCos(yawRad, &sy, &cy);

    Matrix44 result;
    result.M[0][0] = cr * cy + sr * sp * sy;
    result.M[0][1] = sr * cp;
    result.M[0][2] = cr * -sy + sr * sp * cy;
    result.M[0][3] = 0.0f;

    result.M[1][0] = -sr * cy + cr * sp * sy;
    result.M[1][1] = cr * cp;
    result.M[1][2] = -sr * -sy + cr * sp * cy;
    result.M[1][3] = 0.0f;

    result.M[2][0] = cp * sy;
    result.M[2][1] = -sp;
    result.M[2][2] = cp * cy;
    result.M[2][3] = 0.0f;

    result.M[3][0] = 0.0f;
    result.M[3][1] = 0.0f;
    result.M[3][2] = 0.0f;
    result.M[3][3] = 1.0f;

    return result;
}