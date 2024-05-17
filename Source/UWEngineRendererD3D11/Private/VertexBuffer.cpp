/*
* D3D11 버텍스 버퍼
*
* 작성자: bumpsgoodman
* 작성일: 2023.05.13
*/

#include "Precompiled.h"
#include "VertexBuffer.h"
#include "UWEngineCommon/Interfaces/IRenderer.h"

VertexBuffer::~VertexBuffer()
{
    Release();
}

bool __stdcall VertexBuffer::Initialize(IRenderer* pRenderer, const uint flag, const vsize vertexSize, const vsize numVertices)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");
    ASSERT(flag > 0, "Invalid flag");
    ASSERT(vertexSize > 0, "vertexSize == 0");
    ASSERT(numVertices > 0, "numVertices == 0");

    bool bResult = false;
    HRESULT hr;

    ID3D11Device* pDevice = (ID3D11Device*)pRenderer->GetD3DDevice();

    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(bd));
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = (UINT)(vertexSize * numVertices);
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    hr = pDevice->CreateBuffer(&bd, nullptr, &m_pVertexBuffer);
    if (FAILED(hr))
    {
        ASSERT(false, "Failed to create vertex buffer");
        goto lb_return;
    }

    pRenderer->AddRef();
    m_pRenderer = pRenderer;
    m_flag = flag;
    m_numVertices = numVertices;
    m_vertexSize = vertexSize;
    m_startIndex = 0;

    bResult = true;

lb_return:
    SAFE_RELEASE(pDevice);

    return bResult;
}

void __stdcall VertexBuffer::Release()
{
    SAFE_RELEASE(m_pVertexBuffer);
    SAFE_RELEASE(m_pRenderer);
}

vsize __stdcall VertexBuffer::GetStartIndex() const
{
    return m_startIndex;
}

void __stdcall VertexBuffer::SetStartIndex(const vsize startIndex)
{
    ASSERT(startIndex < m_numVertices, "Wrong startIndex");
    m_startIndex = startIndex;
}

bool __stdcall VertexBuffer::SetVertex(const void* pVertices, const vsize num, const vsize vertexSize, const vsize offset, const uint flag)
{
    ASSERT(pVertices != nullptr, "pVertices == nullptr");
    ASSERT(num > 0, "num == 0");
    ASSERT(vertexSize > 0 && vertexSize <= m_vertexSize, "Wrong vertexSize");
    ASSERT(offset < m_vertexSize, "Wrong offset");
    ASSERT(GET_MASK(m_flag, flag) == flag, "Mismatch flag");

    bool bResult = false;
    HRESULT hr;

    ID3D11DeviceContext* pImmediateContext = (ID3D11DeviceContext*)m_pRenderer->GetD3DImmediateContext();

    D3D11_MAPPED_SUBRESOURCE subResource;
    hr = pImmediateContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    char* pDest = (char*)subResource.pData + m_startIndex * m_vertexSize;
    char* pSrc = (char*)pVertices;
    for (vsize i = 0; i < num; ++i)
    {
        memcpy(pDest + offset, pSrc, vertexSize);
        pDest += m_vertexSize;
        pSrc += vertexSize;
    }

    pImmediateContext->Unmap(m_pVertexBuffer, 0);

    bResult = true;

lb_return:
    SAFE_RELEASE(pImmediateContext);

    return bResult;
}

bool __stdcall VertexBuffer::SetVertex1(const void* pVertex, const vsize num, const vsize vertexSize, const vsize offset, const uint flag)
{
    ASSERT(pVertex != nullptr, "pVertex == nullptr");
    ASSERT(num > 0, "num == 0");
    ASSERT(vertexSize > 0 && vertexSize <= m_vertexSize, "Wrong vertexSize");
    ASSERT(offset < m_vertexSize, "Wrong offset");
    ASSERT(GET_MASK(m_flag, flag) == flag, "Mismatch flag");

    bool bResult = false;
    HRESULT hr;

    ID3D11DeviceContext* pImmediateContext = (ID3D11DeviceContext*)m_pRenderer->GetD3DImmediateContext();

    D3D11_MAPPED_SUBRESOURCE subResource;
    hr = pImmediateContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE, 0, &subResource);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    char* pDest = (char*)subResource.pData + m_startIndex * m_vertexSize;
    for (vsize i = 0; i < num; ++i)
    {
        memcpy(pDest + offset, pVertex, vertexSize);
        pDest += m_vertexSize;
    }

    pImmediateContext->Unmap(m_pVertexBuffer, 0);

    bResult = true;

lb_return:
    SAFE_RELEASE(pImmediateContext);

    return bResult;
}

ID3D11Buffer* __stdcall VertexBuffer::GetBuffer() const
{
    return m_pVertexBuffer;
}