/*
* MeshObject
*
* 작성자: bumpsgoodman
* 작성일: 2023.02.03
*/

#include "Precompiled.h"
#include "IMeshObject.h"
#include "IRendererD3D11.h"

using namespace DirectX;

struct ConstantBuffer
{
    XMMATRIX mWorld;
    XMMATRIX mView;
    XMMATRIX mProjection;
};

class MeshObject : public IMeshObject
{
public:
    MeshObject() = default;
    MeshObject(const MeshObject&) = delete;
    MeshObject& operator=(const MeshObject&) = delete;
    MeshObject(MeshObject&&) = default;
    MeshObject& operator=(MeshObject&&) = default;
    ~MeshObject() = default;

    virtual size_t __stdcall AddRef() override;
    virtual size_t __stdcall Release() override;
    virtual size_t __stdcall GetRefCount() const override;

    virtual bool __stdcall Initialize(IRendererD3D11* pRenderer) override;
    virtual bool __stdcall CreateMesh(const void* pVertices, const uint_t vertexSize, const uint_t numVertices,
                                      const uint16_t* pIndices, const uint_t numIndices,
                                      const wchar_t* pShaderFileName) override;
    virtual void __stdcall RenderMesh() override;

    virtual void __stdcall SetCamera(ICamera* pCamera) override;

private:
    size_t m_refCount = 0;

    IRendererD3D11* m_pRenderer = nullptr;
    ID3D11InputLayout* m_pVertexLayout = nullptr;
    ID3D11VertexShader* m_pVertexShader = nullptr;
    ID3D11PixelShader* m_pPixelShader = nullptr;
    ID3D11Buffer* m_pVertexBuffer = nullptr;
    ID3D11Buffer* m_pIndexBuffer = nullptr;
    ID3D11Buffer* m_pConstantBuffer = nullptr;

    XMMATRIX m_world = {};

    uint_t m_numVertices = 0;
    uint_t m_numIndices = 0;
    uint_t m_vertexSize = 0;

    ICamera* m_pCamera = nullptr;
};

size_t __stdcall MeshObject::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

size_t __stdcall MeshObject::Release()
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

size_t __stdcall MeshObject::GetRefCount() const
{
    return m_refCount;
}

bool __stdcall MeshObject::Initialize(IRendererD3D11* pRenderer)
{
    ASSERT(pRenderer != nullptr, "pRenderer == nullptr");

    pRenderer->AddRef();
    m_pRenderer = pRenderer;

    return true;
}

bool __stdcall MeshObject::CreateMesh(const void* pVertices, const uint_t vertexSize, const uint_t numVertices,
                                      const uint16_t* pIndices, const uint_t numIndices,
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
    uint_t compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    uint_t compileFlags = 0;
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
    const size_t numLayout = ARRAYSIZE(layout);

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
    bd.ByteWidth = sizeof(uint16_t) * numIndices;
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
    m_world = XMMatrixIdentity();

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
    cb.mWorld = XMMatrixTranspose(m_world);
    cb.mView = XMMatrixTranspose(m_pCamera->GetView());
    cb.mProjection = XMMatrixTranspose(m_pCamera->GetProjection());
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