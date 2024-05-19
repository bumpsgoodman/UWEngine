/*
* D3D11 메시 오브젝트
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.12
*/

#pragma once

#include "VertexBuffer.h"
#include "TextureFaceGroup.h"

ALIGN16 class MeshObject : public IMeshObject
{
public:
    MeshObject() = default;
    MeshObject(const MeshObject&) = delete;
    MeshObject& operator=(const MeshObject&) = delete;
    ~MeshObject() = default;

    virtual vsize       __stdcall       AddRef() override;
    virtual vsize       __stdcall       Release() override;
    virtual vsize       __stdcall       GetRefCount() const override;

    virtual bool        __stdcall       Initialize(IRenderer* pRenderer) override;

    virtual bool        __stdcall       CreateMesh(const int includeFlag,
                                                   const void* pVertices, const uint vertexSize, const uint numVertices,
                                                   const uint16** ppIndices, const uint16* pNumIndices, const uint numIndexBuffers,
                                                   const void* pTexCoordsOrNull, const wchar_t** ppTextureFileNamesOrNull,
                                                   const wchar_t* pShaderFileName, const char* pVSEntryPoint, const char* pPSEntryPoint) override;
    virtual void        __stdcall       RenderMesh() override;

    virtual void        __vectorcall    Translate(const Vector4 dist) override;
    virtual void        __stdcall       TranslateX(const float dist) override;
    virtual void        __stdcall       TranslateY(const float dist) override;
    virtual void        __stdcall       TranslateZ(const float dist) override;

    virtual void        __vectorcall    Rotate(const Vector4 angleDegrees) override;
    virtual void        __stdcall       RotateX(const float angleDegree) override;
    virtual void        __stdcall       RotateY(const float angleDegree) override;
    virtual void        __stdcall       RotateZ(const float angleDegree) override;

    virtual void        __vectorcall    Scale(const Vector4 scale) override;
    virtual void        __stdcall       ScaleX(const float scale) override;
    virtual void        __stdcall       ScaleY(const float scale) override;
    virtual void        __stdcall       ScaleZ(const float scale) override;

    virtual Vector4     __vectorcall    GetPosition() const override;
    virtual Vector4     __vectorcall    GetRotation() const override;
    virtual Vector4     __vectorcall    GetScale() const override;

    virtual void        __vectorcall    SetPosition(const Vector4 position) override;
    virtual void        __vectorcall    SetRotation(const Vector4 angleDegrees) override;
    virtual void        __vectorcall    SetScale(const Vector4 scale) override;

    virtual void        __stdcall       SetCamera(ICamera* pCamera) override;

private:
    void                __stdcall       updateWorldMatrix();

private:
    ID3D11SamplerState*         m_pSamplerLinear = nullptr;

private:
    vsize                       m_refCount = 0;

    IRenderer*                  m_pRenderer = nullptr;
    ID3D11Device*               m_pDevice = nullptr;
    ID3D11DeviceContext*        m_pImmediateContext = nullptr;

    ID3D11InputLayout*          m_pVertexLayout = nullptr;
    ID3D11VertexShader*         m_pVertexShader = nullptr;
    ID3D11PixelShader*          m_pPixelShader = nullptr;
    ID3D11Buffer*               m_pConstantBuffer = nullptr;

    XMMATRIX                    m_world = {};

    // 모델 데이터
    VertexBuffer                m_vertexBuffer;
    VertexBuffer                m_subVertexBuffer;
    TextureFaceGroup            m_faceGroup;


    uint                        m_includeFlag = 0;
    uint                        m_vertexSize = 0;

    ICamera*                    m_pCamera = nullptr;
    XMVECTOR                    m_position = {};
    XMVECTOR                    m_rotationDegree = {};
    XMVECTOR                    m_scale = {};
};
