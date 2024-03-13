/*
* MeshObject Interface
*
* 작성자: bumpsgoodman
* 작성일: 2023.02.03
*/

#pragma once

#include "UWEngineCommon/Defines.h"
#include "UWEngineMath/UWMath.h"
#include "ICamera.h"

interface IRendererD3D11;

interface IMeshObject : public IRefObject
{
public:
    virtual bool __stdcall Initialize(IRendererD3D11* pRenderer) = 0;
    virtual bool __stdcall CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                                      const uint16* pIndices, const uint numIndices,
                                      const wchar_t* pShaderFileName) = 0;
    virtual void __stdcall RenderMesh() = 0;

    virtual void __stdcall Translate(const Vector4 dist) = 0;
    virtual void __stdcall TranslateX(const float dist) = 0;
    virtual void __stdcall TranslateY(const float dist) = 0;
    virtual void __stdcall TranslateZ(const float dist) = 0;

    virtual void __stdcall Rotate(const Vector4 angleDegrees) = 0;
    virtual void __stdcall RotateX(const float angleDegree) = 0;
    virtual void __stdcall RotateY(const float angleDegree) = 0;
    virtual void __stdcall RotateZ(const float angleDegree) = 0;

    virtual void __stdcall Scale(const Vector4 scale) = 0;
    virtual void __stdcall ScaleX(const float scale) = 0;
    virtual void __stdcall ScaleY(const float scale) = 0;
    virtual void __stdcall ScaleZ(const float scale) = 0;

    virtual void __stdcall SetPosition(const Vector4 position) = 0;
    virtual void __stdcall SetRotation(const Vector4 angleDegrees) = 0;
    virtual void __stdcall SetScale(const Vector4 scale) = 0;

    virtual Vector4 __stdcall GetPosition() const = 0;
    virtual Vector4 __stdcall GetRotation() const = 0;
    virtual Vector4 __stdcall GetScale() const = 0;

    virtual void __stdcall SetCamera(ICamera* pCamera) = 0;
};