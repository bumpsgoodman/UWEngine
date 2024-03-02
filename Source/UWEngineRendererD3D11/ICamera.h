/*
* ICamera 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.02.20
*/

#pragma once

#include <DirectXMath.h>

#include "UWEngineCommon/Defines.h"

using namespace DirectX;

interface ICamera : public IRefObject
{
    virtual bool __stdcall Initailize() = 0;
    virtual void __stdcall Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ) = 0;

    virtual void __stdcall MoveX(const float dist) = 0;
    virtual void __stdcall MoveY(const float dist) = 0;
    virtual void __stdcall MoveZ(const float dist) = 0;

    virtual void __stdcall RotatePitch(const float angleRad) = 0;
    virtual void __stdcall RotateYaw(const float angleRad) = 0;
    virtual void __stdcall RotateRoll(const float angleRad) = 0;

    virtual XMMATRIX __vectorcall GetView() const = 0;
    virtual XMMATRIX __vectorcall GetProjection() const = 0;
};