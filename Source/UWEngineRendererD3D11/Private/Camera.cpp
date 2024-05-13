/*
* D3D11 카메라
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.17
*/

#include "Precompiled.h"
#include "UWEngineCommon/Interfaces/IRenderer.h"
#include "Camera.h"

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

bool __stdcall Camera::Initailize(const float fovY, const float aspectRatio, const float nearZ, const float farZ)
{
    m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_at = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    m_position = XMVectorSet(0.0f, 10.0f, -50.0f, 0.0f);
    m_rotationDegree = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    m_view = XMMatrixLookAtLH(m_position, m_at, m_up);
    m_projection = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);

    return true;
}

void __stdcall Camera::Update()
{
    static const XMVECTOR DEFAULT_FORWARD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    const XMMATRIX rotationMat = XMMatrixRotationRollPitchYawFromVector(UW_XMVectorToRadian(m_rotationDegree));
    m_at = XMVector3Transform(DEFAULT_FORWARD, rotationMat);
    m_at = XMVector3Normalize(m_at);

    const XMMATRIX rotateYTempMatrix = XMMatrixRotationY(XMVectorGetY(m_rotationDegree) * PI_DIV_180);
    m_up = XMVector3TransformCoord(m_up, rotateYTempMatrix);

    m_at = m_position + m_at;

    m_view = XMMatrixLookAtLH(m_position, m_at, m_up);
}

void __vectorcall Camera::Translate(const Vector4 dist)
{
    m_position += UW_Vector4ToXMVector(dist);
}

void __stdcall Camera::TranslateX(const float dist)
{
    static const XMVECTOR DEFAULT_RIGHT = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

    const XMMATRIX rotateYTempMatrix = XMMatrixRotationY(XMVectorGetY(m_rotationDegree) * PI_DIV_180);
    const XMVECTOR right = XMVector3TransformCoord(DEFAULT_RIGHT, rotateYTempMatrix);

    m_position += dist * right;
}

void __stdcall Camera::TranslateY(const float dist)
{
    static const XMVECTOR DEFAULT_UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    const XMMATRIX rotateYTempMatrix = XMMatrixRotationY(XMVectorGetY(m_rotationDegree) * PI_DIV_180);
    const XMVECTOR up = XMVector3TransformCoord(DEFAULT_UP, rotateYTempMatrix);

    m_position += dist * up;
}

void __stdcall Camera::TranslateZ(const float dist)
{
    static const XMVECTOR DEFAULT_FORWARD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    const XMMATRIX rotateYTempMatrix = XMMatrixRotationY(XMVectorGetY(m_rotationDegree) * PI_DIV_180);
    const XMVECTOR forward = XMVector3TransformCoord(DEFAULT_FORWARD, rotateYTempMatrix);

    m_position += dist * forward;
}

void __vectorcall Camera::Rotate(const Vector4 angleDegrees)
{
    m_rotationDegree += UW_Vector4ToXMVector(angleDegrees);
    m_rotationDegree = UW_XMVectorWrap(m_rotationDegree, XMVectorZero(), XMVectorSet(360.0f, 360.0f, 360.0f, 360.0f));
}

void __stdcall Camera::RotateX(const float angleDegree)
{
    m_rotationDegree = XMVectorSetX(m_rotationDegree, Wrap(XMVectorGetX(m_rotationDegree) + angleDegree, 0.0f, 360.0f));
}

void __stdcall Camera::RotateY(const float angleDegree)
{
    m_rotationDegree = XMVectorSetY(m_rotationDegree, Wrap(XMVectorGetY(m_rotationDegree) + angleDegree, 0.0f, 360.0f));
}

void __stdcall Camera::RotateZ(const float angleDegree)
{
    m_rotationDegree = XMVectorSetZ(m_rotationDegree, Wrap(XMVectorGetZ(m_rotationDegree) + angleDegree, 0.0f, 360.0f));
}

Vector4 __vectorcall Camera::GetPosition() const
{
    return UW_XMVectorToVector4(m_position);
}

void __vectorcall Camera::SetPosition(const Vector4 position)
{
    m_position = UW_Vector4ToXMVector(position);
}

Vector4 __vectorcall Camera::GetRotation() const
{
    return UW_XMVectorToVector4(m_rotationDegree);
}

void __vectorcall Camera::SetRotation(const Vector4 angleDegrees)
{
    m_rotationDegree = UW_XMVectorWrap(UW_Vector4ToXMVector(angleDegrees), XMVectorZero(), XMVectorSet(360.0f, 360.0f, 360.0f, 360.0f));
}

Matrix44 __vectorcall Camera::GetView() const
{
    return UW_XMMatrixToMatrix44(m_view);
}

Matrix44 __vectorcall Camera::GetProjection() const
{
    return UW_XMMatrixToMatrix44(m_projection);
}