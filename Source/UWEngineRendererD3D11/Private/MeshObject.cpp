/*
* MeshObject
*
* 작성자: bumpsgoodman
* 작성일: 2023.02.03
*/

#include "Precompiled.h"
#include "IMeshObject.h"
#include "IRendererD3D11.h"

struct ConstantBuffer
{
    Matrix44 WVP;
};

ALIGN16 class MeshObject : public IMeshObject
{
public:
    MeshObject() = default;
    MeshObject(const MeshObject&) = delete;
    MeshObject& operator=(const MeshObject&) = delete;
    ~MeshObject() = default;

    virtual vsize __stdcall AddRef() override;
    virtual vsize __stdcall Release() override;
    virtual vsize __stdcall GetRefCount() const override;

    virtual bool __stdcall Initialize(IRendererD3D11* pRenderer) override;
    virtual bool __stdcall CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                                      const uint16* pIndices, const uint numIndices,
                                      const wchar_t* pShaderFileName) override;
    virtual void __stdcall RenderMesh() override;

    virtual void __stdcall Translate(const Vector4 dist) override;
    virtual void __stdcall TranslateX(const float dist) override;
    virtual void __stdcall TranslateY(const float dist) override;
    virtual void __stdcall TranslateZ(const float dist) override;

    virtual void __stdcall Rotate(const Vector4 angleDegrees) override;
    virtual void __stdcall RotateX(const float angleDegree) override;
    virtual void __stdcall RotateY(const float angleDegree) override;
    virtual void __stdcall RotateZ(const float angleDegree) override;

    virtual void __stdcall Scale(const Vector4 scale) override;
    virtual void __stdcall ScaleX(const float scale) override;
    virtual void __stdcall ScaleY(const float scale) override;
    virtual void __stdcall ScaleZ(const float scale) override;

    virtual void __stdcall SetPosition(const Vector4 position) override;
    virtual void __stdcall SetRotation(const Vector4 angleDegrees) override;
    virtual void __stdcall SetScale(const Vector4 scale) override;

    virtual Vector4 __stdcall GetPosition() const override;
    virtual Vector4 __stdcall GetRotation() const override;
    virtual Vector4 __stdcall GetScale() const override;

    virtual void __stdcall SetCamera(ICamera* pCamera) override;

private:
    void __stdcall updateWorldMatrix();

private:
    vsize m_refCount = 0;

    IRendererD3D11* m_pRenderer = nullptr;
    ID3D11InputLayout* m_pVertexLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pConstantBuffer = nullptr;

    Matrix44 m_world = {};

    uint m_numVertices = 0;
    uint m_numIndices = 0;
    uint m_vertexSize = 0;

    ICamera* m_pCamera = nullptr;
    Vector4 m_position = {};
    Vector4 m_rotationDegree = {};
    Vector4 m_scale = {};
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
        SAFE_RELEASE(m_pConstantBuffer);
        SAFE_RELEASE(m_pIndexBuffer);
        SAFE_RELEASE(m_pVertexBuffer);
        SAFE_RELEASE(m_pVertexLayout);
        SAFE_RELEASE(m_pPixelShader);
        SAFE_RELEASE(m_pVertexShader);
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

bool __stdcall MeshObject::Initialize(IRendererD3D11* pRenderer)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");

    m_position = Vector4Zero();
    m_rotationDegree = Vector4Zero();
    m_scale = Vector4One();

    m_world = Matrix44Identity();

    pRenderer->AddRef();
    m_pRenderer = pRenderer;

    return true;
}

bool __stdcall MeshObject::CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                                      const uint16* pIndices, const uint numIndices,
                                      const wchar_t* pShaderFileName)
{
    ASSERT(pVertices != nullptr, "pVertices == nullptr");
    ASSERT(pIndices != nullptr, "pIndices == nullptr");
    ASSERT(pShaderFileName != nullptr, "pShaderFileName == nullptr");

    bool bResult = false;
    HRESULT hr;

    ID3D11Device* pDevice = (ID3D11Device*)m_pRenderer->Private_GetD3dDevice();
    ID3DBlob* pVertexShader = nullptr;
    ID3DBlob* pPixelShader = nullptr;

#if defined(_DEBUG)
    uint compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    uint compileFlags = 0;
#endif

    hr = D3DCompileFromFile(pShaderFileName, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &pVertexShader, nullptr);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = D3DCompileFromFile(pShaderFileName, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pPixelShader, nullptr);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = pDevice->CreateVertexShader(pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), nullptr, &m_pVertexShader);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    hr = pDevice->CreatePixelShader(pPixelShader->GetBufferPointer(), pPixelShader->GetBufferSize(), nullptr, &m_pPixelShader);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
    const vsize numLayout = ARRAYSIZE(layout);

    hr = pDevice->CreateInputLayout(layout, numLayout, pVertexShader->GetBufferPointer(), pVertexShader->GetBufferSize(), &m_pVertexLayout);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    SAFE_RELEASE(pVertexShader);
    SAFE_RELEASE(pPixelShader);

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

    // Initialize the world matrix
    m_world = Matrix44Identity();

    bResult = true;

