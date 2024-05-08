/*
* UWEngine 코어 타입
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.28
*/

#pragma once

#include "Interfaces.h"

class Camera final : public ICamera
{
public:
    Camera() = default;
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    ~Camera() = default;

    virtual UWMETHOD(vsize)             AddRef() override;
    virtual UWMETHOD(vsize)             Release() override;
    virtual UWMETHOD(vsize)             GetRefCount() const override;

    virtual UWMETHOD(bool)              Initailize(const float fovY, const float aspectRatio, const float nearZ, const float farZ) override;
    virtual UWMETHOD(void)              Update() override;

    virtual UWMETHOD_VECTOR(void)       Translate(const XMVECTOR dist) override;
    virtual UWMETHOD(void)              TranslateX(const float dist) override;
    virtual UWMETHOD(void)              TranslateY(const float dist) override;
    virtual UWMETHOD(void)              TranslateZ(const float dist) override;

    virtual UWMETHOD_VECTOR(void)       Rotate(const XMVECTOR angleDegrees) override;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) override;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) override;
    virtual UWMETHOD(void)              RotateZ(const float angleDegree) override;

    virtual UWMETHOD_VECTOR(XMVECTOR)   GetPosition() const override;
    virtual UWMETHOD_VECTOR(void)       SetPosition(const XMVECTOR position) override;

    virtual UWMETHOD_VECTOR(XMVECTOR)   GetRotation() const override;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const XMVECTOR angleDegrees) override;

    virtual UWMETHOD_VECTOR(XMMATRIX)   GetView() const override;
    virtual UWMETHOD_VECTOR(XMMATRIX)   GetProjection() const override;

private:
    vsize m_refCount = 0;

    XMVECTOR m_up = {};
    XMVECTOR m_at = {};

    XMVECTOR m_position = {};
    XMVECTOR m_rotationDegree = {};

    XMMATRIX m_view = {};
    XMMATRIX m_projection = {};
};

class MeshObject : public IMeshObject
{
public:
    MeshObject() = default;
    MeshObject(const MeshObject&) = delete;
    MeshObject& operator=(const MeshObject&) = delete;
    virtual ~MeshObject() = default;

    virtual UWMETHOD(vsize)             AddRef() override;
    virtual UWMETHOD(vsize)             Release() override;
    virtual UWMETHOD(vsize)             GetRefCount() const override;

    virtual UWMETHOD(bool)              Initialize(IRendererD3D11* pRenderer) override;

    virtual UWMETHOD(bool)              CreateMesh(const int includeFlag,
                                                   const void* pVertices, const uint vertexSize, const uint numVertices,
                                                   const uint16** ppIndices, const uint16* pNumIndices, const uint numIndexBuffers,
                                                   const wchar_t** ppTextureFileNamesOrNull, const uint numTextures,
                                                   const wchar_t* pShaderFileName) override;
    virtual UWMETHOD(void)              RenderMesh() override;

    virtual UWMETHOD_VECTOR(void)       Translate(const XMVECTOR dist) override;
    virtual UWMETHOD(void)              TranslateX(const float dist) override;
    virtual UWMETHOD(void)              TranslateY(const float dist) override;
    virtual UWMETHOD(void)              TranslateZ(const float dist) override;

    virtual UWMETHOD_VECTOR(void)       Rotate(const XMVECTOR angleDegrees) override;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) override;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) override;
    virtual UWMETHOD(void)              RotateZ(const float angleDegree) override;

    virtual UWMETHOD_VECTOR(void)       Scale(const XMVECTOR scale) override;
    virtual UWMETHOD(void)              ScaleX(const float scale) override;
    virtual UWMETHOD(void)              ScaleY(const float scale) override;
    virtual UWMETHOD(void)              ScaleZ(const float scale) override;

    virtual UWMETHOD_VECTOR(XMVECTOR)   GetPosition() const override;
    virtual UWMETHOD_VECTOR(XMVECTOR)   GetRotation() const override;
    virtual UWMETHOD_VECTOR(XMVECTOR)   GetScale() const override;

    virtual UWMETHOD_VECTOR(void)       SetPosition(const XMVECTOR position) override;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const XMVECTOR angleDegrees) override;
    virtual UWMETHOD_VECTOR(void)       SetScale(const XMVECTOR scale) override;

    virtual UWMETHOD(void)              SetCamera(ICamera* pCamera) override;

private:
    UWMETHOD(void)                      updateWorldMatrix();

private:
    vsize                       m_refCount = 0;

    IRendererD3D11*             m_pRenderer = nullptr;
    ID3D11DeviceContext*        m_pImmediateContext = nullptr;

    ID3D11InputLayout*          m_pVertexLayout = nullptr;
    ID3D11VertexShader*         m_pVertexShader = nullptr;
    ID3D11PixelShader*          m_pPixelShader = nullptr;
    ID3D11Buffer*               m_pConstantBuffer = nullptr;

    XMMATRIX                    m_world = {};

    // 모델 데이터
    ID3D11ShaderResourceView**  m_ppTextureResourceViews = nullptr;
    ID3D11SamplerState*         m_pSamplerLinear = nullptr;

    uint                        m_numTextures = 0;
    uint                        m_includeFlag = 0;
    uint                        m_numVertices = 0;
    uint                        m_vertexSize = 0;
    uint                        m_numIndexBuffers = 0;

    ID3D11Buffer*               m_pVertexBuffer = nullptr;
    ID3D11Buffer**              m_ppIndexBuffers = nullptr;
    uint16*                     m_pNumIndices = 0;

    ICamera*                    m_pCamera = nullptr;
    XMVECTOR                    m_position = {};
    XMVECTOR                    m_rotationDegree = {};
    XMVECTOR                    m_scale = {};
};