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

    bool                        __stdcall   Initialize(IRenderer* pRenderer, const uint numFaces);
    void                        __stdcall   Release();

    bool                        __stdcall   AddIndexBuffer(const uint16* pIndices, const uint numIndices);
    bool                        __stdcall   AddTexture(const wchar_t* pTexturePath);

    IndexBuffer*                __stdcall   GetIndexBuffer(const uint index);
    ID3D11ShaderResourceView*   __stdcall   GetTexture(const uint index);

    uint                        __stdcall   GetNumGroups() const;

private:
    IRenderer*          m_pRenderer = nullptr;

    uint                m_numFaces = 0;
    FixedArray          m_indexBuffers;  // <IndexBuffer*>
    FixedArray          m_textures;      // <ID3D11ShaderResourceView*>
};