lb_return:
    SAFE_RELEASE(pDevice);
    return bResult;
}

void __stdcall MeshObject::RenderMesh()
{
    ID3D11DeviceContext* pImmediateContext = (ID3D11DeviceContext*)m_pRenderer->Private_GetImmediateContext();

#if 0
    static float t = 0.0f;
    static ULONGLONG timeStart = 0;
    ULONGLONG timeCur = GetTickCount64();
    if (timeStart == 0)
        timeStart = timeCur;
    t = (timeCur - timeStart) / 1000.0f;

    m_world = XMMatrixRotationY(t);
#endif

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

void __stdcall MeshObject::Translate(const Vector4 dist)
{
    m_position += dist;
    updateWorldMatrix();
}

void __stdcall MeshObject::TranslateX(const float dist)
{
    m_position.X += dist;
    updateWorldMatrix();
}

void __stdcall MeshObject::TranslateY(const float dist)
{
    m_position.Y += dist;
    updateWorldMatrix();
}

void __stdcall MeshObject::TranslateZ(const float dist)
{
    m_position.Z += dist;
    updateWorldMatrix();
}

void __stdcall MeshObject::Rotate(const Vector4 angleDegree)
{
    m_rotationDegree += angleDegree;
    m_rotationDegree = Vector4Wrap(angleDegree, Vector4Zero(), Vector4Set(360.0f, 360.0f, 360.0f, 0.0f));
    updateWorldMatrix();
}

void __stdcall MeshObject::RotateX(const float angleDegree)
{
    m_rotationDegree.X += angleDegree;
    m_rotationDegree.X = Wrap(m_rotationDegree.X, 0.0f, 360.0f);
    updateWorldMatrix();
}

void __stdcall MeshObject::RotateY(const float angleDegree)
{
    m_rotationDegree.Y += angleDegree;
    m_rotationDegree.Y = Wrap(m_rotationDegree.Y, 0.0f, 360.0f);
    updateWorldMatrix();
}

void __stdcall MeshObject::RotateZ(const float angleDegree)
{
    m_rotationDegree.Z += angleDegree;
    m_rotationDegree.Z = Wrap(m_rotationDegree.Z, 0.0f, 360.0f);
    updateWorldMatrix();
}

void __stdcall MeshObject::Scale(const Vector4 scale)
{
    m_scale += scale;
    updateWorldMatrix();
}

void __stdcall MeshObject::ScaleX(const float scale)
{
    m_scale.X += scale;
    updateWorldMatrix();
}

void __stdcall MeshObject::ScaleY(const float scale)
{
    m_scale.Y += scale;
    updateWorldMatrix();
}

void __stdcall MeshObject::ScaleZ(const float scale)
{
    m_scale.Z += scale;
    updateWorldMatrix();
}

void __stdcall MeshObject::SetPosition(const Vector4 position)
{
    m_position = position;
    updateWorldMatrix();
}

void __stdcall MeshObject::SetRotation(const Vector4 angleDegrees)
{
    m_rotationDegree = Vector4DegreeToRad(angleDegrees);
    updateWorldMatrix();
}

void __stdcall MeshObject::SetScale(const Vector4 scale)
{
    m_scale = scale;
    updateWorldMatrix();
}

Vector4 __stdcall MeshObject::GetPosition() const
{
    return m_position;
}

Vector4 __stdcall MeshObject::GetRotation() const
{
    return m_rotationDegree;
}

Vector4 __stdcall MeshObject::GetScale() const
{
    return m_scale;
}

void __stdcall MeshObject::SetCamera(ICamera* pCamera)
{
    ASSERT(pCamera != nullptr, "pCamera == nullptr");
    m_pCamera = pCamera;
}

void __stdcall Private_CreateMeshObject(IMeshObject** ppOutMeshObject)
{
    ASSERT(ppOutMeshObject != nullptr, "ppOutMeshObject == nullptr");

    MeshObject* pMeshObject = new MeshObject;
    pMeshObject->AddRef();

    *ppOutMeshObject = pMeshObject;
}

void __stdcall MeshObject::updateWorldMatrix()
{
    m_world = Matrix44WorldFromVector(m_position, Vector4DegreeToRad(m_rotationDegree), m_scale);
}