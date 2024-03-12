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
    virtual bool __stdcall Initailize() = 0;
    virtual void __stdcall Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ) = 0;

    virtual void __stdcall Translate(const Vector4 dist) = 0;
    virtual void __stdcall TranslateX(const float dist) = 0;
    virtual void __stdcall TranslateY(const float dist) = 0;
    virtual void __stdcall TranslateZ(const float dist) = 0;

    virtual void __stdcall Rotate(const Vector4 angleDegrees) = 0;
    virtual void __stdcall RotateX(const float angleDegree) = 0;
    virtual void __stdcall RotateY(const float angleDegree) = 0;
    virtual void __stdcall RotateZ(const float angleDegree) = 0;

    virtual void __stdcall SetRotation(const Vector4 angleDegrees) = 0;
    virtual Vector4 __stdcall GetRotation() const = 0;

    virtual void __vectorcall SetEye(const Vector4 v) = 0;
    virtual void __vectorcall SetAt(const Vector4 v) = 0;
    virtual void __vectorcall SetUp(const Vector4 v) = 0;

    virtual Vector4 __vectorcall GetEye() const = 0;
    virtual Vector4 __vectorcall GetAt() const = 0;
    virtual Vector4 __vectorcall GetUp() const = 0;

    virtual Matrix44 __vectorcall GetView() const = 0;
    virtual Matrix44 __vectorcall GetProjection() const = 0;
};