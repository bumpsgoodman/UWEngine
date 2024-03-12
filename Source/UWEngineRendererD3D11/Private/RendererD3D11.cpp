/*
* RendererD3D11
*
* 작성자: bumpsgoodman
* 작성일: 2023.01.28
*/

#include "Precompiled.h"
#include "IRendererD3D11.h"

using namespace DirectX;

extern void __stdcall Private_CreateMeshObject(IMeshObject** ppOutMeshObject);
void __stdcall Private_CreateCamera(ICamera** ppOutCamera);

class RendererD3D11 final : public IRendererD3D11
{
public:
    RendererD3D11() = default;
    RendererD3D11(const RendererD3D11&) = delete;
    RendererD3D11& operator=(const RendererD3D11&) = delete;
    RendererD3D11(RendererD3D11&&) = default;
    RendererD3D11& operator=(RendererD3D11&&) = default;
    ~RendererD3D11() = default;

    virtual vsize __stdcall AddRef() override;
    virtual vsize __stdcall Release() override;
    virtual vsize __stdcall GetRefCount() const override;

    virtual bool __stdcall Initialize(const HWND hWnd) override;

    virtual void __stdcall BeginRender() override;
    virtual void __stdcall EndRender() override;

    virtual IMeshObject* __stdcall CreateMeshObject() override;
    virtual ICamera* __stdcall CreateCamera() override;

    virtual float __stdcall GetDeltaTime() const override;
    virtual uint __stdcall GetFPS() const override;

    virtual void* __stdcall Private_GetD3dDevice() const override;
    virtual void* __stdcall Private_GetImmediateContext() const override;

private:
    vsize                   m_refCount = 0;
    HWND                    m_hWnd = nullptr;

    bool                    m_bDebug = true;

    D3D_DRIVER_TYPE         m_driver = D3D_DRIVER_TYPE_HARDWARE;
    D3D_FEATURE_LEVEL       m_featureLevel = D3D_FEATURE_LEVEL_11_0;
    ID3D11Device*           m_pDevice = nullptr;
    ID3D11DeviceContext*    m_pImmediateContext = nullptr;
    IDXGISwapChain*         m_pSwapChain = nullptr;
    ID3D11RenderTargetView* m_pRenderTargetView = nullptr;
    ID3D11Texture2D*        m_pDepthStencil = nullptr;
    ID3D11DepthStencilView* m_pDepthStencilView = nullptr;
    
    Timer m_frameTimer = {};
    float m_startRenderingTime = 0.0f;
    float m_deltaTime = 0.0f;
};

vsize __stdcall RendererD3D11::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

vsize __stdcall RendererD3D11::Release()
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

vsize __stdcall RendererD3D11::GetRefCount() const
{
    return m_refCount;
}

bool __stdcall RendererD3D11::Initialize(const HWND hWnd)
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

void __stdcall RendererD3D11::BeginRender()
{
    const FLOAT backColor[] = { 0.18f, 0.38f, 0.37f, 1.0f };

    m_pImmediateContext->ClearRenderTargetView(m_pRenderTargetView, backColor);

    m_pImmediateContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
}

void __stdcall RendererD3D11::EndRender()
{
    m_pSwapChain->Present(0, 0);

    const float endRenderingTime = TimerGetTime(&m_frameTimer);
    m_deltaTime = endRenderingTime - m_startRenderingTime;

    m_startRenderingTime = endRenderingTime;
}

IMeshObject* __stdcall RendererD3D11::CreateMeshObject()
{
    IMeshObject* pMeshObject;
    Private_CreateMeshObject(&pMeshObject);

    pMeshObject->Initialize(this);
    return pMeshObject;
}

ICamera* __stdcall RendererD3D11::CreateCamera()
{
    ICamera* pCamera;
    Private_CreateCamera(&pCamera);

    return pCamera;
}

float __stdcall RendererD3D11::GetDeltaTime() const
{
    return m_deltaTime;
}

uint __stdcall RendererD3D11::GetFPS() const
{
    return (uint)(1000.0f / m_deltaTime);
}

void* __stdcall RendererD3D11::Private_GetD3dDevice() const
{
    m_pDevice->AddRef();
    return m_pDevice;
}

void* __stdcall RendererD3D11::Private_GetImmediateContext() const
{
    m_pImmediateContext->AddRef();
    return m_pImmediateContext;
}

bool __stdcall CreateDllInstance(void** ppOutInstance)
{
    ASSERT(ppOutInstance != nullptr, "ppOutInstance == nullptr");

    RendererD3D11* pRenderer = new RendererD3D11;
    pRenderer->AddRef();

    *ppOutInstance = pRenderer;

    return true;
}