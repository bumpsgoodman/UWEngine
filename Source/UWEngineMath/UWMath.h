/*
* UWEngine Math
*
* 작성자: bumpsgoodman
* 작성일: 2023.12.22
*/

#pragma once

// 테스트용
#include <DirectXMath.h>
using namespace DirectX;

#include <intrin.h>

#include "UWEngineCommon/Defines.h"
#include "UWMathDefines.h"
#include "UWMathMisc.h"

ALIGN16 union ColorF
{
    ALIGN16 struct
    {
        float Red;
        float Green;
        float Blue;
        float Alpha;
    };
    float RGBA[4];
};

ALIGN16 union Vector4
{
    ALIGN16 struct
    {
        float X;
        float Y;
        float Z;
        float W;
    };
    ALIGN16 float XYZW[4];
    __m128 MM_XYZW;

    inline Vector4() = default;
    inline Vector4(const float x, const float y, const float z, const float w);
    inline Vector4(const float* pXYZW);

    inline Vector4 __vectorcall operator+(const Vector4 v) const;
    inline Vector4 __vectorcall operator-(const Vector4 v) const;
    inline Vector4 __vectorcall operator*(const Vector4 v) const;
    inline Vector4 __vectorcall operator/(const Vector4 v) const;

    inline Vector4 __vectorcall operator+(const float scalar) const;
    inline Vector4 __vectorcall operator-(const float scalar) const;
    inline Vector4 __vectorcall operator*(const float scalar) const;
    inline Vector4 __vectorcall operator/(const float scalar) const;

    inline Vector4& __vectorcall operator+=(const Vector4 v);
    inline Vector4& __vectorcall operator-=(const Vector4 v);
    inline Vector4& __vectorcall operator/=(const Vector4 v);
    inline Vector4& __vectorcall operator*=(const Vector4 v);

    inline Vector4& __vectorcall operator+=(const float scalar);
    inline Vector4& __vectorcall operator-=(const float scalar);
    inline Vector4& __vectorcall operator/=(const float scalar);
    inline Vector4& __vectorcall operator*=(const float scalar);
};

ALIGN16 union Matrix44
{
    struct
    {
        float M00, M01, M02, M03;
        float M10, M11, M12, M13;
        float M20, M21, M22, M23;
        float M30, M31, M32, M33;
    };
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
    ALIGN16 float M[4][4];


    Matrix44() = default;
    Matrix44(const float r00, const float r01, const float r02, const float r03,
             const float r10, const float r11, const float r12, const float r13,
             const float r20, const float r21, const float r22, const float r23,
             const float r30, const float r31, const float r32, const float r33);
    Matrix44(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3);
    Matrix44(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3);

    inline Matrix44 __vectorcall operator+(const Matrix44 mat) const;
    inline Matrix44 __vectorcall operator-(const Matrix44 mat) const;
    inline Matrix44 __vectorcall operator*(const Matrix44 mat) const;

    inline Matrix44& __vectorcall operator+=(const Matrix44 mat);
    inline Matrix44& __vectorcall operator-=(const Matrix44 mat);
    inline Matrix44& __vectorcall operator*=(const Matrix44 mat);
};

// Vector4
static const Vector4 s_zeroVector4 = { 0.0f, 0.0f, 0.0f, 0.0f };
static const Vector4 s_oneVector4 = { 1.0f, 1.0f, 1.0f, 1.0f };

