/*
* D3D11 인덱스 버퍼
*
* 작성자: bumpsgoodman
* 작성일: 2023.05.14
*/

#include "Precompiled.h"
#include "IndexBuffer.h"
#include "UWEngineCommon/Interfaces/IRenderer.h"

IndexBuffer::~IndexBuffer()
{
    Release();
}

bool __stdcall IndexBuffer::Initialize(IRenderer* pRenderer, const uint numIndices)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");
    ASSERT(numIndices > 0 && numIndices % 3 == 0, "Wrong numIndices"); // 현재는 삼각형 단위로만 가능
    
    bool bResult = false;
    HRESULT hr;

    ID3D11Device* pDevice = (ID3D11Device*)pRenderer->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = (UINT)(sizeof(uint16) * numIndices);
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = pDevice->CreateBuffer(&bd, nullptr, &m_pIndexBuffer);
    if (FAILED(hr))
    {
        ASSERT(false, "Failed to create index buffer");
        goto lb_return;
    }

    pRenderer->AddRef();
    m_pRenderer = pRenderer;
    m_numIndices = numIndices;
    m_startIndex = 0;

    bResult = true;

lb_return:
    SAFE_RELEASE(pDevice);

    return bResult;
}

void __stdcall IndexBuffer::Release()
{
    SAFE_RELEASE(m_pIndexBuffer);
    SAFE_RELEASE(m_pRenderer);
}

uint __stdcall IndexBuffer::GetStartIndex() const
{
    return m_startIndex;
}

void __stdcall IndexBuffer::SetStartIndex(const uint startIndex)
{
    ASSERT(startIndex < m_numIndices, "Wrong startIndex");
    m_startIndex = startIndex;
}

bool __stdcall IndexBuffer::SetIndex(const void* pIndices, const uint numIndices)
{
    ASSERT(pIndices != nullptr, "pIndices == nullptr");
    ASSERT(numIndices > 0 && numIndices % 3 == 0 && numIndices <= m_numIndices, "Wrong numIndices"); // 현재는 삼각형 단위로만 가능

    bool bResult = false;
    HRESULT hr;

    ID3D11DeviceContext* pImmediateContext = (ID3D11DeviceContext*)m_pRenderer->GetD3DImmediateContext();

    D3D11_MAPPED_SUBRESOURCE subResource;
    hr = pImmediateContext->Map(m_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    uint16* pDest = (uint16*)subResource.pData + m_startIndex;
    uint16* pSrc = (uint16*)pIndices;
    for (uint i = 0; i < numIndices; ++i)
    {
        *pDest = *pSrc;

        ++pDest;
        ++pSrc;
    }

    pImmediateContext->Unmap(m_pIndexBuffer, 0);

    bResult = true;

lb_return:
    SAFE_RELEASE(pImmediateContext);

    return bResult;
}

uint __stdcall IndexBuffer::GetNumIndices() const
{
    return m_numIndices;
}

ID3D11Buffer* __stdcall IndexBuffer::GetBuffer() const
{
    return m_pIndexBuffer;
}