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
    IndexBuffer** ppIndexBuffers = (IndexBuffer**)m_pIndexBuffers->GetElementsPtrOrNull();
    for (vsize i = 0; i < m_pIndexBuffers->GetNumElements(); ++i)
    {
        IndexBuffer* pBuffer = ppIndexBuffers[i];
        SAFE_DELETE(pBuffer);
    }

    ID3D11ShaderResourceView** ppTextures = (ID3D11ShaderResourceView**)m_pTextures->GetElementsPtrOrNull();
    for (vsize i = 0; i < m_pTextures->GetNumElements(); ++i)
    {
        ID3D11ShaderResourceView* pTexture = ppTextures[i];
        SAFE_RELEASE(pTexture);
    }

    DestroyFixedArray(m_pIndexBuffers);
    DestroyFixedArray(m_pTextures);

    SAFE_RELEASE(m_pRenderer);
}

bool __stdcall TextureFaceGroup::Initialize(IRenderer* pRenderer, const vsize numFaces)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");
    ASSERT(numFaces > 0, "numFaces == 0");

    bool bResult = false;

    if (!CreateFixedArray(&m_pIndexBuffers))
    {
        ASSERT(false, "Failed to create index buffer array");
        goto lb_return;
    }

    if (!m_pIndexBuffers->Initialize(sizeof(UW_PTR_SIZE), numFaces))
    {
        ASSERT(false, "Failed to init index buffer array");
        goto lb_return;
    }

    if (!CreateFixedArray(&m_pTextures))
    {
        ASSERT(false, "Failed to create texture array");
        goto lb_return;
    }

    if (!m_pTextures->Initialize(sizeof(UW_PTR_SIZE), numFaces))
    {
        ASSERT(false, "Failed to init texture array");
        goto lb_return;
    }

    pRenderer->AddRef();
    m_pRenderer = pRenderer;
    m_numFaces = numFaces;

    bResult = true;

lb_return:
    if (!bResult)
    {
        DestroyFixedArray(m_pIndexBuffers);
        DestroyFixedArray(m_pTextures);
    }

    return bResult;
}

bool __stdcall TextureFaceGroup::AddIndexBuffer(const uint16* pIndices, const vsize numIndices)
{
    ASSERT(pIndices != nullptr, "pIndices == nullptr");
    ASSERT(numIndices > 0 && numIndices % 3 == 0, "Wrong numIndices");

    bool bResult = false;

    IndexBuffer* pIndexBuffer = new IndexBuffer;
    if (!pIndexBuffer->Initialize(m_pRenderer, numIndices))
    {
        ASSERT(false, "Failed to init index buffer");
        goto lb_return;
    }

    if (!pIndexBuffer->SetIndex(pIndices, numIndices))
    {
        goto lb_return;
    }

    m_pIndexBuffers->PushBack(&pIndexBuffer, UW_PTR_SIZE);

    bResult = true;

lb_return:
    return bResult;
}

bool __stdcall TextureFaceGroup::AddTexture(const wchar_t* pTexturePath)
{
    ASSERT(pTexturePath != nullptr, "pTexturePath == nullptr");

    bool bResult = false;
    HRESULT hr;

    ID3D11Device* pDevice = (ID3D11Device*)m_pRenderer->GetD3DDevice();
    ID3D11ShaderResourceView* pTextureResourceView = nullptr;
    
    hr = CreateDDSTextureFromFile(pDevice, pTexturePath, nullptr, &pTextureResourceView);
    if (FAILED(hr))
    {
        ASSERT(false, "Failed to create texture from file");
        goto lb_return;
    }

    m_pTextures->PushBack(&pTextureResourceView, UW_PTR_SIZE);

    bResult = true;

lb_return:
    SAFE_RELEASE(pDevice);

    return bResult;
}

IndexBuffer* __stdcall TextureFaceGroup::GetIndexBuffer(const vsize index)
{
    ASSERT(index < m_pIndexBuffers->GetNumElements(), "Invalid index");
    return *(IndexBuffer**)m_pIndexBuffers->GetElementOrNull(index);
}

ID3D11ShaderResourceView* __stdcall TextureFaceGroup::GetTexture(const vsize index)
{
    ASSERT(index < m_pTextures->GetNumElements(), "Invalid index");
    return *(ID3D11ShaderResourceView**)m_pTextures->GetElementOrNull(index);
}

vsize __stdcall TextureFaceGroup::GetNumGroups() const
{
    return m_numFaces;
}