// Matrix44
static const float s_identity44R0[4] = { 1.0f, 0.0f, 0.0f, 0.0f };
static const float s_identity44R1[4] = { 0.0f, 1.0f, 0.0f, 0.0f };
static const float s_identity44R2[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
static const float s_identity44R3[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
static const Matrix44 s_identity44 = { s_identity44R0, s_identity44R1, s_identity44R2, s_identity44R3 };

// -------------------------------------------------------------------------------------
// Vector4

inline Vector4::Vector4(const float x, const float y, const float z, const float w)
    : MM_XYZW(_mm_set_ps(w, z, y, x))
{
}

inline Vector4::Vector4(const float* pXYZW)
    : MM_XYZW(_mm_load_ps(pXYZW))
{
}

inline Vector4 __vectorcall Vector4::operator+(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_add_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

inline Vector4 __vectorcall Vector4::operator-(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_sub_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

inline Vector4 __vectorcall Vector4::operator*(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_mul_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

inline Vector4 __vectorcall Vector4::operator/(const Vector4 v) const
{
    Vector4 result;
    result.MM_XYZW = _mm_div_ps(MM_XYZW, v.MM_XYZW);
    return result;
}

inline Vector4 __vectorcall Vector4::operator+(const float scalar) const
{
    const __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_add_ps(MM_XYZW, temp);
    return result;
}

inline Vector4 __vectorcall Vector4::operator-(const float scalar) const
{
    const __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_sub_ps(MM_XYZW, temp);
    return result;
}

inline Vector4 __vectorcall Vector4::operator*(const float scalar) const
{
    const __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_mul_ps(MM_XYZW, temp);
    return result;
}

inline Vector4 __vectorcall Vector4::operator/(const float scalar) const
{
    const __m128 temp = _mm_set_ps1(scalar);
    Vector4 result;
    result.MM_XYZW = _mm_div_ps(MM_XYZW, temp);
    return result;
}

inline Vector4& __vectorcall Vector4::operator+=(const Vector4 v)
{
    MM_XYZW = _mm_add_ps(MM_XYZW, v.MM_XYZW);
    return *this;
}

inline Vector4& __vectorcall Vector4::operator-=(const Vector4 v)
{
    MM_XYZW = _mm_sub_ps(MM_XYZW, v.MM_XYZW);
    return *this;
}

inline Vector4& __vectorcall Vector4::operator/=(const Vector4 v)
{
    MM_XYZW = _mm_div_ps(MM_XYZW, v.MM_XYZW);
    return *this;
}

inline Vector4& __vectorcall Vector4::operator*=(const Vector4 v)
{
    MM_XYZW = _mm_mul_ps(MM_XYZW, v.MM_XYZW);
    return *this;
}

inline Vector4& __vectorcall Vector4::operator+=(const float scalar)
{
    const __m128 temp = _mm_set_ps1(scalar);
    MM_XYZW = _mm_add_ps(MM_XYZW, temp);
    return *this;
}

inline Vector4& __vectorcall Vector4::operator-=(const float scalar)
{
    const __m128 temp = _mm_set_ps1(scalar);
    MM_XYZW = _mm_sub_ps(MM_XYZW, temp);
    return *this;
}

inline Vector4& __vectorcall Vector4::operator/=(const float scalar)
{
    const __m128 temp = _mm_set_ps1(scalar);
    MM_XYZW = _mm_div_ps(MM_XYZW, temp);
    return *this;
}

inline Vector4& __vectorcall Vector4::operator*=(const float scalar)
{
    const __m128 temp = _mm_set_ps1(scalar);
    MM_XYZW = _mm_mul_ps(MM_XYZW, temp);
    return *this;
}

inline Vector4 __vectorcall Vector4Set(const float x, const float y, const float z, const float w)
{
    Vector4 result;
    result.MM_XYZW = _mm_set_ps(w, z, y, x);
    return result;
}

inline Vector4 __vectorcall Vector4Set(const float* pXYZW)
{
    Vector4 result;
    result.MM_XYZW = _mm_load_ps(pXYZW);
    return result;
}

inline Vector4 __vectorcall Vector4Set(const __m128 xyzw)
{
    Vector4 result;
    result.MM_XYZW = xyzw;
    return result;
}

inline Vector4 __vectorcall Vector4Zero()
{
    return s_zeroVector4;
}

inline Vector4 __vectorcall Vector4One()
{
    return s_oneVector4;
}

inline float __vectorcall Vector4Length(const Vector4 v)
{
    return _mm_cvtss_f32(_mm_sqrt_ps(_mm_dp_ps(v.MM_XYZW, v.MM_XYZW, 0xff)));
}

inline float __vectorcall Vector4LengthSquared(const Vector4 v)
{
    return _mm_cvtss_f32(_mm_dp_ps(v.MM_XYZW, v.MM_XYZW, 0xff));
}

inline Vector4 __vectorcall Vector4Normalize(const Vector4 v)
{
    Vector4 result;
    result.MM_XYZW = _mm_mul_ps(v.MM_XYZW, _mm_rsqrt_ps(_mm_dp_ps(v.MM_XYZW, v.MM_XYZW, 0xff)));
    return result;
}

inline Vector4 __vectorcall Vector4Negate(const Vector4 v)
{
    Vector4 result;
    result.MM_XYZW = _mm_sub_ps(s_zeroVector4.MM_XYZW, v.MM_XYZW);
    return result;
}

inline float __vectorcall Vector4Dot(const Vector4 v0, const Vector4 v1)
{
    return _mm_cvtss_f32(_mm_dp_ps(v0.MM_XYZW, v1.MM_XYZW, 0xff));
}

inline Vector4 __vectorcall Vector4Cross(const Vector4 v0, const Vector4 v1)
{
    // (v0.y * v1.z - v0.z * v1.y
    //  v0.z * v1.x - v0.x * v1.z
    //  v0.x * v1.y - v0.y * v1.x)

    __m128 shuffleV0 = _mm_shuffle_ps(v0.MM_XYZW, v0.MM_XYZW, _MM_SHUFFLE(3, 0, 2, 1)); // (z, x, y)
    __m128 shuffleV1 = _mm_shuffle_ps(v1.MM_XYZW, v1.MM_XYZW, _MM_SHUFFLE(3, 1, 0, 2)); // (y, z, x)
    const __m128 temp0 = _mm_mul_ps(shuffleV0, shuffleV1);

    shuffleV0 = _mm_shuffle_ps(v0.MM_XYZW, v0.MM_XYZW, _MM_SHUFFLE(3, 1, 0, 2)); // (y, z, x)
    shuffleV1 = _mm_shuffle_ps(v1.MM_XYZW, v1.MM_XYZW, _MM_SHUFFLE(3, 0, 2, 1)); // (z, x, y)
    const __m128 temp1 = _mm_mul_ps(shuffleV0, shuffleV1);

    Vector4 result;
    result.MM_XYZW = _mm_sub_ps(temp0, temp1);
    return result;
}

inline Vector4 __vectorcall Vector4Max(const Vector4 v0, const Vector4 v1)
{
    Vector4 result;
    result.MM_XYZW = _mm_max_ps(v0.MM_XYZW, v1.MM_XYZW);
    return result;
}

inline Vector4 __vectorcall Vector4Min(const Vector4 v0, const Vector4 v1)
{
    Vector4 result;
    result.MM_XYZW = _mm_min_ps(v0.MM_XYZW, v1.MM_XYZW);
    return result;
}

inline Vector4 __vectorcall Vector4Round(const Vector4 v)
{
    Vector4 result;
    result.MM_XYZW = _mm_round_ps(v.MM_XYZW, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC);
    return result;
}

inline Vector4 __vectorcall Vector4Floor(const Vector4 v)
{
    Vector4 result;
    result.MM_XYZW = _mm_round_ps(v.MM_XYZW, _MM_FROUND_FLOOR);
    return result;
}

inline Vector4 __vectorcall Vector4Ceil(const Vector4 v)
{
    Vector4 result;
    result.MM_XYZW = _mm_round_ps(v.MM_XYZW, _MM_FROUND_CEIL);
    return result;
}

inline Vector4 __vectorcall Vector4Trunc(const Vector4 v)
{
    Vector4 result;
    result.MM_XYZW = _mm_trunc_ps(v.MM_XYZW);
    return result;
}

inline Vector4 __vectorcall Vector4Mod(const Vector4 v0, const Vector4 v1)
{
    // v0 % v1 = v0 - v1 * TRUNC(v0 / v1);
    Vector4 result;
    result.MM_XYZW = _mm_trunc_ps(_mm_div_ps(v0.MM_XYZW, v1.MM_XYZW));
    result.MM_XYZW = _mm_sub_ps(v0.MM_XYZW, _mm_mul_ps(v1.MM_XYZW, result.MM_XYZW));
    return result;
}

inline Vector4 __vectorcall Vector4Wrap(const Vector4 v, const Vector4 minV, const Vector4 maxV)
{
    /*
    if (v > maxV)
    {
        return minV + mod(v - maxV, maxV);
    }
    else if (v < minV)
    {
        return maxV + mod(minV + v, maxV);
    }
    */

    // v가 최대 값보다 클 때
    Vector4 maxMask;
    Vector4 maxResult;
    maxMask.MM_XYZW = _mm_cmpgt_ps(v.MM_XYZW, maxV.MM_XYZW);
    maxResult.MM_XYZW = _mm_and_ps(v.MM_XYZW, maxMask.MM_XYZW);
    maxResult.MM_XYZW = _mm_sub_ps(maxResult.MM_XYZW, maxV.MM_XYZW);
    maxResult = Vector4Mod(maxResult, maxV);
    maxResult.MM_XYZW = _mm_add_ps(minV.MM_XYZW, maxResult.MM_XYZW);
    maxResult.MM_XYZW = _mm_and_ps(maxResult.MM_XYZW, maxMask.MM_XYZW);

    // v가 최소 값보다 작을 때
    Vector4 minMask;
    Vector4 minResult;
    minMask.MM_XYZW = _mm_cmplt_ps(v.MM_XYZW, minV.MM_XYZW);
    minResult.MM_XYZW = _mm_and_ps(v.MM_XYZW, minMask.MM_XYZW);
    minResult.MM_XYZW = _mm_add_ps(minResult.MM_XYZW, minV.MM_XYZW);
    minResult = Vector4Mod(minResult, maxV);
    minResult.MM_XYZW = _mm_add_ps(maxV.MM_XYZW, minResult.MM_XYZW);
    minResult.MM_XYZW = _mm_and_ps(minResult.MM_XYZW, minMask.MM_XYZW);

    // 래핑되지 않은 값 추가 (범위 내에 있던 값 추가)
    Vector4 result;
    Vector4 zeroMask;
    result.MM_XYZW = _mm_or_ps(minResult.MM_XYZW, maxResult.MM_XYZW);
    zeroMask.MM_XYZW = _mm_cmpeq_ps(result.MM_XYZW, Vector4Zero().MM_XYZW);
    result.MM_XYZW = _mm_or_ps(result.MM_XYZW, _mm_and_ps(v.MM_XYZW, zeroMask.MM_XYZW));

    // and를 하고 나면 순서가 바뀌기 때문에 역순으로 바꾸기
    result.MM_XYZW = _mm_shuffle_ps(result.MM_XYZW, result.MM_XYZW, _MM_SHUFFLE(3, 2, 1, 0));
    return result;
}

inline Vector4 __stdcall Vector4DegreeToRad(const Vector4 v)
{
    static const __m128 MM_PI_DIV_180 = _mm_set_ps1((float)PI_DIV_180);
    Vector4 result;
    result.MM_XYZW = _mm_mul_ps(v.MM_XYZW, MM_PI_DIV_180);
    return result;
}

// -------------------------------------------------------------------------------------
// Matrix44

inline Matrix44::Matrix44(const float r00, const float r01, const float r02, const float r03,
                          const float r10, const float r11, const float r12, const float r13,
                          const float r20, const float r21, const float r22, const float r23,
                          const float r30, const float r31, const float r32, const float r33)
    : MM_R0(_mm_setr_ps(r00, r01, r02, r03))
    , MM_R1(_mm_setr_ps(r10, r11, r12, r13))
    , MM_R2(_mm_setr_ps(r20, r21, r22, r23))
    , MM_R3(_mm_setr_ps(r30, r31, r32, r33))
{
}

inline Matrix44::Matrix44(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3)
    : MM_R0(r0.MM_XYZW)
    , MM_R1(r1.MM_XYZW)
    , MM_R2(r2.MM_XYZW)
    , MM_R3(r3.MM_XYZW)
{
}

inline Matrix44::Matrix44(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3)
    : MM_R0(r0)
    , MM_R1(r1)
    , MM_R2(r2)
    , MM_R3(r3)
{
}

inline Matrix44 __vectorcall Matrix44::operator+(const Matrix44 mat) const
{
    Matrix44 result;
    result.MM_R0 = _mm_add_ps(MM_R0, mat.MM_R0);
    result.MM_R1 = _mm_add_ps(MM_R1, mat.MM_R1);
    result.MM_R2 = _mm_add_ps(MM_R2, mat.MM_R2);
    result.MM_R3 = _mm_add_ps(MM_R3, mat.MM_R3);
    return result;
}

inline Matrix44 __vectorcall Matrix44::operator-(const Matrix44 mat) const
{
    Matrix44 result;
    result.MM_R0 = _mm_sub_ps(MM_R0, mat.MM_R0);
    result.MM_R1 = _mm_sub_ps(MM_R1, mat.MM_R1);
    result.MM_R2 = _mm_sub_ps(MM_R2, mat.MM_R2);
    result.MM_R3 = _mm_sub_ps(MM_R3, mat.MM_R3);
    return result;
}

inline Matrix44 __vectorcall Matrix44::operator*(const Matrix44 mat) const
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

inline Matrix44& __vectorcall Matrix44::operator+=(const Matrix44 mat)
{
    MM_R0 = _mm_add_ps(MM_R0, mat.MM_R0);
    MM_R1 = _mm_add_ps(MM_R1, mat.MM_R1);
    MM_R2 = _mm_add_ps(MM_R2, mat.MM_R2);
    MM_R3 = _mm_add_ps(MM_R3, mat.MM_R3);
    return *this;
}

inline Matrix44& __vectorcall Matrix44::operator-=(const Matrix44 mat)
{
    MM_R0 = _mm_sub_ps(MM_R0, mat.MM_R0);
    MM_R1 = _mm_sub_ps(MM_R1, mat.MM_R1);
    MM_R2 = _mm_sub_ps(MM_R2, mat.MM_R2);
    MM_R3 = _mm_sub_ps(MM_R3, mat.MM_R3);
    return *this;
}

inline Matrix44& __vectorcall Matrix44::operator*=(const Matrix44 mat)
{
    float x;
    float y;
    float z;
    float w;

    x = M[0][0];
    y = M[0][1];
    z = M[0][2];
    w = M[0][3];
    M[0][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    M[0][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    M[0][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    M[0][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    x = M[1][0];
    y = M[1][1];
    z = M[1][2];
    w = M[1][3];
    M[1][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    M[1][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    M[1][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    M[1][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    x = M[2][0];
    y = M[2][1];
    z = M[2][2];
    w = M[2][3];
    M[2][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    M[2][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    M[2][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    M[2][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    x = M[3][0];
    y = M[3][1];
    z = M[3][2];
    w = M[3][3];
    M[3][0] = (x * mat.M[0][0]) + (y * mat.M[1][0]) + (z * mat.M[2][0]) + (w * mat.M[3][0]);
    M[3][1] = (x * mat.M[0][1]) + (y * mat.M[1][1]) + (z * mat.M[2][1]) + (w * mat.M[3][1]);
    M[3][2] = (x * mat.M[0][2]) + (y * mat.M[1][2]) + (z * mat.M[2][2]) + (w * mat.M[3][2]);
    M[3][3] = (x * mat.M[0][3]) + (y * mat.M[1][3]) + (z * mat.M[2][3]) + (w * mat.M[3][3]);

    return *this;
}

inline Vector4 __vectorcall Matrix44IdentityR0()
{
    return s_identity44R0;
}

inline Vector4 __vectorcall Matrix44IdentityR1()
{
    return s_identity44R1;
}

inline Vector4 __vectorcall Matrix44IdentityR2()
{
    return s_identity44R2;
}

inline Vector4 __vectorcall Matrix44IdentityR3()
{
    return s_identity44R3;
}

inline Matrix44 __vectorcall Matrix44Identity()
{
    return s_identity44;
}

inline Matrix44 __vectorcall Matrix44Set(const float r00, const float r01, const float r02, const float r03,
                                         const float r10, const float r11, const float r12, const float r13,
                                         const float r20, const float r21, const float r22, const float r23,
                                         const float r30, const float r31, const float r32, const float r33)
{
    Matrix44 result;
    result.MM_R0 = _mm_setr_ps(r00, r01, r02, r03);
    result.MM_R1 = _mm_setr_ps(r10, r11, r12, r13);
    result.MM_R2 = _mm_setr_ps(r20, r21, r22, r23);
    result.MM_R3 = _mm_setr_ps(r30, r31, r32, r33);
    return result;
}

inline Matrix44 __vectorcall Matrix44Set(const Vector4 r0, const Vector4 r1, const Vector4 r2, const Vector4 r3)
{
    Matrix44 result;
    result.MM_R0 = r0.MM_XYZW;
    result.MM_R1 = r1.MM_XYZW;
    result.MM_R2 = r2.MM_XYZW;
    result.MM_R3 = r3.MM_XYZW;
    return result;
}

inline Matrix44 __vectorcall Matrix44Set(const __m128 r0, const __m128 r1, const __m128 r2, const __m128 r3)
{
    Matrix44 result;
    result.MM_R0 = r0;
    result.MM_R1 = r1;
    result.MM_R2 = r2;
    result.MM_R3 = r3;
    return result;
}

inline Matrix44 __vectorcall Matrix44Transpose(Matrix44 mat)
{
    _MM_TRANSPOSE4_PS(mat.MM_R0, mat.MM_R1, mat.MM_R2, mat.MM_R3);
    return mat;
}

inline Matrix44 __vectorcall Matrix44TranslateX(const float translation)
{
    Matrix44 result = Matrix44Identity();
    result.M20 = translation;
    return result;
}

inline Matrix44 __vectorcall Matrix44TranslateY(const float translation)
{
    Matrix44 result = Matrix44Identity();
    result.M21 = translation;
    return result;
}

inline Matrix44 __vectorcall Matrix44TranslateZ(const float translation)
{
    Matrix44 result = Matrix44Identity();
    result.M22 = translation;
    return result;
}

inline Matrix44 __vectorcall Matrix44TranslateFromVector(const Vector4 translation)
{
    Matrix44 result = Matrix44Identity();
    result.R3 = translation;
    result.M33 = 1.0f;
    return result;
}

inline Matrix44 __vectorcall Matrix44RotatePitch(const float angleRad)
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

inline Matrix44 __vectorcall Matrix44RotateYaw(const float angleRad)
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

inline Matrix44 __vectorcall Matrix44RotateRoll(const float angleRad)
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

// x: Pitch
// y: Yaw
// z: Roll
inline Matrix44 __vectorcall Matrix44RotateRollPitchYawFromVector(const Vector4 angleRad)
{
    float sr;
    float cr;
    GetSinCos(angleRad.Z, &sr, &cr);

    float sp;
    float cp;
    GetSinCos(angleRad.X, &sp, &cp);

    float sy;
    float cy;
    GetSinCos(angleRad.Y, &sy, &cy);

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

inline Matrix44 __vectorcall Matrix44ScaleX(const float scale)
{
    Matrix44 result;
    result.R0 = Vector4Set(scale, 0.0f, 0.0f, 0.0f);
    result.R1 = s_identity44R1;
    result.R2 = s_identity44R2;
    result.R3 = s_identity44R3;
    return result;
}

inline Matrix44 __vectorcall Matrix44ScaleY(const float scale)
{
    Matrix44 result;
    result.R0 = s_identity44R0;
    result.R1 = Vector4Set(0.0f, scale, 0.0f, 0.0f);
    result.R2 = s_identity44R2;
    result.R3 = s_identity44R3;
    return result;
}

inline Matrix44 __vectorcall Matrix44ScaleZ(const float scale)
{
    Matrix44 result;
    result.R0 = s_identity44R0;
    result.R1 = s_identity44R1;
    result.R2 = Vector4Set(0.0f, 0.0f, scale, 0.0f);
    result.R3 = s_identity44R3;
    return result;
}

inline Matrix44 __vectorcall Matrix44ScaleFromVector(const Vector4 scale)
{
    Matrix44 result;
    result.R0 = Vector4Set(scale.X, 0.0f, 0.0f, 0.0f);
    result.R1 = Vector4Set(0.0f, scale.Y, 0.0f, 0.0f);
    result.R2 = Vector4Set(0.0f, 0.0f, scale.Z, 0.0f);
    result.R3 = s_identity44R3;
    return result;
}

inline Matrix44 __vectorcall Matrix44WorldFromVector(const Vector4 translation, const Vector4 angleRad, const Vector4 scale)
{
    const Matrix44 translationMat = Matrix44TranslateFromVector(translation);
    const Matrix44 rotationMat = Matrix44RotateRollPitchYawFromVector(angleRad);
    const Matrix44 scaleMat = Matrix44ScaleFromVector(scale);

    const Matrix44 result = scaleMat * rotationMat * translationMat;
    return result;
}






// 테스트
inline XMVECTOR __vectorcall Vector4ToXMVector(const Vector4 v)
{
    FXMVECTOR result = XMVectorSet(v.X, v.Y, v.Z, v.W);
    return result;
}

inline Vector4 __vectorcall XMVectorToVector4(FXMVECTOR v)
{
    const Vector4 result = Vector4Set(v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
    return result;
}

inline XMMATRIX __vectorcall Matrix44ToXMMatrix(const Matrix44 mat)
{
    FXMMATRIX result = XMMatrixSet(mat.M00, mat.M01, mat.M02, mat.M03,
                                   mat.M10, mat.M11, mat.M12, mat.M13,
                                   mat.M20, mat.M21, mat.M22, mat.M23,
                                   mat.M30, mat.M31, mat.M32, mat.M33);
    return result;
}

inline Matrix44 __vectorcall XMMatrixToMatrix44(FXMMATRIX mat)
{
    const Matrix44 result = Matrix44Set(mat.r[0], mat.r[1], mat.r[2], mat.r[3]);
    return result;
}