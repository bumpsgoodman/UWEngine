/*
* Camera
*
* 작성자: bumpsgoodman
* 작성일: 2024.02.20
*/

#include "Precompiled.h"
#include "Camera.h"

#include <DirectXMath.h>
using namespace DirectX;

void __stdcall Private_CreateCamera(ICamera** ppOutCamera);

ALIGN16 class Camera final : public ICamera
{
public:
    Camera() = default;
    Camera(const Camera&) = default;
    Camera& operator=(const Camera&) = default;
    ~Camera() = default;

    virtual vsize __stdcall AddRef() override;
    virtual vsize __stdcall Release() override;
    virtual vsize __stdcall GetRefCount() const override;

    virtual bool __stdcall Initailize() override;
    virtual void __stdcall Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ) override;

    virtual void __stdcall Translate(const Vector4 dist) override;
    virtual void __stdcall TranslateX(const float dist) override;
    virtual void __stdcall TranslateY(const float dist) override;
    virtual void __stdcall TranslateZ(const float dist) override;

    virtual void __stdcall Rotate(const Vector4 angleDegrees) override;
    virtual void __stdcall RotateX(const float angleDegree) override;
    virtual void __stdcall RotateY(const float angleDegree) override;
    virtual void __stdcall RotateZ(const float angleDegree) override;

    virtual void __stdcall SetRotation(const Vector4 angleDegrees) override;
    virtual Vector4 __stdcall GetRotation() const override;

    virtual void __vectorcall SetEye(const Vector4 v) override;
    virtual void __vectorcall SetAt(const Vector4 v) override;
    virtual void __vectorcall SetUp(const Vector4 v) override;

    virtual Vector4 __vectorcall GetEye() const override;
    virtual Vector4 __vectorcall GetAt() const override;
    virtual Vector4 __vectorcall GetUp() const override;

    virtual Matrix44 __vectorcall GetView() const override;
    virtual Matrix44 __vectorcall GetProjection() const override;

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

vsize __stdcall Camera::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

vsize __stdcall Camera::Release()
{
    --m_refCount;
    if (m_refCount == 0)
    {
        delete this;
        return 0;
    }

    return m_refCount;
}

vsize __stdcall Camera::GetRefCount() const
{
    return m_refCount;
}

bool __stdcall Camera::Initailize()
{
    m_eye = Vector4Set(0.0f, 150.0f, -300.0f, 0.0f);
    m_at = Vector4Set(0.0f, 150.0f, 0.0f, 0.0f);
    m_up = Vector4Set(0.0f, 1.0f, 0.0f, 0.0f);

    m_vView = m_at - m_eye;
    m_vView = Vector4Normalize(m_vView);

    return true;
} 

void __stdcall Camera::Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ)
{
    Matrix44 rotationMat = Matrix44RotateRollPitchYawFromVector(Vector4DegreeToRad(m_rotationDegree));

    m_view = rotationMat * XMMatrixToMatrix44(XMMatrixLookAtLH(Vector4ToXMVector(m_eye), Vector4ToXMVector(m_at), Vector4ToXMVector(m_up)));
    m_projection = XMMatrixToMatrix44(XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ));
}

void __stdcall Camera::Translate(const Vector4 dist)
{
    m_eye += dist;
    m_at += dist;
}

void __stdcall Camera::TranslateX(const float dist)
{
    m_eye.X += dist;
    m_at.X += dist;
}

void __stdcall Camera::TranslateY(const float dist)
{
    m_eye.Y += dist;
    m_at.Y += dist;
}

void __stdcall Camera::TranslateZ(const float dist)
{
    m_eye.Z += dist;
    m_at.Z += dist;
}

void __stdcall Camera::Rotate(const Vector4 angleDegree)
{
    m_rotationDegree += angleDegree;
}

void __stdcall Camera::RotateX(const float angleDegree)
{
    m_rotationDegree.X += angleDegree;
}

void __stdcall Camera::RotateY(const float angleDegree)
{
    m_rotationDegree.Y += angleDegree;
}

void __stdcall Camera::RotateZ(const float angleDegree)
{
    m_rotationDegree.Z += angleDegree;
}

void __stdcall Camera::SetRotation(const Vector4 angleDegrees)
{
    m_rotationDegree = angleDegrees;
}

Vector4 __stdcall Camera::GetRotation() const
{
    return m_rotationDegree;
}

void __vectorcall Camera::SetEye(const Vector4 v)
{
    m_eye = v;
}

void __vectorcall Camera::SetAt(const Vector4 v)
{
    m_at = v;
}

void __vectorcall Camera::SetUp(const Vector4 v)
{
    m_up = v;
}

Vector4 __vectorcall Camera::GetEye() const
{
    return m_eye;
}

Vector4 __vectorcall Camera::GetAt() const
{
    return m_at;
}

Vector4 __vectorcall Camera::GetUp() const
{
    return m_up;
}

Matrix44 __vectorcall Camera::GetView() const
{
    return m_view;
}

Matrix44 __vectorcall Camera::GetProjection() const
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