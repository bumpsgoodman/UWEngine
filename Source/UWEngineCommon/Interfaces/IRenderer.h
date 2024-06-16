/*
* UWEngine 렌더러 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.11
*/

#pragma once

#include "UWEngineCommon/Defines.h"

struct IRenderer;

interface ICamera : public IRefObject
{
    virtual bool        __stdcall       Initailize(const float fovY, const float aspectRatio, const float nearZ, const float farZ) = 0;
    virtual void        __stdcall       Update() = 0;
       
    virtual void        __vectorcall    Translate(const Vector4 dist) = 0;
    virtual void        __stdcall       TranslateX(const float dist) = 0;
    virtual void        __stdcall       TranslateY(const float dist) = 0;
    virtual void        __stdcall       TranslateZ(const float dist) = 0;
          
    virtual void        __vectorcall    Rotate(const Vector4 angleDegrees) = 0;
    virtual void        __stdcall       RotateX(const float angleDegree) = 0;
    virtual void        __stdcall       RotateY(const float angleDegree) = 0;
    virtual void        __stdcall       RotateZ(const float angleDegree) = 0;

    virtual Vector4     __vectorcall    GetPosition() const = 0;
    virtual void        __vectorcall    SetPosition(const Vector4 position) = 0;

    virtual Vector4     __vectorcall    GetRotation() const = 0;
    virtual void        __vectorcall    SetRotation(const Vector4 angleDegrees) = 0;

    virtual Matrix44    __vectorcall    GetView() const = 0;
    virtual Matrix44    __vectorcall    GetProjection() const = 0;
};

interface IMeshObject : public IRefObject
{
    virtual bool        __stdcall       Initialize(IRenderer* pRenderer) = 0;

    virtual bool        __stdcall       CreateMesh(const int includeFlag,
                                                   const void* pVertices, const uint vertexSize, const uint numVertices,
                                                   const uint16** ppIndices, const uint16* pNumIndices, const uint numIndexBuffers,
                                                   const void* pTexCoordsOrNull, const wchar_t** ppTextureFileNamesOrNull,
                                                   const wchar_t* pShaderFileName, const char* pVSEntryPoint, const char* pPSEntryPoint) = 0;
    virtual void        __stdcall       RenderMesh() = 0;

    virtual void        __vectorcall    Translate(const Vector4 dist) = 0;
    virtual void        __stdcall       TranslateX(const float dist) = 0;
    virtual void        __stdcall       TranslateY(const float dist) = 0;
    virtual void        __stdcall       TranslateZ(const float dist) = 0;

    virtual void        __vectorcall    Rotate(const Vector4 angleDegrees) = 0;
    virtual void        __stdcall       RotateX(const float angleDegree) = 0;
    virtual void        __stdcall       RotateY(const float angleDegree) = 0;
    virtual void        __stdcall       RotateZ(const float angleDegree) = 0;

    virtual void        __vectorcall    Scale(const Vector4 scale) = 0;
    virtual void        __stdcall       ScaleX(const float scale) = 0;
    virtual void        __stdcall       ScaleY(const float scale) = 0;
    virtual void        __stdcall       ScaleZ(const float scale) = 0;

    virtual Vector4     __vectorcall    GetPosition() const = 0;
    virtual Vector4     __vectorcall    GetRotation() const = 0;
    virtual Vector4     __vectorcall    GetScale() const = 0;

    virtual void        __vectorcall    SetPosition(const Vector4 position) = 0;
    virtual void        __vectorcall    SetRotation(const Vector4 angleDegrees) = 0;
    virtual void        __vectorcall    SetScale(const Vector4 scale) = 0;

    virtual void        __stdcall       SetCamera(ICamera* pCamera) = 0;
};

interface IRenderer : public IRefObject
{
    virtual bool    __stdcall   Initialize(const HWND hWnd) = 0;

    virtual void    __stdcall   BeginRender() = 0;
    virtual void    __stdcall   EndRender() = 0;

    virtual uint    __stdcall   GetWindowWidth() const = 0;
    virtual uint    __stdcall   GetWindowHeight() const = 0;

    virtual float   __stdcall   GetDeltaTime() const = 0;
    virtual uint    __stdcall   GetFPS() const = 0;

    virtual void    __stdcall   CreateCamera(ICamera** ppOutCamera) = 0;
    virtual void    __stdcall   CreateMeshObject(IMeshObject** ppOutMeshObject) = 0;

    virtual uint    __stdcall   GetRenderMode() const = 0;
    virtual void    __stdcall   SetRenderMode(const uint mode) = 0;

    virtual void*   __stdcall   GetD3DDevice() const = 0;
    virtual void*   __stdcall   GetD3DImmediateContext() const = 0;
};