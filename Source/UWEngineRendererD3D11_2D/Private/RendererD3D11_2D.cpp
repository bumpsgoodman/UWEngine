/*
* RendererD3D11_2D
*
* 작성자: bumpsgoodman
* 작성일: 2023.01.28
*/

#include "Precompiled.h"
#include "UWEngineCommon/Typedef.h"

using namespace DirectX;

class RendererD3D11_2D final : public IRendererD3D11_2D
{
public:
    RendererD3D11_2D() = default;
    RendererD3D11_2D(const RendererD3D11_2D&) = delete;
    RendererD3D11_2D& operator=(const RendererD3D11_2D&) = delete;
    ~RendererD3D11_2D() = default;

    virtual UWMETHOD(vsize)     AddRef() override;
    virtual UWMETHOD(vsize)     Release() override;
    virtual UWMETHOD(vsize)     GetRefCount() const override;

    virtual UWMETHOD(bool)      Initialize(const HWND hWnd) override;

    virtual UWMETHOD(void)      BeginRender() override;
    virtual UWMETHOD(void)      EndRender() override;

    virtual UWMETHOD(float)     GetDeltaTime() const override;
    virtual UWMETHOD(uint)      GetFPS() const override;

    virtual UWMETHOD(void)      CreateCamera(ICamera2D** ppOutCamera) override;
    virtual UWMETHOD(void)      CreateMeshObject(IMeshObject2D** ppOutMeshObject) override;

    virtual UWMETHOD(void*)     Private_GetD3dDevice() const override;
    virtual UWMETHOD(void*)     Private_GetImmediateContext() const override;

private:
    vsize                       m_refCount = 0;
    HWND                        m_hWnd = nullptr;

    bool                        m_bDebug = true;

    D3D_DRIVER_TYPE             m_driver = D3D_DRIVER_TYPE_HARDWARE;
    D3D_FEATURE_LEVEL           m_featureLevel = D3D_FEATURE_LEVEL_11_0;
    ID3D11Device*               m_pDevice = nullptr;
    ID3D11DeviceContext*        m_pImmediateContext = nullptr;
    IDXGISwapChain*             m_pSwapChain = nullptr;
    ID3D11RenderTargetView*     m_pRenderTargetView = nullptr;
    ID3D11Texture2D*            m_pDepthStencil = nullptr;
    ID3D11DepthStencilView*     m_pDepthStencilView = nullptr;

    Timer                       m_frameTimer = {};
    float                       m_startRenderingTime = 0.0f;
    float                       m_deltaTime = 0.0f;
};

UWMETHOD(vsize) RendererD3D11_2D::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

UWMETHOD(vsize) RendererD3D11_2D::Release()
{
    --m_refCount;
    if (m_refCount == 0)
    {
        if (m_pImmediateContext != nullptr)
        {
            m_pImmediateContext->ClearState();
        }

        SAFE_RELEASE(m_pRenderTargetView);
        SAFE_RELEASE(m_pDepthStencil);
        SAFE_RELEASE(m_pDepthStencilView);
        SAFE_RELEASE(m_pSwapChain);
        SAFE_RELEASE(m_pImmediateContext);
        SAFE_RELEASE(m_pDevice);

        delete this;
        return 0;
    }

    return m_refCount;
}

UWMETHOD(vsize) RendererD3D11_2D::GetRefCount() const
{
    return m_refCount;
}

