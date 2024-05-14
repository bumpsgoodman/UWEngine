/*
* D3D11 TextureFaceGroup
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.11
*/

#pragma once

#include "IndexBuffer.h"

class RendererD3D11;
class IFxedArray;

class TextureFaceGroup final
{
public:
    TextureFaceGroup() = default;
    TextureFaceGroup(const TextureFaceGroup&) = delete;
    TextureFaceGroup& operator=(const TextureFaceGroup&) = delete;
    ~TextureFaceGroup();

    bool                        __stdcall   Initialize(RendererD3D11* pRenderer, const vsize numFaces);
    void                        __stdcall   Release();

    bool                        __stdcall   AddIndexBuffer(const uint16* pIndices, const vsize numIndices);
    bool                        __stdcall   AddTexture(const wchar_t* pTexturePath);

    IndexBuffer*                __stdcall   GetIndexBuffer(const vsize index);
    ID3D11ShaderResourceView*   __stdcall   GetTexture(const vsize index);

private:
    RendererD3D11*     m_pRenderer = nullptr;

    vsize               m_numFaces = 0;
    IFixedArray*        m_pIndexBuffers = nullptr;  // <IndexBuffer*>
    IFixedArray*        m_pTextures = nullptr;      // <ID3D11ShaderResourceView*>
};