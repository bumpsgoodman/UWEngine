/*
* D3D11 메시 오브젝트
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.17
*/

#include "Precompiled.h"
#include "UWEngineCommon/Interfaces/IRenderer.h"
#include "MeshObject.h"

struct ConstantBuffer
{
    XMMATRIX WVP;
    XMMATRIX World;
};

uint __stdcall MeshObject::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

uint __stdcall MeshObject::Release()
{
    --m_refCount;

    if (m_refCount == 0)
    {
        SAFE_RELEASE(m_pSamplerLinear);

        SAFE_RELEASE(m_pConstantBuffer);
        SAFE_RELEASE(m_pVertexLayout);
        SAFE_RELEASE(m_pPixelShader);
        SAFE_RELEASE(m_pVertexShader);

        SAFE_RELEASE(m_pImmediateContext);
        SAFE_RELEASE(m_pDevice);
        SAFE_RELEASE(m_pRenderer);

        SAFE_RELEASE(m_pCamera);

        delete this;
        return 0;
    }

    return m_refCount;
}

uint __stdcall MeshObject::GetRefCount() const
{
    return m_refCount;
}

bool __stdcall MeshObject::Initialize(IRenderer* pRenderer)
{
    m_position = XMVectorZero();
    m_rotationDegree = XMVectorZero();
    m_scale = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);

    m_world = XMMatrixIdentity();

    pRenderer->AddRef();
    m_pRenderer = pRenderer;

    m_pDevice = (ID3D11Device*)pRenderer->GetD3DDevice();
    m_pImmediateContext = (ID3D11DeviceContext*)pRenderer->GetD3DImmediateContext();

    // Create the sample state
    if (m_pSamplerLinear == nullptr)
    {
        D3D11_SAMPLER_DESC sampDesc;
        memset(&sampDesc, 0, sizeof(sampDesc));
        sampDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        sampDesc.MinLOD = 0;
        sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

        HRESULT hr = m_pDevice->CreateSamplerState(&sampDesc, &m_pSamplerLinear);
        if (FAILED(hr))
        {
            CRASH();
        }
    }

    return true;
}

