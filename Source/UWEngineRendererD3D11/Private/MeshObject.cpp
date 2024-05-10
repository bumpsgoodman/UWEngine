/*
* Mesh Object
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.17
*/

#include "Precompiled.h"
#include "UWEngineCommon/CoreTypes.h"
#include "DDSTextureLoader.h"

struct ConstantBuffer
{
    XMMATRIX WVP;
    XMMATRIX World;
};

UWMETHOD(vsize) MeshObject::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

UWMETHOD(vsize) MeshObject::Release()
{
    --m_refCount;

    if (m_refCount == 0)
    {
        for (vsize i = 0; i < m_numTextures; ++i)
        {
            SAFE_RELEASE(m_ppTextureResourceViews[i]);
        }

        for (vsize i = 0; i < m_numIndexBuffers; ++i)
        {
            SAFE_RELEASE(m_ppIndexBuffers[i]);
        }

        SAFE_RELEASE(m_pSamplerLinear);

        SAFE_RELEASE(m_pConstantBuffer);
        SAFE_RELEASE(m_pVertexBuffer);
        SAFE_RELEASE(m_pVertexLayout);
        SAFE_RELEASE(m_pPixelShader);
        SAFE_RELEASE(m_pVertexShader);

        SAFE_RELEASE(m_pImmediateContext);
        SAFE_RELEASE(m_pRenderer);

        delete this;
        return 0;
    }

    return m_refCount;
}

UWMETHOD(vsize) MeshObject::GetRefCount() const
{
    return m_refCount;
}

UWMETHOD(bool) MeshObject::Initialize(IRendererD3D11* pRenderer)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");

    m_position = XMVectorZero();
    m_rotationDegree = XMVectorZero();
    m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    m_world = XMMatrixIdentity();

    pRenderer->AddRef();
    m_pRenderer = pRenderer;

    m_pImmediateContext = (ID3D11DeviceContext*)pRenderer->Private_GetImmediateContext();

    return true;
}

