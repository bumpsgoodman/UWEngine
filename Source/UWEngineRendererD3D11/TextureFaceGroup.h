/*
* D3D11 TextureFaceGroup
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.11
*/

#pragma once

#include "IndexBuffer.h"

interface IRenderer;
interface IFxedArray;

class TextureFaceGroup final
{
public:
    TextureFaceGroup() = default;
    TextureFaceGroup(const TextureFaceGroup&) = delete;
    TextureFaceGroup& operator=(const TextureFaceGroup&) = delete;
    ~TextureFaceGroup();

    bool                        __stdcall   Initialize(IRenderer* pRenderer, const vsize numFaces);
    void                        __stdcall   Release();

    bool                        __stdcall   AddIndexBuffer(const uint16* pIndices, const vsize numIndices);
    bool                        __stdcall   AddTexture(const wchar_t* pTexturePath);

    IndexBuffer*                __stdcall   GetIndexBuffer(const vsize index);
    ID3D11ShaderResourceView*   __stdcall   GetTexture(const vsize index);

    vsize                       __stdcall   GetNumGroups() const;

private:
    IRenderer*          m_pRenderer = nullptr;

    vsize               m_numFaces = 0;
    IFixedArray*        m_pIndexBuffers = nullptr;  // <IndexBuffer*>
    IFixedArray*        m_pTextures = nullptr;      // <ID3D11ShaderResourceView*>
};