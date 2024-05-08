/*
* UWEngine 공용 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.06
*/

#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <DirectXMath.h>
using namespace DirectX;

#include "Defines.h"

interface IRendererD3D11;
interface IRendererD3D11_2D;

interface IRefObject
{
    virtual UWMETHOD(vsize) AddRef() PURE;
    virtual UWMETHOD(vsize) Release() PURE;
    virtual UWMETHOD(vsize) GetRefCount() const PURE;
};

interface ICamera : public IRefObject
{
    virtual UWMETHOD(bool)              Initailize(const float fovY, const float aspectRatio, const float nearZ, const float farZ) PURE;
    virtual UWMETHOD(void)              Update() PURE;

    virtual UWMETHOD_VECTOR(void)       Translate(const XMVECTOR dist) PURE;
    virtual UWMETHOD(void)              TranslateX(const float dist) PURE;
    virtual UWMETHOD(void)              TranslateY(const float dist) PURE;
    virtual UWMETHOD(void)              TranslateZ(const float dist) PURE;

    virtual UWMETHOD_VECTOR(void)       Rotate(const XMVECTOR angleDegrees) PURE;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) PURE;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) PURE;
    virtual UWMETHOD(void)              RotateZ(const float angleDegree) PURE;

    virtual UWMETHOD_VECTOR(XMVECTOR)   GetPosition() const PURE;
    virtual UWMETHOD_VECTOR(void)       SetPosition(const XMVECTOR position) PURE;

    virtual UWMETHOD_VECTOR(XMVECTOR)   GetRotation() const PURE;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const XMVECTOR angleDegrees) PURE;

    virtual UWMETHOD_VECTOR(XMMATRIX)   GetView() const PURE;
    virtual UWMETHOD_VECTOR(XMMATRIX)   GetProjection() const PURE;
};

interface IMeshObject : public IRefObject
{
    virtual UWMETHOD(bool)              Initialize(IRendererD3D11* pRenderer) PURE;

    virtual UWMETHOD(bool)              CreateMesh(const int includeFlag,
                                                   const void* pVertices, const uint vertexSize, const uint numVertices,
                                                   const uint16** ppIndices, const uint16* pNumIndices, const uint numIndexBuffers,
                                                   const wchar_t** ppTextureFileNamesOrNull, const uint numTextures,
                                                   const wchar_t* pShaderFileName) PURE;
    virtual UWMETHOD(void)              RenderMesh() PURE;

    virtual UWMETHOD_VECTOR(void)       Translate(const XMVECTOR dist) PURE;
    virtual UWMETHOD(void)              TranslateX(const float dist) PURE;
    virtual UWMETHOD(void)              TranslateY(const float dist) PURE;
    virtual UWMETHOD(void)              TranslateZ(const float dist) PURE;

    virtual UWMETHOD_VECTOR(void)       Rotate(const XMVECTOR angleDegrees) PURE;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) PURE;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) PURE;
    virtual UWMETHOD(void)              RotateZ(const float angleDegree) PURE;

    virtual UWMETHOD_VECTOR(void)       Scale(const XMVECTOR scale) PURE;
    virtual UWMETHOD(void)              ScaleX(const float scale) PURE;
    virtual UWMETHOD(void)              ScaleY(const float scale) PURE;
    virtual UWMETHOD(void)              ScaleZ(const float scale) PURE;

    virtual UWMETHOD_VECTOR(XMVECTOR)   GetPosition() const PURE;
    virtual UWMETHOD_VECTOR(XMVECTOR)   GetRotation() const PURE;
    virtual UWMETHOD_VECTOR(XMVECTOR)   GetScale() const PURE;

    virtual UWMETHOD_VECTOR(void)       SetPosition(const XMVECTOR position) PURE;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const XMVECTOR angleDegrees) PURE;
    virtual UWMETHOD_VECTOR(void)       SetScale(const XMVECTOR scale) PURE;

    virtual UWMETHOD(void)              SetCamera(ICamera* pCamera) PURE;
};

interface IRendererD3D11 : public IRefObject
{
    virtual UWMETHOD(bool)      Initialize(const HWND hWnd) PURE;

    virtual UWMETHOD(void)      BeginRender() PURE;
    virtual UWMETHOD(void)      EndRender() PURE;

    virtual UWMETHOD(uint)      GetWindowWidth() const PURE;
    virtual UWMETHOD(uint)      GetWindowHeight() const PURE;

    virtual UWMETHOD(float)     GetDeltaTime() const PURE;
    virtual UWMETHOD(uint)      GetFPS() const PURE;

    virtual UWMETHOD(void)      CreateCamera(ICamera** ppOutCamera) PURE;
    virtual UWMETHOD(void)      CreateMeshObject(IMeshObject** ppOutMeshObject) PURE;

    virtual UWMETHOD(bool)      IsWireframeMode() const PURE;
    virtual UWMETHOD(void)      ToggleRenderMode() PURE;

    virtual UWMETHOD(void*)     Private_GetD3dDevice() const PURE;
    virtual UWMETHOD(void*)     Private_GetImmediateContext() const PURE;
};