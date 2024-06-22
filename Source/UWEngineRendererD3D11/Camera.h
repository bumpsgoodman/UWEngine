/*
* D3D11 카메라
*
* 작성자: bumpsgoodman
* 작성일: 2024.05.12
*/

#pragma once

ALIGN16 class Camera final : public ICamera
{
public:
    Camera() = default;
    Camera(const Camera&) = delete;
    Camera& operator=(const Camera&) = delete;
    ~Camera() = default;

    virtual uint        __stdcall       AddRef() override;
    virtual uint        __stdcall       Release() override;
    virtual uint        __stdcall       GetRefCount() const override;

    virtual bool        __stdcall       Initailize(const float fovY, const float aspectRatio, const float nearZ, const float farZ) override;
    virtual void        __stdcall       Update() override;

    virtual void        __vectorcall    Translate(const Vector4 dist) override;
    virtual void        __stdcall       TranslateX(const float dist) override;
    virtual void        __stdcall       TranslateY(const float dist) override;
    virtual void        __stdcall       TranslateZ(const float dist) override;

    virtual void        __vectorcall    Rotate(const Vector4 angleDegrees) override;
    virtual void        __stdcall       RotateX(const float angleDegree) override;
    virtual void        __stdcall       RotateY(const float angleDegree) override;
    virtual void        __stdcall       RotateZ(const float angleDegree) override;

    virtual Vector4     __vectorcall    GetPosition() const override;
    virtual void        __vectorcall    SetPosition(const Vector4 position) override;

    virtual Vector4     __vectorcall    GetRotation() const override;
    virtual void        __vectorcall    SetRotation(const Vector4 angleDegrees) override;

    virtual Matrix44    __vectorcall    GetView() const override;
    virtual Matrix44    __vectorcall    GetProjection() const override;

private:
    uint        m_refCount = 0;

    XMVECTOR    m_up = {};
    XMVECTOR    m_at = {};

    XMVECTOR    m_position = {};
    XMVECTOR    m_rotationDegree = {};

    XMMATRIX    m_view = {};
    XMMATRIX    m_projection = {};
};