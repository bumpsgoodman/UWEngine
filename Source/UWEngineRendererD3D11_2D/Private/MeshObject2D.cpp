/*
* 2D Mesh Object
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.06
*/

#include "Precompiled.h"
#include "UWEngineCommon/Typedef.h"

struct ConstantBuffer
{
    Matrix44 WVP;
};

UWMETHOD(vsize) MeshObject2D::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

UWMETHOD(vsize) MeshObject2D::Release()
{
    --m_refCount;

    if (m_refCount == 0)
    {
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

UWMETHOD(vsize) MeshObject2D::GetRefCount() const
{
    return m_refCount;
}

UWMETHOD(bool) MeshObject2D::Initialize(IRendererD3D11_2D* pRenderer)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");

    m_position = Vector4Zero();
    m_rotationDegree = Vector4Zero();
    m_scale = Vector4One();

    m_world = Matrix44Identity();

    pRenderer->AddRef();
    m_pRenderer = pRenderer;

    m_pImmediateContext = (ID3D11DeviceContext*)pRenderer->Private_GetImmediateContext();

    m_renderType = MESH_RENDER_TYPE_WIREFRAME;

    return true;
}

UWMETHOD(bool) MeshObject2D::CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                                        const uint16* pIndices, const uint numIndices,
                                        const ColorF wireframeColor,
                                        const wchar_t* pShaderFileName)
{
    ASSERT(pVertices != nullptr, "pVertices == nullptr");
    ASSERT(pIndices != nullptr, "pIndices == nullptr");
    ASSERT(pShaderFileName != nullptr, "pShaderFileName == nullptr");

    bool bResult = false;
    HRESULT hr;

    ID3D11Device* pDevice = (ID3D11Device*)m_pRenderer->Private_GetD3dDevice();
    ID3DBlob* pVertexShaderBlob = nullptr;
    ID3DBlob* pPixelShaderBlob = nullptr;
    ID3DBlob* pErrorBlob = nullptr;

#if defined(_DEBUG)
    const uint compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    const uint compileFlags = 0;
#endif

    hr = D3DCompileFromFile(pShaderFileName, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &pVertexShaderBlob, &pErrorBlob);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = D3DCompileFromFile(pShaderFileName, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pPixelShaderBlob, nullptr);
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

    D3D11_INPUT_ELEMENT_DESC layout[2];

    layout[0].SemanticName = "POSITION";
    layout[0].SemanticIndex = 0;
    layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
    layout[0].InputSlot = 0;
    layout[0].AlignedByteOffset = 0;
    layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    layout[0].InstanceDataStepRate = 0;

    switch (m_renderType)
    {
    case MESH_RENDER_TYPE_WIREFRAME:
        layout[1].SemanticName = "COLOR";
        layout[1].SemanticIndex = 0;
        layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        layout[1].InputSlot = 0;
        layout[1].AlignedByteOffset = 12;
        layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[1].InstanceDataStepRate = 0;
        break;
    case MESH_RENDER_TYPE_COLOR:
        layout[1].SemanticName = "COLOR";
        layout[1].SemanticIndex = 0;
        layout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        layout[1].InputSlot = 0;
        layout[1].AlignedByteOffset = 12;
        layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[1].InstanceDataStepRate = 0;
        break;
    case MESH_RENDER_TYPE_TEXTURE:
        layout[1].SemanticName = "TEXCOORD";
        layout[1].SemanticIndex = 0;
        layout[1].Format = DXGI_FORMAT_R32G32_FLOAT;
        layout[1].InputSlot = 0;
        layout[1].AlignedByteOffset = 12;
        layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
        layout[1].InstanceDataStepRate = 0;
        break;
    default:
        ASSERT(false, "Invalid render type");
        break;
    }
    const vsize numLayout = ARRAYSIZE(layout);

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

    m_numVertices = numVertices;
    m_numIndices = numIndices;
    m_vertexSize = vertexSize;

    m_world = Matrix44Identity();

    bResult = true;

lb_return:
    SAFE_RELEASE(pDevice);
    return bResult;
}

