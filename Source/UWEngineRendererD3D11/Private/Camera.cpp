/*
* Camera
*
* 작성자: bumpsgoodman
* 작성일: 2024.02.20
*/

#include "Precompiled.h"
#include "Camera.h"

void __stdcall Private_CreateCamera(ICamera** ppOutCamera);

class Camera final : public ICamera
{
public:
    Camera() = default;
    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;
    ~Camera() = default;

    virtual size_t __stdcall AddRef() override;
    virtual size_t __stdcall Release() override;
    virtual size_t __stdcall GetRefCount() const override;

    virtual bool __stdcall Initailize() override;
    virtual void __stdcall Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ) override;

    virtual void __stdcall MoveX(const float dist) override;
    virtual void __stdcall MoveY(const float dist) override;
    virtual void __stdcall MoveZ(const float dist) override;

    virtual void __stdcall RotatePitch(const float angleRad) override;
    virtual void __stdcall RotateYaw(const float angleRad) override;
    virtual void __stdcall RotateRoll(const float angleRad) override;

    virtual void __vectorcall SetEye(FXMVECTOR v) override;
    virtual void __vectorcall SetAt(FXMVECTOR v) override;
    virtual void __vectorcall SetUp(FXMVECTOR v) override;

    virtual XMVECTOR __vectorcall GetEye() const override;
    virtual XMVECTOR __vectorcall GetAt() const override;
    virtual XMVECTOR __vectorcall GetUp() const override;

    virtual XMMATRIX __vectorcall GetView() const override;
    virtual XMMATRIX __vectorcall GetProjection() const override;

private:
    size_t m_refCount = 0;

    XMVECTOR m_eye = {};
    XMVECTOR m_at = {};
    XMVECTOR m_up = {};
    XMVECTOR m_vView = {};

    XMVECTOR m_rotation = {};

    XMMATRIX m_view = {};
    XMMATRIX m_projection = {};
};

size_t __stdcall Camera::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

size_t __stdcall Camera::Release()
{
    --m_refCount;
    if (m_refCount == 0)
    {
        delete this;
        return 0;
    }

    return m_refCount;
}

size_t __stdcall Camera::GetRefCount() const
{
    return m_refCount;
}

bool __stdcall Camera::Initailize()
{
    m_eye = XMVectorSet(0.0f, 150.0f, -300.0f, 0.0f);
    m_at = XMVectorSet(0.0f, 150.0f, 0.0f, 0.0f);
    m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    m_vView = m_at - m_eye;
    m_vView = XMVector4Normalize(m_vView);

    return true;
} 

void __stdcall Camera::Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ)
{
    XMMATRIX rotationMat = XMMatrixRotationRollPitchYawFromVector(m_rotation);

    m_view = rotationMat * XMMatrixLookAtLH(m_eye, m_at, m_up);
    m_projection = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);
}

void __stdcall Camera::MoveX(const float dist)
{
    XMVECTOR dir = XMVectorSet(1.0f * dist, 0.0f, 0.0f, 0.0f);
    m_eye += dir;
    m_at += dir;
}

void __stdcall Camera::MoveY(const float dist)
{
    XMVECTOR dir = XMVectorSet(0.0f, 1.0f * dist, 0.0f, 0.0f);
    m_eye += dir;
    m_at += dir;
}

void __stdcall Camera::MoveZ(const float dist)
{
    XMVECTOR dir = XMVectorSet(0.0f, 0.0f, 1.0f * dist, 0.0f);
    m_eye += dir;
    m_at += dir;
}

void __stdcall Camera::RotatePitch(const float angleDegree)
{
    const float angleRad = angleDegree * (float)PI / 180.0f;
    XMVECTOR angle = XMVectorSet(1.0f * angleRad, 0.0f, 0.0f, 0.0f);
    m_rotation += angle;
}

void __stdcall Camera::RotateYaw(const float angleDegree)
{
    const float angleRad = angleDegree * (float)PI / 180.0f;
    XMVECTOR angle = XMVectorSet(0.0f, 1.0f * angleRad, 0.0f, 0.0f);
    m_rotation += angle;
}

void __stdcall Camera::RotateRoll(const float angleDegree)
{
    const float angleRad = angleDegree * (float)PI / 180.0f;
    XMVECTOR angle = XMVectorSet(0.0f, 0.0f, 1.0f * angleRad, 0.0f);
    m_rotation += angle;
}

void __vectorcall Camera::SetEye(FXMVECTOR v)
{
    m_eye = v;
}

void __vectorcall Camera::SetAt(FXMVECTOR v)
{
    m_at = v;
}

void __vectorcall Camera::SetUp(FXMVECTOR v)
{
    m_up = v;
}

XMVECTOR __vectorcall Camera::GetEye() const
{
    return m_eye;
}

XMVECTOR __vectorcall Camera::GetAt() const
{
    return m_at;
}

XMVECTOR __vectorcall Camera::GetUp() const
{
    return m_up;
}

XMMATRIX __vectorcall Camera::GetView() const
{
    return m_view;
}

XMMATRIX __vectorcall Camera::GetProjection() const
{
    return m_projection;
}

void __stdcall Private_CreateCamera(ICamera** ppOutCamera)
{
    ASSERT(ppOutCamera != nullptr, "ppOutCamera == nullptr");

    Camera* pCamera = new Camera;
    pCamera->AddRef();

    *ppOutCamera = pCamera;
}