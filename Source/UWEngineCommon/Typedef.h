/*
* UWEngine 공용 구조체/클래스
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.06
*/

#pragma once

#include "Interfaces.h"

class Camera2D final : public ICamera2D
{
public:
    Camera2D() = default;
    Camera2D(const Camera2D&) = delete;
    Camera2D& operator=(const Camera2D&) = delete;
    ~Camera2D() = default;

    virtual UWMETHOD(vsize)             AddRef() override;
    virtual UWMETHOD(vsize)             Release() override;
    virtual UWMETHOD(vsize)             GetRefCount() const override;

    virtual UWMETHOD(bool)              Initailize() override;
    virtual UWMETHOD(void)              Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ) override;

    virtual UWMETHOD_VECTOR(void)       Translate(const Vector4 dist) override;
    virtual UWMETHOD(void)              TranslateX(const float dist) override;
    virtual UWMETHOD(void)              TranslateY(const float dist) override;

    virtual UWMETHOD_VECTOR(void)       Rotate(const Vector4 angleDegrees) override;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) override;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) override;

    virtual UWMETHOD_VECTOR(Vector4)    GetRotation() const override;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const Vector4 angleDegrees) override;

    virtual UWMETHOD_VECTOR(Vector4)    GetEye() const override;
    virtual UWMETHOD_VECTOR(Vector4)    GetAt() const override;
    virtual UWMETHOD_VECTOR(Vector4)    GetUp() const override;

    virtual UWMETHOD_VECTOR(void)       SetEye(const Vector4 v) override;
    virtual UWMETHOD_VECTOR(void)       SetAt(const Vector4 v) override;
    virtual UWMETHOD_VECTOR(void)       SetUp(const Vector4 v) override;

    virtual UWMETHOD_VECTOR(Matrix44)   GetView() const override;
    virtual UWMETHOD_VECTOR(Matrix44)   GetProjection() const override;

private:
    vsize m_refCount = 0;

    Vector4 m_eye = {};
    Vector4 m_at = {};
    Vector4 m_up = {};
    Vector4 m_vView = {};

    Vector4 m_rotationDegree = {};

    Matrix44 m_view = {};
    Matrix44 m_projection = {};
};

class MeshObject2D : public IMeshObject2D
{
public:
    MeshObject2D() = default;
    MeshObject2D(const MeshObject2D&) = delete;
    MeshObject2D& operator=(const MeshObject2D&) = delete;
    virtual ~MeshObject2D() = default;

    virtual UWMETHOD(vsize)             AddRef() override;
    virtual UWMETHOD(vsize)             Release() override;
    virtual UWMETHOD(vsize)             GetRefCount() const override;

    virtual UWMETHOD(bool)              Initialize(IRendererD3D11_2D* pRenderer) override;

    virtual UWMETHOD(bool)              CreateMesh(const void* pVertices, const uint vertexSize, const uint numVertices,
                                                   const uint16* pIndices, const uint numIndices,
                                                   const ColorF wireframeColor,
                                                   const wchar_t* pShaderFileName) override;
    virtual UWMETHOD(void)              RenderMesh() override;

    virtual UWMETHOD_VECTOR(void)       Translate(const Vector4 dist) override;
    virtual UWMETHOD(void)              TranslateX(const float dist) override;
    virtual UWMETHOD(void)              TranslateY(const float dist) override;

    virtual UWMETHOD_VECTOR(void)       Rotate(const Vector4 angleDegrees) override;
    virtual UWMETHOD(void)              RotateX(const float angleDegree) override;
    virtual UWMETHOD(void)              RotateY(const float angleDegree) override;

    virtual UWMETHOD_VECTOR(void)       Scale(const Vector4 scale) override;
    virtual UWMETHOD(void)              ScaleX(const float scale) override;
    virtual UWMETHOD(void)              ScaleY(const float scale) override;

    virtual UWMETHOD_VECTOR(Vector4)    GetPosition() const override;
    virtual UWMETHOD_VECTOR(Vector4)    GetRotation() const override;
    virtual UWMETHOD_VECTOR(Vector4)    GetScale() const override;

    virtual UWMETHOD_VECTOR(void)       SetPosition(const Vector4 position) override;
    virtual UWMETHOD_VECTOR(void)       SetRotation(const Vector4 angleDegrees) override;
    virtual UWMETHOD_VECTOR(void)       SetScale(const Vector4 scale) override;

    virtual UWMETHOD(void)              SetCamera(ICamera2D* pCamera) override;

    virtual UWMETHOD(MESH_RENDER_TYPE)  GetRenderType() const override;
    virtual UWMETHOD(void)              SetRenderType(const MESH_RENDER_TYPE type) override;

private:
    UWMETHOD(void)                      updateWorldMatrix();

private:
    vsize                   m_refCount = 0;

    IRendererD3D11_2D*      m_pRenderer = nullptr;
    ID3D11DeviceContext*    m_pImmediateContext = nullptr;

    ID3D11InputLayout*      m_pVertexLayout = nullptr;
    ID3D11VertexShader*     m_pVertexShader = nullptr;
    ID3D11PixelShader*      m_pPixelShader = nullptr;
    ID3D11Buffer*           m_pVertexBuffer = nullptr;
    ID3D11Buffer*           m_pIndexBuffer = nullptr;
    ID3D11Buffer*           m_pConstantBuffer = nullptr;

    MESH_RENDER_TYPE        m_renderType = MESH_RENDER_TYPE_WIREFRAME;

    Matrix44                m_world = {};

    uint                    m_numVertices = 0;
    uint                    m_numIndices = 0;
    uint                    m_vertexSize = 0;

    ICamera2D*              m_pCamera = nullptr;
    Vector4                 m_position = {};
    Vector4                 m_rotationDegree = {};
    Vector4                 m_scale = {};
};