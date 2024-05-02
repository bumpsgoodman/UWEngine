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
        SAFE_RELEASE(m_pTextureRV);
        SAFE_RELEASE(m_pSamplerLinear);

        SAFE_RELEASE(m_pConstantBuffer);
        SAFE_RELEASE(m_pIndexBuffer);
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

    m_renderType = MESH_RENDER_TYPE_DEFAULT;

    return true;
}

UWMETHOD(bool) MeshObject::CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                                      const uint16* pIndices, const uint numIndices,
                                      const wchar_t* pShaderFileName,
                                      const wchar_t* pTextureFileName)
{
    ASSERT(pVertices != nullptr, "pVertices == nullptr");
    ASSERT(pIndices != nullptr, "pIndices == nullptr");
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
    uint numLayout = 0;

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

    switch (m_renderType)
    {
    case MESH_RENDER_TYPE_DEFAULT:
        numLayout = 2;
        break;
    case MESH_RENDER_TYPE_COLOR:
        layout[2].SemanticName = "COLOR";
        layout[2].SemanticIndex = 0;
        layout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        layout[2].InputSlot = 0;
        layout[2].AlignedByteOffset = 24;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;
        numLayout = 3;
        break;
    case MESH_RENDER_TYPE_TEXTURE:
        layout[2].SemanticName = "TEXCOORD";
        layout[2].SemanticIndex = 0;
        layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        layout[2].InputSlot = 0;
        layout[2].AlignedByteOffset = 24;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;
        numLayout = 3;
        break;
    default:
        ASSERT(false, "Invalid render type");
        break;
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

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(uint16) * numIndices;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    initData.pSysMem = pIndices;

    hr = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
    if (FAILED(hr))
    {
        goto lb_return;
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

    if (m_renderType == MESH_RENDER_TYPE_TEXTURE)
    {
        hr = CreateDDSTextureFromFile(pDevice, pTextureFileName, nullptr, &m_pTextureRV);
        if (FAILED(hr))
        {
            goto lb_return;
        }

        // Create the sample state
        D3D11_SAMPLER_DESC sampDesc;
        memset(&sampDesc, 0, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
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

    m_numVertices = numVertices;
    m_numIndices = numIndices;
    m_vertexSize = vertexSize;

    m_world = XMMatrixIdentity();

    bResult = true;

lb_return:
    SAFE_RELEASE(pVertexShaderBlob);
    SAFE_RELEASE(pPixelShaderBlob);

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
    pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    switch (m_renderType)
    {
    case MESH_RENDER_TYPE_DEFAULT:
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        break;
    case MESH_RENDER_TYPE_COLOR:
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        break;
    case MESH_RENDER_TYPE_TEXTURE:
        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pImmediateContext->PSSetShaderResources(0, 1, &m_pTextureRV);
        pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
        break;
    default:
        ASSERT(false, "Invalid render type");
        break;
    }

    pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);

    pImmediateContext->DrawIndexed(m_numIndices, 0, 0);

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

UWMETHOD(MESH_RENDER_TYPE) MeshObject::GetRenderType() const
{
    return m_renderType;
}

UWMETHOD(void) MeshObject::SetRenderType(const MESH_RENDER_TYPE type)
{
    if (type >= NUM_MESH_RENDER_TYPE)
    {
        m_renderType = MESH_RENDER_TYPE_DEFAULT;
    }
    else
    {
        m_renderType = type;
    }
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