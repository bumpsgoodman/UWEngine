/*
* ICamera 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.02.20
*/

#pragma once

#include "UWEngineCommon/Defines.h"
#include "UWEngineMath/UWMath.h"

interface ICamera : public IRefObject
{
    UWMETHOD(bool) Initailize() PURE;
    UWMETHOD(void) Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ) PURE;

    UWMETHOD(void) Translate(const Vector4 dist) PURE;
    UWMETHOD(void) TranslateX(const float dist) PURE;
    UWMETHOD(void) TranslateY(const float dist) PURE;
    UWMETHOD(void) TranslateZ(const float dist) PURE;

    UWMETHOD(void) Rotate(const Vector4 angleDegrees) PURE;
    UWMETHOD(void) RotateX(const float angleDegree) PURE;
    UWMETHOD(void) RotateY(const float angleDegree) PURE;
    UWMETHOD(void) RotateZ(const float angleDegree) PURE;

    UWMETHOD_VECTOR(void) SetRotation(const Vector4 angleDegrees) PURE;
    UWMETHOD_VECTOR(Vector4) GetRotation() const PURE;

    UWMETHOD_VECTOR(void) SetEye(const Vector4 v) PURE;
    UWMETHOD_VECTOR(void) SetAt(const Vector4 v) PURE;
    UWMETHOD_VECTOR(void) SetUp(const Vector4 v) PURE;

    UWMETHOD_VECTOR(Vector4) GetEye() const PURE;
    UWMETHOD_VECTOR(Vector4) GetAt() const PURE;
    UWMETHOD_VECTOR(Vector4) GetUp() const PURE;

    UWMETHOD_VECTOR(Matrix44) GetView() const PURE;
    UWMETHOD_VECTOR(Matrix44) GetProjection() const PURE;
};