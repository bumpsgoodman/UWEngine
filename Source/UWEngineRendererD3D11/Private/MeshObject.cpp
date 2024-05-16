/*
* D3D11 메시 오브젝트
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.17
*/

#include "Precompiled.h"
#include "UWEngineCommon/Interfaces/IRenderer.h"
#include "MeshObject.h"
#include "DDSTextureLoader.h"
#include "D3D11Helper.h"

struct ConstantBuffer
{
    XMMATRIX WVP;
    XMMATRIX World;
};

vsize __stdcall MeshObject::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

vsize __stdcall MeshObject::Release()
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
        SAFE_RELEASE(m_pDevice);
        SAFE_RELEASE(m_pRenderer);

        delete this;
        return 0;
    }

    return m_refCount;
}

vsize __stdcall MeshObject::GetRefCount() const
{
    return m_refCount;
}

bool __stdcall MeshObject::Initialize(IRenderer* pRenderer)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");

    m_position = XMVectorZero();
    m_rotationDegree = XMVectorZero();
    m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    m_world = XMMatrixIdentity();

    pRenderer->AddRef();
    m_pRenderer = pRenderer;

    m_pDevice = (ID3D11Device*)pRenderer->GetD3DDevice();
    m_pImmediateContext = (ID3D11DeviceContext*)pRenderer->GetD3DImmediateContext();

    return true;
}

