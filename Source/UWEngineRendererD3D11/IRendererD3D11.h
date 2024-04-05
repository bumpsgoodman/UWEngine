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
    UWMETHOD(bool) Initialize(const HWND hWnd) PURE;

    UWMETHOD(void) BeginRender() PURE;
    UWMETHOD(void) EndRender() PURE;

    UWMETHOD(IMeshObject*) CreateMeshObject() PURE;
    UWMETHOD(ICamera*) CreateCamera() PURE;

    UWMETHOD(float) GetDeltaTime() const PURE;
    UWMETHOD(uint) GetFPS() const PURE;

    UWMETHOD(void*) Private_GetD3dDevice() const PURE;
    UWMETHOD(void*) Private_GetImmediateContext() const PURE;
};