UWMETHOD(void) MeshObject2D::RenderMesh()
{
    ID3D11DeviceContext* pImmediateContext = (ID3D11DeviceContext*)m_pRenderer->Private_GetImmediateContext();

    ConstantBuffer cb;
    cb.WVP = Matrix44Transpose(m_world * m_pCamera->GetView() * m_pCamera->GetProjection());
    pImmediateContext->UpdateSubresource(m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    UINT stride = m_vertexSize;
    UINT offset = 0;

    pImmediateContext->IASetInputLayout(m_pVertexLayout);
    pImmediateContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);
    pImmediateContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
    pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    pImmediateContext->VSSetShader(m_pVertexShader, nullptr, 0);
    pImmediateContext->VSSetConstantBuffers(0, 1, &m_pConstantBuffer);
    pImmediateContext->PSSetShader(m_pPixelShader, nullptr, 0);

    pImmediateContext->DrawIndexed(m_numIndices, 0, 0);

    SAFE_RELEASE(pImmediateContext);
}

UWMETHOD_VECTOR(void) MeshObject2D::Translate(const Vector4 dist)
{
    m_position += dist;
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject2D::TranslateX(const float dist)
{
    m_position.X += dist;
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject2D::TranslateY(const float dist)
{
    m_position.Y += dist;
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject2D::Rotate(const Vector4 angleDegree)
{
    m_rotationDegree += angleDegree;
    m_rotationDegree = Vector4Wrap(angleDegree, Vector4Zero(), Vector4Set(360.0f, 360.0f, 360.0f, 0.0f));
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject2D::RotateX(const float angleDegree)
{
    m_rotationDegree.X += angleDegree;
    m_rotationDegree.X = Wrap(m_rotationDegree.X, 0.0f, 360.0f);
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject2D::RotateY(const float angleDegree)
{
    m_rotationDegree.Y += angleDegree;
    m_rotationDegree.Y = Wrap(m_rotationDegree.Y, 0.0f, 360.0f);
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject2D::Scale(const Vector4 scale)
{
    m_scale += scale;
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject2D::ScaleX(const float scale)
{
    m_scale.X += scale;
    updateWorldMatrix();
}

UWMETHOD(void) MeshObject2D::ScaleY(const float scale)
{
    m_scale.Y += scale;
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject2D::SetPosition(const Vector4 position)
{
    m_position = position;
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject2D::SetRotation(const Vector4 angleDegrees)
{
    m_rotationDegree = Vector4DegreeToRad(angleDegrees);
    updateWorldMatrix();
}

UWMETHOD_VECTOR(void) MeshObject2D::SetScale(const Vector4 scale)
{
    m_scale = scale;
    updateWorldMatrix();
}

UWMETHOD_VECTOR(Vector4) MeshObject2D::GetPosition() const
{
    return m_position;
}

UWMETHOD_VECTOR(Vector4) MeshObject2D::GetRotation() const
{
    return m_rotationDegree;
}

UWMETHOD_VECTOR(Vector4) MeshObject2D::GetScale() const
{
    return m_scale;
}

UWMETHOD(void) MeshObject2D::SetCamera(ICamera2D* pCamera)
{
    ASSERT(pCamera != nullptr, "pCamera == nullptr");
    m_pCamera = pCamera;
}

UWMETHOD(MESH_RENDER_TYPE) MeshObject2D::GetRenderType() const
{
    return m_renderType;
}

UWMETHOD(void) MeshObject2D::SetRenderType(const MESH_RENDER_TYPE type)
{
    if (type >= NUM_MESH_RENDER_TYPE)
    {
        m_renderType = MESH_RENDER_TYPE_WIREFRAME;
    }
    else
    {
        m_renderType = type;
    }
}

UWMETHOD(void) MeshObject2D::updateWorldMatrix()
{
    m_world = Matrix44WorldFromVector(m_position, Vector4DegreeToRad(m_rotationDegree), m_scale);
}