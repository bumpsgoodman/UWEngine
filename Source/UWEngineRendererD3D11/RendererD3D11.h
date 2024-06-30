/*
* RendererD3D11
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.11
*/

#pragma once

#include "UWEngineCommon/Interfaces/IRenderer.h"

class RendererD3D11 final : public IRenderer
{
public:
    RendererD3D11() = default;
    RendererD3D11(const RendererD3D11&) = delete;
    RendererD3D11& operator=(const RendererD3D11&) = delete;
    ~RendererD3D11() = default;

    virtual uint    __stdcall   AddRef() override;
    virtual uint    __stdcall   Release() override;
    virtual uint    __stdcall   GetRefCount() const override;

    virtual bool    __stdcall   Initialize(const HWND hWnd) override;

    virtual void    __stdcall   BeginRender() override;
    virtual void    __stdcall   EndRender() override;

    virtual uint    __stdcall   GetWindowWidth() const override;
    virtual uint    __stdcall   GetWindowHeight() const override;

    virtual float   __stdcall   GetDeltaTime() const override;
    virtual uint    __stdcall   GetFPS() const override;

    virtual void    __stdcall   CreateCamera(ICamera** ppOutCamera) override;
    virtual void    __stdcall   CreateMeshObject(IMeshObject** ppOutMeshObject) override;

    virtual uint    __stdcall   GetRenderMode() const override;
    virtual void    __stdcall   SetRenderMode(const uint mode) override;

    virtual void*   __stdcall   GetD3DDevice() const override;
    virtual void*   __stdcall   GetD3DImmediateContext() const override;

private:
    uint                        m_refCount = 0;
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

    ID3D11RasterizerState*      m_pWireframeState = nullptr;
    ID3D11RasterizerState*      m_pSolidState = nullptr;
    bool                        m_renderMode = false;

    uint                        m_windowWidth = 0;
    uint                        m_windowHeight = 0;

    Timer                       m_frameTimer = {};
    float                       m_startRenderingTime = 0.0f;
    float                       m_deltaTime = 0.0f;
};