/*
* 2D Camera
*
* 작성자: bumpsgoodman
* 작성일: 2024.04.06
*/

#include "Precompiled.h"
#include "UWEngineCommon/Typedef.h"

#include <DirectXMath.h>
using namespace DirectX;

UWMETHOD(vsize) Camera2D::AddRef()
{
    ++m_refCount;
    return m_refCount;
}

UWMETHOD(vsize) Camera2D::Release()
{
    --m_refCount;
    if (m_refCount == 0)
    {
        delete this;
        return 0;
    }

    return m_refCount;
}

UWMETHOD(vsize) Camera2D::GetRefCount() const
{
    return m_refCount;
}

UWMETHOD(bool) Camera2D::Initailize()
{
    m_eye = Vector4Set(0.0f, 150.0f, -300.0f, 0.0f);
    m_at = Vector4Set(0.0f, 150.0f, 0.0f, 0.0f);
    m_up = Vector4Set(0.0f, 1.0f, 0.0f, 0.0f);

    m_vView = m_at - m_eye;
    m_vView = Vector4Normalize(m_vView);

    return true;
}

UWMETHOD(void) Camera2D::Update(const float fovY, const float aspectRatio, const float nearZ, const float farZ)
{
    Matrix44 rotationMat = Matrix44RotateRollPitchYawFromVector(Vector4DegreeToRad(m_rotationDegree));

    m_view = rotationMat * XMMatrixToMatrix44(XMMatrixLookAtLH(Vector4ToXMVector(m_eye), Vector4ToXMVector(m_at), Vector4ToXMVector(m_up)));
    m_projection = XMMatrixToMatrix44(XMMatrixPerspectiveFovLH(fovY, aspectRatio, nearZ, farZ));
}

UWMETHOD_VECTOR(void) Camera2D::Translate(const Vector4 dist)
{
    m_eye += dist;
    m_at += dist;
}

UWMETHOD(void) Camera2D::TranslateX(const float dist)
{
    m_eye.X += dist;
    m_at.X += dist;
}

UWMETHOD(void) Camera2D::TranslateY(const float dist)
{
    m_eye.Y += dist;
    m_at.Y += dist;
}

UWMETHOD_VECTOR(void) Camera2D::Rotate(const Vector4 angleDegrees)
{
    m_rotationDegree += angleDegrees;
}

UWMETHOD(void) Camera2D::RotateX(const float angleDegree)
{
    m_rotationDegree.X += angleDegree;
}

UWMETHOD(void) Camera2D::RotateY(const float angleDegree)
{
    m_rotationDegree.Y += angleDegree;
}

UWMETHOD_VECTOR(Vector4) Camera2D::GetRotation() const
{
    return m_rotationDegree;
}

UWMETHOD_VECTOR(void) Camera2D::SetRotation(const Vector4 angleDegrees)
{
    m_rotationDegree = angleDegrees;
}

UWMETHOD_VECTOR(Vector4) Camera2D::GetEye() const
{
    return m_eye;
}

UWMETHOD_VECTOR(Vector4) Camera2D::GetAt() const
{
    return m_at;
}

UWMETHOD_VECTOR(Vector4) Camera2D::GetUp() const
{
    return m_up;
}

UWMETHOD_VECTOR(void) Camera2D::SetEye(const Vector4 v)
{
    m_eye = v;
}

UWMETHOD_VECTOR(void) Camera2D::SetAt(const Vector4 v)
{
    m_at = v;
}

UWMETHOD_VECTOR(void) Camera2D::SetUp(const Vector4 v)
{
    m_up = v;
}

UWMETHOD_VECTOR(Matrix44) Camera2D::GetView() const
{
    return m_view;
}

UWMETHOD_VECTOR(Matrix44) Camera2D::GetProjection() const
{
    return m_projection;
}