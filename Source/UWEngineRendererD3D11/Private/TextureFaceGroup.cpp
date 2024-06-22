/*
* D3D11 TextureFaceGroup
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.14
*/

#include "Precompiled.h"
#include "TextureFaceGroup.h"
#include "IndexBuffer.h"
#include "UWEngineCommon/Interfaces/IRenderer.h"

TextureFaceGroup::~TextureFaceGroup()
{
    Release();
}

void TextureFaceGroup::Release()
{
    IndexBuffer** ppIndexBuffers = (IndexBuffer**)m_indexBuffers.GetElementsPtr();
    for (uint i = 0; i < m_indexBuffers.GetNumElements(); ++i)
    {
        IndexBuffer* pBuffer = ppIndexBuffers[i];
        SAFE_DELETE(pBuffer);
    }

    ID3D11ShaderResourceView** ppTextures = (ID3D11ShaderResourceView**)m_textures.GetElementsPtr();
    for (uint i = 0; i < m_textures.GetNumElements(); ++i)
    {
        ID3D11ShaderResourceView* pTexture = ppTextures[i];
        SAFE_RELEASE(pTexture);
    }

    m_indexBuffers.Release();
    m_textures.Release();

    SAFE_RELEASE(m_pRenderer);
}

bool __stdcall TextureFaceGroup::Initialize(IRenderer* pRenderer, const uint numFaces)
{
    if (!m_indexBuffers.Initialize(UW_PTR_SIZE, numFaces))
    {
        CRASH();
    }

    if (!m_textures.Initialize(UW_PTR_SIZE, numFaces))
    {
        CRASH();
    }

    pRenderer->AddRef();
    m_pRenderer = pRenderer;
    m_numFaces = numFaces;

    return true;
}

bool __stdcall TextureFaceGroup::AddIndexBuffer(const uint16* pIndices, const uint numIndices)
{
    IndexBuffer* pIndexBuffer = new IndexBuffer;
    if (!pIndexBuffer->Initialize(m_pRenderer, numIndices))
    {
        CRASH();
    }

    if (!pIndexBuffer->SetIndex(pIndices, numIndices))
    {
        CRASH();
    }

    m_indexBuffers.PushBack(&pIndexBuffer, UW_PTR_SIZE);

    return true;
}

bool __stdcall TextureFaceGroup::AddTexture(const wchar_t* pTexturePath)
{
    HRESULT hr;

    ID3D11Device* pDevice = (ID3D11Device*)m_pRenderer->GetD3DDevice();
    ID3D11ShaderResourceView* pTextureResourceView = nullptr;
    
    hr = CreateDDSTextureFromFile(pDevice, pTexturePath, nullptr, &pTextureResourceView);
    if (FAILED(hr))
    {
        CRASH();
    }

    m_textures.PushBack(&pTextureResourceView, UW_PTR_SIZE);

    SAFE_RELEASE(pDevice);
    return true;
}

IndexBuffer* __stdcall TextureFaceGroup::GetIndexBuffer(const uint index)
{
    return *(IndexBuffer**)m_indexBuffers.GetElement(index);
}

ID3D11ShaderResourceView* __stdcall TextureFaceGroup::GetTexture(const uint index)
{
    return *(ID3D11ShaderResourceView**)m_textures.GetElement(index);
}

uint __stdcall TextureFaceGroup::GetNumGroups() const
{
    return m_numFaces;
}