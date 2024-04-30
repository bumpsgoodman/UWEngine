/*
* Camera
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.17
*/

#include "Precompiled.h"
#include "UWEngineCommon/CoreTypes.h"

static const XMVECTOR DEFAULT_RIGHT;
static const XMVECTOR DEFAULT_FORWARD;
static const XMVECTOR DEFAULT_UP;

UWMETHOD(vsize) Camera::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

UWMETHOD(vsize) Camera::Release()
{
    --m_refCount;
    if (m_refCount == 0)
    {
        delete this;
        return 0;
    }

    return m_refCount;
}

UWMETHOD(vsize) Camera::GetRefCount() const
{
    return m_refCount;
}

UWMETHOD(bool) Camera::Initailize(const float fovY, const float aspectRatio, const float nearZ, const float farZ)
{
    m_up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
    m_at = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    m_position = XMVectorSet(0.0f, 10.0f, -50.0f, 0.0f);
    m_rotationDegree = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);

    m_view = XMMatrixLookAtLH(m_position, m_at, m_up);
    m_projection = XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ);

    return true;
}

UWMETHOD(void) Camera::Update()
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

UWMETHOD_VECTOR(void) Camera::Translate(const XMVECTOR dist)
{
    m_position += dist;
}

UWMETHOD(void) Camera::TranslateX(const float dist)
{
    static const XMVECTOR DEFAULT_RIGHT = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

    const XMMATRIX rotateYTempMatrix = XMMatrixRotationY(XMVectorGetY(m_rotationDegree) * PI_DIV_180);
    const XMVECTOR right = XMVector3TransformCoord(DEFAULT_RIGHT, rotateYTempMatrix);

    m_position += dist * right;
}

UWMETHOD(void) Camera::TranslateY(const float dist)
{
    static const XMVECTOR DEFAULT_UP = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    const XMMATRIX rotateYTempMatrix = XMMatrixRotationY(XMVectorGetY(m_rotationDegree) * PI_DIV_180);
    const XMVECTOR up = XMVector3TransformCoord(DEFAULT_UP, rotateYTempMatrix);

    m_position += dist * up;
}

UWMETHOD(void) Camera::TranslateZ(const float dist)
{
    static const XMVECTOR DEFAULT_FORWARD = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

    const XMMATRIX rotateYTempMatrix = XMMatrixRotationY(XMVectorGetY(m_rotationDegree) * PI_DIV_180);
    const XMVECTOR forward = XMVector3TransformCoord(DEFAULT_FORWARD, rotateYTempMatrix);

    m_position += dist * forward;
}

UWMETHOD_VECTOR(void) Camera::Rotate(const XMVECTOR angleDegrees)
{
    m_rotationDegree += angleDegrees;
    m_rotationDegree = UW_XMVectorWrap(m_rotationDegree, XMVectorZero(), XMVectorSet(360.0f, 360.0f, 360.0f, 360.0f));
}

UWMETHOD(void) Camera::RotateX(const float angleDegree)
{
    m_rotationDegree = XMVectorSetX(m_rotationDegree, Wrap(XMVectorGetX(m_rotationDegree) + angleDegree, 0.0f, 360.0f));
}

UWMETHOD(void) Camera::RotateY(const float angleDegree)
{
    m_rotationDegree = XMVectorSetY(m_rotationDegree, Wrap(XMVectorGetY(m_rotationDegree) + angleDegree, 0.0f, 360.0f));
}

UWMETHOD(void) Camera::RotateZ(const float angleDegree)
{
    m_rotationDegree = XMVectorSetZ(m_rotationDegree, Wrap(XMVectorGetZ(m_rotationDegree) + angleDegree, 0.0f, 360.0f));
}

UWMETHOD_VECTOR(XMVECTOR) Camera::GetPosition() const
{
    return m_position;
}

UWMETHOD_VECTOR(void) Camera::SetPosition(const XMVECTOR position)
{
    m_position = position;
}

UWMETHOD_VECTOR(XMVECTOR) Camera::GetRotation() const
{
    return m_rotationDegree;
}

UWMETHOD_VECTOR(void) Camera::SetRotation(const XMVECTOR angleDegrees)
{
    m_rotationDegree = UW_XMVectorWrap(angleDegrees, XMVectorZero(), XMVectorSet(360.0f, 360.0f, 360.0f, 360.0f));
}

UWMETHOD_VECTOR(XMMATRIX) Camera::GetView() const
{
    return m_view;
}

UWMETHOD_VECTOR(XMMATRIX) Camera::GetProjection() const
{
    return m_projection;
}