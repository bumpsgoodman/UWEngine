/*
* UWEngine 공용 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.06
*/

#pragma once

#include <Windows.h>

#include "Defines.h"
#include "UWEngineMath/UWMath.h"

interface IRendererD3D11_2D;

interface IRefObject
{
    virtual UWMETHOD(vsize) AddRef() PURE;
    virtual UWMETHOD(vsize) Release() PURE;
    virtual UWMETHOD(vsize) GetRefCount() const PURE;
};

interface ICamera2D : public IRefObject
{
    virtual UWMETHOD(bool)              Initailize() PURE;
    virtual UWMETHOD(void)              Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ) PURE;

    virtual UWMETHOD_VECTOR(void)       Translate(const Vector4 dist) PURE;
    virtual UWMETHOD(void)              TranslateX(const float dist) PURE;
    virtual UWMETHOD(void)              TranslateY(const float dist) PURE;

    virtual UWMETHOD_VECTOR(void)       Rotate(const Vector4 angleDegrees) PURE;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) PURE;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) PURE;

    virtual UWMETHOD_VECTOR(Vector4)    GetRotation() const PURE;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const Vector4 angleDegrees) PURE;

    virtual UWMETHOD_VECTOR(Vector4)    GetEye() const PURE;
    virtual UWMETHOD_VECTOR(Vector4)    GetAt() const PURE;
    virtual UWMETHOD_VECTOR(Vector4)    GetUp() const PURE;

    virtual UWMETHOD_VECTOR(void)       SetEye(const Vector4 v) PURE;
    virtual UWMETHOD_VECTOR(void)       SetAt(const Vector4 v) PURE;
    virtual UWMETHOD_VECTOR(void)       SetUp(const Vector4 v) PURE;

    virtual UWMETHOD_VECTOR(Matrix44)   GetView() const PURE;
    virtual UWMETHOD_VECTOR(Matrix44)   GetProjection() const PURE;
};

enum MESH_RENDER_TYPE
{
    MESH_RENDER_TYPE_WIREFRAME,
    MESH_RENDER_TYPE_COLOR,
    MESH_RENDER_TYPE_TEXTURE,

    NUM_MESH_RENDER_TYPE
};

interface IMeshObject2D : public IRefObject
{
    virtual UWMETHOD(bool)              Initialize(IRendererD3D11_2D* pRenderer) PURE;

    virtual UWMETHOD(bool)              CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                                                   const uint16* pIndices, const uint numIndices,
                                                   const ColorF wireframeColor,
                                                   const wchar_t* pShaderFileName) PURE;
    virtual UWMETHOD(void)              RenderMesh() PURE;

    virtual UWMETHOD_VECTOR(void)       Translate(const Vector4 dist) PURE;
    virtual UWMETHOD(void)              TranslateX(const float dist) PURE;
    virtual UWMETHOD(void)              TranslateY(const float dist) PURE;

    virtual UWMETHOD_VECTOR(void)       Rotate(const Vector4 angleDegrees) PURE;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) PURE;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) PURE;

    virtual UWMETHOD_VECTOR(void)       Scale(const Vector4 scale) PURE;
    virtual UWMETHOD(void)              ScaleX(const float scale) PURE;
    virtual UWMETHOD(void)              ScaleY(const float scale) PURE;

    virtual UWMETHOD_VECTOR(Vector4)    GetPosition() const PURE;
    virtual UWMETHOD_VECTOR(Vector4)    GetRotation() const PURE;
    virtual UWMETHOD_VECTOR(Vector4)    GetScale() const PURE;

    virtual UWMETHOD_VECTOR(void)       SetPosition(const Vector4 position) PURE;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const Vector4 angleDegrees) PURE;
    virtual UWMETHOD_VECTOR(void)       SetScale(const Vector4 scale) PURE;

    virtual UWMETHOD(void)              SetCamera(ICamera2D* pCamera) PURE;

    virtual UWMETHOD(MESH_RENDER_TYPE)  GetRenderType() const PURE;
    virtual UWMETHOD(void)              SetRenderType(const MESH_RENDER_TYPE type) PURE;
};

interface IRendererD3D11_2D : public IRefObject
{
    virtual UWMETHOD(bool)      Initialize(const HWND hWnd) PURE;

    virtual UWMETHOD(void)      BeginRender() PURE;
    virtual UWMETHOD(void)      EndRender() PURE;

    virtual UWMETHOD(float)     GetDeltaTime() const PURE;
    virtual UWMETHOD(uint)      GetFPS() const PURE;

    virtual UWMETHOD(void)      CreateCamera(ICamera2D** ppOutCamera) PURE;
    virtual UWMETHOD(void)      CreateMeshObject(IMeshObject2D** ppOutMeshObject) PURE;

    virtual UWMETHOD(void*)     Private_GetD3dDevice() const PURE;
    virtual UWMETHOD(void*)     Private_GetImmediateContext() const PURE;
};