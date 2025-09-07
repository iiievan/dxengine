#ifndef __CAMERA_H
#define __CAMERA_H
#include <DirectXMath.h>

class Camera
{
public:
    DirectX::XMMATRIX GetMatrix() const noexcept;
    void SpawnControlWindow() noexcept;
    void Reset() noexcept;
private:
    float m_r = 20.0f;      // расстояние от центра координат по оси Z

    float m_pitch = 0.0f;   // Угол тангажа вокруг собственной оси X
    float m_yaw = 0.0f;     // Угол рысканья вокруг собственной оси Y
    float m_roll = 0.0f;    // Угол крена вокруг собственной оси Z

    float m_theta = 0.0f;   // Угол орбитального вращения вокруг мировой оси Y
    float m_phi = 0.0f;     // Угол орбитального вращения вокруг мировой оси X
};



#endif //__CAMERA_H