UWMETHOD(bool) MeshObject::CreateMesh(const int includeFlag,
                                      const void* pVertices, const uint vertexSize, const uint numVertices,
                                      const uint16** ppIndices, const uint16* pNumIndices, const uint numIndexBuffers,
                                      const wchar_t** ppTextureFileNamesOrNull, const uint numTextures,
                                      const wchar_t* pShaderFileName)
{
    ASSERT(pVertices != nullptr, "pVertices == nullptr");
    ASSERT(vertexSize > 0, "vertexSize == 0");
    ASSERT(numVertices > 0, "numVertices == 0");
    ASSERT(ppIndices != nullptr, "ppIndices == nullptr");
    ASSERT(pNumIndices != nullptr, "pNumIndices == nullptr");
    ASSERT(numIndexBuffers > 0, "numIndexBuffers == 0");
    ASSERT(pShaderFileName != nullptr, "pShaderFileName == nullptr");

    bool bResult = false;
    HRESULT hr;

    ID3D11Device* pDevice = (ID3D11Device*)m_pRenderer->Private_GetD3dDevice();
    ID3DBlob* pVertexShaderBlob = nullptr;
    ID3DBlob* pPixelShaderBlob = nullptr;

    hr = CompileShaderFromFile(pShaderFileName, "VSMain", "vs_5_0", &pVertexShaderBlob);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = CompileShaderFromFile(pShaderFileName, "PSMain", "ps_5_0", &pPixelShaderBlob);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    D3D11_INPUT_ELEMENT_DESC layout[3];
    uint numLayout = 2;

    layout[0].SemanticName = "POSITION";
    layout[0].SemanticIndex = 0;
    layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[0].InputSlot = 0;
    layout[0].AlignedByteOffset = 0;
    layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[0].InstanceDataStepRate = 0;

    layout[1].SemanticName = "NORMAL";
    layout[1].SemanticIndex = 0;
    layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[1].InputSlot = 0;
    layout[1].AlignedByteOffset = 12;
    layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[1].InstanceDataStepRate = 0;

    if (UW3D_HAS_INCLUDE_FLAG(includeFlag, UW3D_INCLUDE_FLAG_TEXTURE))
    {
        layout[2].SemanticName = "TEXCOORD";
        layout[2].SemanticIndex = 0;
        layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        layout[2].InputSlot = 0;
        layout[2].AlignedByteOffset = 24;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;
        numLayout = 3;

        m_ppTextureResourceViews = (ID3D11ShaderResourceView**)malloc(sizeof(UW_PTR_SIZE) * numIndexBuffers);
        ASSERT(m_ppTextureResourceViews != nullptr, "Failed to malloc texture resource views");

        for (vsize i = 0; i < numTextures; ++i)
        {
            const wchar_t* pTextureFileName = ppTextureFileNamesOrNull[i];
            hr = CreateDDSTextureFromFile(pDevice, ppTextureFileNamesOrNull[i], nullptr, &m_ppTextureResourceViews[i]);
            if (FAILED(hr))
            {
                goto lb_return;
            }
        }

        // Create the sample state
        D3D11_SAMPLER_DESC sampDesc;
        memset(&sampDesc, 0, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
        hr = pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
        if (FAILED(hr))
        {
            goto lb_return;
        }
    }
    else if (UW3D_HAS_INCLUDE_FLAG(includeFlag, UW3D_INCLUDE_FLAG_COLOR))
    {
        layout[2].SemanticName = "COLOR";
        layout[2].SemanticIndex = 0;
        layout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        layout[2].InputSlot = 0;
        layout[2].AlignedByteOffset = 24;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;
        numLayout = 3;
    }

    hr = pDevice->CreateInputLayout(layout, numLayout, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pVertexLayout);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    SAFE_RELEASE(pVertexShaderBlob);
    SAFE_RELEASE(pPixelShaderBlob);

    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = vertexSize * numVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    memset(&initData, 0, sizeof(initData));
    initData.pSysMem = pVertices;

    hr = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    m_ppIndexBuffers = (ID3D11Buffer**)malloc(sizeof(UW_PTR_SIZE) * numIndexBuffers);
    ASSERT(m_ppIndexBuffers != nullptr, "Failed to malloc index buffers");

    m_pNumIndices = (uint16*)malloc(sizeof(uint16) * numIndexBuffers);
    ASSERT(m_pNumIndices != nullptr, "Failed to malloc m_pNumIndices");

    for (vsize i = 0; i < numIndexBuffers; ++i)
    {
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(uint16) * pNumIndices[i];
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        initData.pSysMem = ppIndices[i];

        m_ppIndexBuffers[i] = nullptr;
        hr = pDevice->CreateBuffer(&bd, &initData, &m_ppIndexBuffers[i]);
        if (FAILED(hr))
        {
            goto lb_return;
        }

        m_pNumIndices[i] = pNumIndices[i];
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    m_numTextures = numTextures;
    m_includeFlag = includeFlag;
    m_numVertices = numVertices;
    m_vertexSize = vertexSize;
    m_numIndexBuffers = numIndexBuffers;

    m_world = XMMatrixIdentity();

    bResult = true;

lb_return:
    SAFE_RELEASE(pVertexShaderBlob);
    SAFE_RELEASE(pPixelShaderBlob);

#if 0
    if (!bResult)
    {
        SAFE_RELEASE(m_pVertexShader);
        SAFE_RELEASE(m_pPixelShader);
        SAFE_RELEASE(m_pVertexLayout);
        SAFE_RELEASE(m_pVertexBuffer);
        SAFE_RELEASE(m_pIndexBuffer);
        SAFE_RELEASE(m_pConstantBuffer);
        SAFE_RELEASE(m_pTextureRV);
        SAFE_RELEASE(m_pSamplerLinear);
    }
#endif

    SAFE_RELEASE(pDevice);
    
    return bResult;
}

UWMETHOD(void) MeshObject::RenderMesh()
{
    ID3D11DeviceContext* pImmediateContext = (ID3D11DeviceContext*)m_pRenderer->Private_GetImmediateContext();

    ConstantBuffer cb;
    cb.WVP = XMMatrixTranspose(m_world * m_pCamera->GetView() * m_pCamera->GetProjection());
    pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    UINT stride = m_vertexSize;
    UINT offset = 0;

    pImmediateContext->IASetInputLayout(m_pVertexLayout);
    pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

    pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);

    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    for (vsize i = 0; i < m_numIndexBuffers; ++i)
    {
        pImmediateContext->IASetIndexBuffer(m_ppIndexBuffers[i], DXGI_FORMAT_R16_UINT, 0);

        if (UW3D_HAS_INCLUDE_FLAG(m_includeFlag, UW3D_INCLUDE_FLAG_TEXTURE))
        {
            pImmediateContext->PSSetShaderResources(0, 1, &m_ppTextureResourceViews[i]);
            pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
        }

        pImmediateContext->DrawIndexed(m_pNumIndices[i], 0, 0);
    }

    SAFE_RELEASE(pImmediateContext);
}

UWMETHOD_VECTOR(void) MeshObject::Translate(const XMVECTOR dist)
{
    m_position += dist;
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::TranslateX(const float dist)
{
    m_position = XMVectorSetX(m_position, XMVectorGetX(m_position) + dist);
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::TranslateY(const float dist)
{
    m_position = XMVectorSetY(m_position, XMVectorGetY(m_position) + dist);
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::TranslateZ(const float dist)
{
    m_position = XMVectorSetY(m_position, XMVectorGetZ(m_position) + dist);
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject::Rotate(const XMVECTOR angleDegree)
{
    m_rotationDegree += angleDegree;
    m_rotationDegree = XMVectorSet(Wrap(XMVectorGetX(m_rotationDegree), 0.0f, 360.0f),
                                   Wrap(XMVectorGetY(m_rotationDegree), 0.0f, 360.0f),
                                   Wrap(XMVectorGetZ(m_rotationDegree), 0.0f, 360.0f),
                                   0.0f);
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::RotateX(const float angleDegree)
{
    m_rotationDegree = XMVectorSetX(m_rotationDegree, XMVectorGetX(m_rotationDegree) + angleDegree);
    m_rotationDegree = XMVectorSetX(m_rotationDegree, Wrap(XMVectorGetX(m_rotationDegree), 0.0f, 360.0f));
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::RotateY(const float angleDegree)
{
    m_rotationDegree = XMVectorSetY(m_rotationDegree, XMVectorGetY(m_rotationDegree) + angleDegree);
    m_rotationDegree = XMVectorSetY(m_rotationDegree, Wrap(XMVectorGetY(m_rotationDegree), 0.0f, 360.0f));
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::RotateZ(const float angleDegree)
{
    m_rotationDegree = XMVectorSetZ(m_rotationDegree, XMVectorGetZ(m_rotationDegree) + angleDegree);
    m_rotationDegree = XMVectorSetZ(m_rotationDegree, Wrap(XMVectorGetZ(m_rotationDegree), 0.0f, 360.0f));
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject::Scale(const XMVECTOR scale)
{
    m_scale += scale;
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::ScaleX(const float scale)
{
    m_scale = XMVectorSetX(m_scale, XMVectorGetX(m_scale) + scale);
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::ScaleY(const float scale)
{
    m_scale = XMVectorSetY(m_scale, XMVectorGetY(m_scale) + scale);
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject::ScaleZ(const float scale)
{
    m_scale = XMVectorSetZ(m_scale, XMVectorGetZ(m_scale) + scale);
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject::SetPosition(const XMVECTOR position)
{
    m_position = position;
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject::SetRotation(const XMVECTOR angleDegrees)
{
    m_rotationDegree = XMVectorSet(Wrap(XMVectorGetX(angleDegrees), 0.0f, 360.0f),
                                   Wrap(XMVectorGetY(angleDegrees), 0.0f, 360.0f),
                                   Wrap(XMVectorGetZ(angleDegrees), 0.0f, 360.0f),
                                   0.0f);
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject::SetScale(const XMVECTOR scale)
{
    m_scale = scale;
    updateWorldMatrix();
}

UWMETHOD_VECTOR(XMVECTOR) MeshObject::GetPosition() const
{
    return m_position;
}

UWMETHOD_VECTOR(XMVECTOR) MeshObject::GetRotation() const
{
    return m_rotationDegree;
}

UWMETHOD_VECTOR(XMVECTOR) MeshObject::GetScale() const
{
    return m_scale;
}

UWMETHOD(void) MeshObject::SetCamera(ICamera* pCamera)
{
    ASSERT(pCamera != nullptr, "pCamera == nullptr");
    m_pCamera = pCamera;
}

UWMETHOD(void) MeshObject::updateWorldMatrix()
{
    FXMMATRIX translationMat = XMMatrixTranslationFromVector(m_position);
    FXMMATRIX rotationMat = XMMatrixRotationRollPitchYawFromVector(XMVectorSet(DegreeToRad(XMVectorGetX(m_rotationDegree)),
                                                                               DegreeToRad(XMVectorGetY(m_rotationDegree)),
                                                                               DegreeToRad(XMVectorGetZ(m_rotationDegree)),
                                                                               0.0f));
    FXMMATRIX scaleMat = XMMatrixScalingFromVector(m_scale);

    m_world = scaleMat * rotationMat * translationMat;
}