UWMETHOD(bool) RendererD3D11_2D::Initialize(const HWND hWnd)
{
    ASSERT(hWnd != nullptr, "hWnd == nullptr");

    bool bResult = false;
    HRESULT hr;

    m_hWnd = hWnd;

    TimerInit(&m_frameTimer);
    m_startRenderingTime = TimerGetTime(&m_frameTimer);

    static const D3D_DRIVER_TYPE DRIVERS[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE
    };
    static const DWORD DRIVER_COUNT = ARRAYSIZE(DRIVERS);

    static const D3D_FEATURE_LEVEL FEATURE_LEVELS[] =
    {
        D3D_FEATURE_LEVEL_11_0
    };
    static const DWORD FEATURE_LEVEL_COUNT = ARRAYSIZE(FEATURE_LEVELS);

    DWORD createDeviceFlag = 0;
    if (m_bDebug)
    {
        createDeviceFlag = D3D11_CREATE_DEVICE_DEBUG;
    }

    for (vsize i = 0; i < DRIVER_COUNT; ++i)
    {
        const D3D_DRIVER_TYPE driver = DRIVERS[i];
        hr = D3D11CreateDevice(nullptr, driver, nullptr, createDeviceFlag,
                               FEATURE_LEVELS, FEATURE_LEVEL_COUNT, D3D11_SDK_VERSION,
                               &m_pDevice, &m_featureLevel, &m_pImmediateContext);
        if (SUCCEEDED(hr))
        {
            break;
        }
    }

    if (FAILED(hr))
    {
        goto lb_return;
    }

    // IDXGIFactory 얻기
    IDXGIFactory* pDxgiFactory = nullptr;
    {
        IDXGIDevice* pDxgiDevice = nullptr;
        m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDxgiDevice);

        IDXGIAdapter* pAdapter = nullptr;
        pDxgiDevice->GetAdapter(&pAdapter);
        SAFE_RELEASE(pDxgiDevice);

        pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDxgiFactory);
        SAFE_RELEASE(pAdapter);
    }

    RECT windowRect;
    GetClientRect(hWnd, &windowRect);

    const UINT windowWidth = windowRect.right - windowRect.left;
    const UINT windowHeight = windowRect.bottom - windowRect.top;

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferDesc.Width = windowWidth;
    sd.BufferDesc.Height = windowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1;
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    sd.BufferCount = 1;

    hr = pDxgiFactory->CreateSwapChain(m_pDevice, &sd, &m_pSwapChain);
    SAFE_RELEASE(pDxgiFactory);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    ID3D11Texture2D* pBack = nullptr;
    hr = m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBack);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    m_pDevice->CreateRenderTargetView(pBack, nullptr, &m_pRenderTargetView);
    SAFE_RELEASE(pBack);

    // Create depth stencil texture
    D3D11_TEXTURE2D_DESC descDepth;
    memset(&descDepth, 0, sizeof(descDepth));
    descDepth.Width = windowWidth;
    descDepth.Height = windowHeight;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = m_pDevice->CreateTexture2D(&descDepth, nullptr, &m_pDepthStencil);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    // Create the depth stencil view
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    memset(&descDSV, 0, sizeof(descDSV));
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = m_pDevice->CreateDepthStencilView(m_pDepthStencil, &descDSV, &m_pDepthStencilView);
    if (FAILED(hr))
    {
        goto lb_return;
    }

    m_pImmediateContext->OMSetRenderTargets(1, &m_pRenderTargetView, nullptr);

    D3D11_VIEWPORT vp;
    ZeroMemory(&vp, sizeof(vp));
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    vp.Width = (FLOAT)windowWidth;
    vp.Height = (FLOAT)windowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    m_pImmediateContext->RSSetViewports(1, &vp);

    bResult = true;

lb_return:
    return bResult;
}

UWMETHOD(void) RendererD3D11_2D::BeginRender()
{
    const FLOAT backColor[] = { 0.18f, 0.38f, 0.37f, 1.0f };

    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, backColor);

    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

UWMETHOD(void) RendererD3D11_2D::EndRender()
{
    m_pSwapChain->Present(0, 0);

    const float endRenderingTime = TimerGetTime(&m_frameTimer);
    m_deltaTime = endRenderingTime - m_startRenderingTime;

    m_startRenderingTime = endRenderingTime;
}

UWMETHOD(float) RendererD3D11_2D::GetDeltaTime() const
{
    return m_deltaTime;
}

UWMETHOD(uint) RendererD3D11_2D::GetFPS() const
{
    return (uint)(1000.0f / m_deltaTime);
}

UWMETHOD(void) RendererD3D11_2D::CreateCamera(ICamera2D** ppOutCamera)
{
    ASSERT(ppOutCamera != nullptr, "ppOutCamera == nullptr");

    Camera2D* pCamera = new Camera2D;
    pCamera->AddRef();

    *ppOutCamera = pCamera;
}

UWMETHOD(void) RendererD3D11_2D::CreateMeshObject(IMeshObject2D** ppOutMeshObject)
{
    ASSERT(ppOutMeshObject != nullptr, "ppOutMeshObject == nullptr");

    MeshObject2D* pMeshObject = new MeshObject2D;
    pMeshObject->AddRef();

    pMeshObject->Initialize(this);

    *ppOutMeshObject = pMeshObject;
}

UWMETHOD(void*) RendererD3D11_2D::Private_GetD3dDevice() const
{
    m_pDevice->AddRef();
    return m_pDevice;
}

UWMETHOD(void*) RendererD3D11_2D::Private_GetImmediateContext() const
{
    m_pImmediateContext->AddRef();
    return m_pImmediateContext;
}

UWMETHOD(bool) CreateDllInstance(void** ppOutInstance)
{
    ASSERT(ppOutInstance != nullptr, "ppOutInstance == nullptr");

    RendererD3D11_2D* pRenderer = new RendererD3D11_2D;
    pRenderer->AddRef();

    *ppOutInstance = pRenderer;

    return true;
}