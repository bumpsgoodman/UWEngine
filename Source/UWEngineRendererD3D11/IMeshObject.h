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

enum MESH_RENDER_TYPE
{
    MESH_RENDER_TYPE_WIREFRAME,
    MESH_RENDER_TYPE_COLOR,
    MESH_RENDER_TYPE_TEXTURE
};

interface IMeshObject : public IRefObject
{
public:
    UWMETHOD(bool) Initialize(IRendererD3D11* pRenderer) PURE;
    UWMETHOD(bool) CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                              const uint16* pIndices, const uint numIndices,
                              const wchar_t* pShaderFileName) PURE;
    UWMETHOD(void) RenderMesh() PURE;

    UWMETHOD(void) Translate(const Vector4 dist) PURE;
    UWMETHOD(void) TranslateX(const float dist) PURE;
    UWMETHOD(void) TranslateY(const float dist) PURE;
    UWMETHOD(void) TranslateZ(const float dist) PURE;

    UWMETHOD(void) Rotate(const Vector4 angleDegrees) PURE;
    UWMETHOD(void) RotateX(const float angleDegree) PURE;
    UWMETHOD(void) RotateY(const float angleDegree) PURE;
    UWMETHOD(void) RotateZ(const float angleDegree) PURE;

    UWMETHOD(void) Scale(const Vector4 scale) PURE;
    UWMETHOD(void) ScaleX(const float scale) PURE;
    UWMETHOD(void) ScaleY(const float scale) PURE;
    UWMETHOD(void) ScaleZ(const float scale) PURE;

    UWMETHOD(void) SetPosition(const Vector4 position) PURE;
    UWMETHOD(void) SetRotation(const Vector4 angleDegrees) PURE;
    UWMETHOD(void) SetScale(const Vector4 scale) PURE;

    UWMETHOD(Vector4) GetPosition() const PURE;
    UWMETHOD(Vector4) GetRotation() const PURE;
    UWMETHOD(Vector4) GetScale() const PURE;

    UWMETHOD(void) SetCamera(ICamera* pCamera) PURE;
};