bool __stdcall MeshObject::CreateMesh(const int includeFlag,
                                      const void* pVertices, const uint vertexSize, const uint numVertices,
                                      const uint16** ppIndices, const uint16* pNumIndices, const uint numIndexBuffers,
                                      const void* pTexCoordsOrNull, const wchar_t** ppTextureFileNamesOrNull,
                                      const wchar_t* pShaderFileName, const char* pVSEntryPoint, const char* pPSEntryPoint)
{
    HRESULT hr;

    ID3DBlob* pVertexShaderBlob = nullptr;
    ID3DBlob* pPixelShaderBlob = nullptr;

    hr = CompileShaderFromFile(pShaderFileName, pVSEntryPoint, "vs_5_0", &pVertexShaderBlob);
    if (FAILED(hr))
    {
        CRASH();
    }

    hr = CompileShaderFromFile(pShaderFileName, pPSEntryPoint, "ps_5_0", &pPixelShaderBlob);
    if (FAILED(hr))
    {
        CRASH();
    }

    hr = m_pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &m_pVertexShader);
    if (FAILED(hr))
    {
        CRASH();
    }

    hr = m_pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &m_pPixelShader);
    if (FAILED(hr))
    {
        CRASH();
    }

    D3D11_INPUT_ELEMENT_DESC layout[5];
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

    if (GET_MASK(includeFlag, UWMESH_INCLUDE_FLAG_TEXTURE))
    {
        layout[2].SemanticName = "TEXCOORD";
        layout[2].SemanticIndex = 0;
        layout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
        layout[2].InputSlot = 1;
        layout[2].AlignedByteOffset = 0;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;
        numLayout = 3;
    }
    else if (GET_MASK(includeFlag, UWMESH_INCLUDE_FLAG_COLOR))
    {
        layout[2].SemanticName = "COLOR";
        layout[2].SemanticIndex = 0;
        layout[2].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        layout[2].InputSlot = 1;
        layout[2].AlignedByteOffset = 0;
        layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[2].InstanceDataStepRate = 0;
        numLayout = 3;
    }

    layout[3].SemanticName = "BLENDWEIGHT";
    layout[3].SemanticIndex = 0;
    layout[3].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    layout[3].InputSlot = 2;
    layout[3].AlignedByteOffset = 0;
    layout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[3].InstanceDataStepRate = 0;

    layout[4].SemanticName = "BLENDINDICES";
    layout[4].SemanticIndex = 0;
    layout[4].Format = DXGI_FORMAT_R32_UINT;
    layout[4].InputSlot = 2;
    layout[4].AlignedByteOffset = 16;
    layout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[4].InstanceDataStepRate = 0;

    numLayout = 5;

    hr = m_pDevice->CreateInputLayout(layout, numLayout, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &m_pVertexLayout);
    if (FAILED(hr))
    {
        CRASH();
    }

    SAFE_RELEASE(pVertexShaderBlob);
    SAFE_RELEASE(pPixelShaderBlob);

    // 버텍스 버퍼 초기화
    if (!m_vertexBuffer.Initialize(m_pRenderer, VERTEX_BUFFER_FLAG_DEFAULT, vertexSize, numVertices))
    {
        CRASH();
    }
    m_vertexBuffer.SetVertex(pVertices, numVertices, vertexSize, 0, VERTEX_BUFFER_FLAG_DEFAULT);

    if (GET_MASK(includeFlag, UWMESH_INCLUDE_FLAG_TEXTURE))
    {
        // 텍스처 버퍼 초기화
        if (!m_textureBuffer.Initialize(m_pRenderer, VERTEX_BUFFER_FLAG_TEXCOORD, sizeof(float) * 2, numVertices))
        {
            CRASH();
        }

        m_textureBuffer.SetVertex(pTexCoordsOrNull, numVertices, sizeof(float) * 2, 0, VERTEX_BUFFER_FLAG_TEXCOORD);

        // 페이스 그룹 초기화
        if (!m_faceGroup.Initialize(m_pRenderer, numIndexBuffers))
        {
            CRASH();
        }
        
        for (uint i = 0; i < numIndexBuffers; ++i)
        {
            m_faceGroup.AddIndexBuffer(ppIndices[i], pNumIndices[i]);
            m_faceGroup.AddTexture(ppTextureFileNamesOrNull[i]);
        }
    }

    // constant 버퍼 생성
    D3D11_BUFFER_DESC bd;
    memset(&bd, 0, sizeof(bd));
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof(ConstantBuffer);
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = m_pDevice->CreateBuffer(&bd, nullptr, &m_pConstantBuffer);
    if (FAILED(hr))
    {
        CRASH();
    }

    m_includeFlag = includeFlag;
    m_vertexSize = vertexSize;

    m_world = XMMatrixIdentity();

    SAFE_RELEASE(pVertexShaderBlob);
    SAFE_RELEASE(pPixelShaderBlob);
    return true;
}

void __stdcall MeshObject::RenderMesh()
{
    ConstantBuffer cb;

    cb.WVP = XMMatrixTranspose(m_world * UW_Matrix44ToXMMatrix(m_pCamera->GetView()) * UW_Matrix44ToXMMatrix(m_pCamera->GetProjection()));
    m_pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    const UINT strides[] = { m_vertexSize, sizeof(float) * 2, sizeof(float) * 4 };
    const UINT offsets[] = { 0, 0, 0 };
    ID3D11Buffer* buffers[] = { m_vertexBuffer.GetBuffer(), m_textureBuffer.GetBuffer() };

    m_pImmediateContext->IASetInputLayout(m_pVertexLayout);
    m_pImmediateContext->IASetVertexBuffers(0, 2, buffers, strides, offsets);

    m_pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    m_pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    m_pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);

    m_pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    const uint numFaceGroups = m_faceGroup.GetNumGroups();
    for (uint i = 0; i < numFaceGroups; ++i)
    {
        const IndexBuffer* pIndexBuffer = m_faceGroup.GetIndexBuffer(i);

        m_pImmediateContext->IASetIndexBuffer(pIndexBuffer->GetBuffer(), DXGI_FORMAT_R16_UINT, 0);

        if (GET_MASK(m_includeFlag, UWMESH_INCLUDE_FLAG_TEXTURE))
        {
            ID3D11ShaderResourceView* pTexture = m_faceGroup.GetTexture(i);
            m_pImmediateContext->PSSetShaderResources(0, 1, &pTexture);
            m_pImmediateContext->PSSetSamplers(0, 1, &m_pSamplerLinear);
        }

        m_pImmediateContext->DrawIndexed((UINT)pIndexBuffer->GetNumIndices(), 0, 0);
    }
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
    pCamera->AddRef();
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