bool __stdcall MeshObject::CreateMesh(const int includeFlag,
                                      const void* pVertices, const uint vertexSize, const uint numVertices,
                                      const uint16** ppIndices, const uint16* pNumIndices, const uint numIndexBuffers,
                                      const void* pTexCoordsOrNull,
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

    ID3DBlob* pVertexShaderBlob = nullptr;
    ID3DBlob* pPixelShaderBlob = nullptr;

    hr = CompileShaderFromFile(pShaderFileName, "VSMain", "vs_5_0", &pVertexShaderBlob);
    if (FAILED(hr))
    {
        CRASH();
        goto lb_return;
    }

    hr = CompileShaderFromFile(pShaderFileName, "PSMain", "ps_5_0", &pPixelShaderBlob);
    if (FAILED(hr))
    {
        CRASH();
        goto lb_return;
    }

    hr = m_pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader);
    if (FAILED(hr))
    {
        CRASH();
        goto lb_return;
    }

    hr = m_pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader);
    if (FAILED(hr))
    {
        CRASH();
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
        layout[2].InputSlot = 1;
        layout[2].AlignedByteOffset = 0;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;
        numLayout = 3;



        m_ppTextureResourceViews = (ID3D11ShaderResourceView**)malloc(sizeof(UW_PTR_SIZE) * numIndexBuffers);
        ASSERT(m_ppTextureResourceViews != nullptr, "Failed to malloc texture resource views");

        for (vsize i = 0; i < numTextures; ++i)
        {
            const wchar_t* pTextureFileName = ppTextureFileNamesOrNull[i];
            hr = CreateDDSTextureFromFile(m_pDevice, ppTextureFileNamesOrNull[i], nullptr, &m_ppTextureResourceViews[i]);
            if (FAILED(hr))
            {
                CRASH();
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
        hr = m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
        if (FAILED(hr))
        {
            CRASH();
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

    hr = m_pDevice->CreateInputLayout(layout, numLayout, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pVertexLayout);
    if (FAILED(hr))
    {
        CRASH();
        goto lb_return;
    }

    SAFE_RELEASE(pVertexShaderBlob);
    SAFE_RELEASE(pPixelShaderBlob);

    if (!m_vertexBuffer.Initialize(m_pRenderer, VERTEX_BUFFER_FLAG_DEFAULT, vertexSize, numVertices))
    {
        CRASH();
        goto lb_return;
    }
    m_vertexBuffer.SetVertex(pVertices, numVertices, vertexSize, 0, VERTEX_BUFFER_FLAG_DEFAULT);

    if (GET_MASK(includeFlag, UW3D_INCLUDE_FLAG_TEXTURE))
    {
        if (!m_uvBuffer.Initialize(m_pRenderer, VERTEX_BUFFER_FLAG_TEXCOORD, sizeof(float) * 2, numVertices))
        {
            CRASH();
            goto lb_return;
        }

        m_uvBuffer.SetVertex(pTexCoordsOrNull, numVertices, sizeof(float) * 2, 0, VERTEX_BUFFER_FLAG_TEXCOORD);

        m_faceGroup.Initialize(m_pRenderer, numIndexBuffers);
        
        for (vsize i = 0; i < numIndexBuffers; ++i)
        {
            m_faceGroup.AddIndexBuffer(ppIndices[i], pNumIndices[i]);
            m_faceGroup.AddTexture(ppTextureFileNamesOrNull[i]);
        }
    }

    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = vertexSize * numVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA initData;
    memset(&initData, 0, sizeof(initData));
    initData.pSysMem = pVertices;

    hr = m_pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
    if (FAILED(hr))
    {
        CRASH();
        goto lb_return;
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(float) * 2 * numVertices;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = 0;

    memset(&initData, 0, sizeof(initData));
    initData.pSysMem = pTexCoordsOrNull;

    hr = m_pDevice->CreateBuffer(&bd, &initData, &m_pUVBuffer);
    if (FAILED(hr))
    {
        CRASH();
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
        hr = m_pDevice->CreateBuffer(&bd, &initData, &m_ppIndexBuffers[i]);
        if (FAILED(hr))
        {
            CRASH();
            goto lb_return;
        }

        m_pNumIndices[i] = pNumIndices[i];
    }

    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
    if (FAILED(hr))
    {
        CRASH();
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
    
    return bResult;
}

void __stdcall MeshObject::RenderMesh()
{
    ConstantBuffer cb;
    cb.WVP = XMMatrixTranspose(m_world * UW_Matrix44ToXMMatrix(m_pCamera->GetView()) * UW_Matrix44ToXMMatrix(m_pCamera->GetProjection()));
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    UINT strides[] = { m_vertexSize, sizeof(float) * 2 };
    UINT offsets[] = { 0, 0 };
    ID3D11Buffer* buffers[] = { m_vertexBuffer.GetBuffer(), m_uvBuffer.GetBuffer() };

    m_pImmediateContext->IASetInputLayout(m_pVertexLayout);
    m_pImmediateContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);

    m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);

    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

#if 1
    for (vsize i = 0; i < m_numIndexBuffers; ++i)
    {
        m_pImmediateContext->IASetIndexBuffer(m_faceGroup.GetIndexBuffer(i)->GetBuffer(), DXGI_FORMAT_R16_UINT, 0);

        if (UW3D_HAS_INCLUDE_FLAG(m_includeFlag, UW3D_INCLUDE_FLAG_TEXTURE))
        {
            ID3D11ShaderResourceView* pTexture = m_faceGroup.GetTexture(i);
            m_pImmediateContext->PSSetShaderResources(0, 1, &pTexture);
            m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
        }

        m_pImmediateContext->DrawIndexed((UINT)m_faceGroup.GetIndexBuffer(i)->GetNumIndices(), 0, 0);
    }
#endif
}

void __vectorcall MeshObject::Translate(const Vector4 dist)
{
    m_position += UW_Vector4ToXMVector(dist);
    updateWorldMatrix();
}

void __stdcall MeshObject::TranslateX(const float dist)
{
    m_position = XMVectorSetX(m_position, XMVectorGetX(m_position) + dist);
    updateWorldMatrix();
}

void __stdcall MeshObject::TranslateY(const float dist)
{
    m_position = XMVectorSetY(m_position, XMVectorGetY(m_position) + dist);
    updateWorldMatrix();
}

void __stdcall MeshObject::TranslateZ(const float dist)
{
    m_position = XMVectorSetY(m_position, XMVectorGetZ(m_position) + dist);
    updateWorldMatrix();
}

void __vectorcall MeshObject::Rotate(const Vector4 angleDegree)
{
    m_rotationDegree += UW_Vector4ToXMVector(angleDegree);
    m_rotationDegree = XMVectorSet(Wrap(XMVectorGetX(m_rotationDegree), 0.0f, 360.0f),
                                   Wrap(XMVectorGetY(m_rotationDegree), 0.0f, 360.0f),
                                   Wrap(XMVectorGetZ(m_rotationDegree), 0.0f, 360.0f),
                                   0.0f);
    updateWorldMatrix();
}

void __stdcall MeshObject::RotateX(const float angleDegree)
{
    m_rotationDegree = XMVectorSetX(m_rotationDegree, XMVectorGetX(m_rotationDegree) + angleDegree);
    m_rotationDegree = XMVectorSetX(m_rotationDegree, Wrap(XMVectorGetX(m_rotationDegree), 0.0f, 360.0f));
    updateWorldMatrix();
}

void __stdcall MeshObject::RotateY(const float angleDegree)
{
    m_rotationDegree = XMVectorSetY(m_rotationDegree, XMVectorGetY(m_rotationDegree) + angleDegree);
    m_rotationDegree = XMVectorSetY(m_rotationDegree, Wrap(XMVectorGetY(m_rotationDegree), 0.0f, 360.0f));
    updateWorldMatrix();
}

void __stdcall MeshObject::RotateZ(const float angleDegree)
{
    m_rotationDegree = XMVectorSetZ(m_rotationDegree, XMVectorGetZ(m_rotationDegree) + angleDegree);
    m_rotationDegree = XMVectorSetZ(m_rotationDegree, Wrap(XMVectorGetZ(m_rotationDegree), 0.0f, 360.0f));
    updateWorldMatrix();
}

void __vectorcall MeshObject::Scale(const Vector4 scale)
{
    m_scale += UW_Vector4ToXMVector(scale);
    updateWorldMatrix();
}

void __stdcall MeshObject::ScaleX(const float scale)
{
    m_scale = XMVectorSetX(m_scale, XMVectorGetX(m_scale) + scale);
    updateWorldMatrix();
}

void __stdcall MeshObject::ScaleY(const float scale)
{
    m_scale = XMVectorSetY(m_scale, XMVectorGetY(m_scale) + scale);
    updateWorldMatrix();
}

void __stdcall MeshObject::ScaleZ(const float scale)
{
    m_scale = XMVectorSetZ(m_scale, XMVectorGetZ(m_scale) + scale);
    updateWorldMatrix();
}

void __vectorcall MeshObject::SetPosition(const Vector4 position)
{
    m_position = UW_Vector4ToXMVector(position);
    updateWorldMatrix();
}

void __vectorcall MeshObject::SetRotation(const Vector4 angleDegrees)
{
    const XMVECTOR degrees = UW_Vector4ToXMVector(angleDegrees);

    m_rotationDegree = XMVectorSet(Wrap(XMVectorGetX(degrees), 0.0f, 360.0f),
                                   Wrap(XMVectorGetY(degrees), 0.0f, 360.0f),
                                   Wrap(XMVectorGetZ(degrees), 0.0f, 360.0f),
                                   0.0f);
    updateWorldMatrix();
}

void __vectorcall MeshObject::SetScale(const Vector4 scale)
{
    m_scale = UW_Vector4ToXMVector(scale);
    updateWorldMatrix();
}

Vector4 __vectorcall MeshObject::GetPosition() const
{
    return UW_XMVectorToVector4(m_position);
}

Vector4 __vectorcall MeshObject::GetRotation() const
{
    return UW_XMVectorToVector4(m_rotationDegree);
}

Vector4 __vectorcall MeshObject::GetScale() const
{
    return UW_XMVectorToVector4(m_scale);
}

void __stdcall MeshObject::SetCamera(ICamera* pCamera)
{
    ASSERT(pCamera != nullptr, "pCamera == nullptr");
    m_pCamera = pCamera;
}

void __stdcall MeshObject::updateWorldMatrix()
{
    const XMMATRIX translationMat = XMMatrixTranslationFromVector(m_position);
    const XMMATRIX rotationMat = XMMatrixRotationRollPitchYawFromVector(XMVectorSet(DegreeToRad(XMVectorGetX(m_rotationDegree)),
                                                                                    DegreeToRad(XMVectorGetY(m_rotationDegree)),
                                                                                    DegreeToRad(XMVectorGetZ(m_rotationDegree)),
                                                                                    0.0f));
    const XMMATRIX scaleMat = XMMatrixScalingFromVector(m_scale);

    m_world = scaleMat * rotationMat * translationMat;
}