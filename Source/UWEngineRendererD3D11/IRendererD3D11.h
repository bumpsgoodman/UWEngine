/*
* RendererD3D11 인터페이스
*
* 작성자: bumpsgoodman
* 작성일: 2023.01.28
*/

#pragma once

#include "UWEngineCommon/Defines.h"
#include "IMeshObject.h"
#include "ICamera.h"

interface IRendererD3D11 : public IRefObject
{
    virtual bool __stdcall Initialize(const HWND hWnd) = 0;

    virtual void __stdcall BeginRender() = 0;
    virtual void __stdcall EndRender() = 0;
    virtual void __stdcall Present() = 0;

    virtual IMeshObject* __stdcall CreateMeshObject() = 0;
    virtual ICamera* __stdcall CreateCamera() = 0;

    virtual void* __stdcall Private_GetD3dDevice() const = 0;
    virtual void* __stdcall Private_GetImmediateContext() const = 0;
};