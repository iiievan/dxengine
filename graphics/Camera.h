#ifndef __CAMERA_H
#define __CAMERA_H
#include <DirectXMath.h>

class Camera
{
public:
    Camera() noexcept;

    DirectX::XMMATRIX GetMatrix() const noexcept;
    void SpawnControlWindow() noexcept;
    void Reset() noexcept;
    void Rotate(float dx, float dy) noexcept;
    void Translate(DirectX::XMFLOAT3 translation) noexcept;

private:
    DirectX::XMFLOAT3 m_pos;
    float m_pitch;
    float m_yaw;

    static constexpr float m_travelSpeed = 12.0f;
    static constexpr float m_rotationSpeed = 0.004f;
    static constexpr float m_pitchConstrain = 0.995f;
};



#endif //__CAMERA_H
