/*
* FaceGroup
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.11
*/

#pragma once

#include "UWEngineCommon/Defines.h"

class IRendererD3D11;

class FaceGroup final
{
public:
    FaceGroup() = default;
    FaceGroup(const FaceGroup&) = delete;
    FaceGroup& operator=(const FaceGroup&) = delete;
    ~FaceGroup();

    bool    __stdcall   Initialize(IRendererD3D11* pRenderer);
    void    __stdcall   Release();

private:
    
};