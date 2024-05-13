/*
* D3D11 FaceGroup
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.11
*/

#pragma once

#include "IndexBuffer.h"

class IRendererD3D11;

class FaceGroup final
{
public:
    FaceGroup() = default;
    FaceGroup(const FaceGroup&) = delete;
    FaceGroup& operator=(const FaceGroup&) = delete;
    ~FaceGroup();

    bool                        __stdcall   Initialize(IRendererD3D11* pRenderer, const vsize numFaceGroups);
    void                        __stdcall   Release();

    IndexBuffer*                __stdcall   GetIndexBuffer(const vsize index);
    ID3D11ShaderResourceView*   __stdcall   GetTexture(const vsize index);

private:
    vsize                           m_numFaceGroups = 0;
    IndexBuffer**                   m_pIndexBuffers = nullptr;
    ID3D11ShaderResourceView**      m_pTextureResourceViews = nullptr;
};