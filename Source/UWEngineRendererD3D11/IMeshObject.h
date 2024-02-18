/*
* MeshObject Interface
*
* 작성자: bumpsgoodman
* 작성일: 2023.02.03
*/

#pragma once

#include "UWEngineCommon/Defines.h"

interface IRendererD3D11;

class IMeshObject : public IRefObject
{
public:
    virtual bool __stdcall Initialize(IRendererD3D11* pRenderer) = 0;

    virtual bool __stdcall CreateMesh(const void* pVertices, const uint_t vertexSize, const uint_t numVertices,
                                      const uint16_t* pIndices, const uint_t numIndices,
                                      const wchar_t* pShaderFileName) = 0;

    virtual void __stdcall RenderMesh() = 0;
};