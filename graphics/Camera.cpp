#include "Camera.h"
#include "imgui.h"
#include "ChiliMath.h"
#include <algorithm>

namespace dx = DirectX;

Camera::Camera() noexcept
{
    Reset();
}

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
    using namespace dx;
    const XMVECTOR forwardBaseVector = XMVectorSet(0.0f,0.0f,1.0f,0.0f);

    // apply the camera rotations to a base vector
    const auto lookVector = XMVector3Transform(forwardBaseVector,XMMatrixRotationRollPitchYaw(m_pitch,m_yaw,0.0f));

    // generate cameera transform (applied to all objects to arrange them relative
    // to camera position/orientation in world) from camera position and direction
    // camera "top" always faces toward +Y (cannot do a barrel roll)
    const auto camPosition = XMLoadFloat3(&m_pos);
    const auto camTarget = camPosition + lookVector;

    return XMMatrixLookAtLH(camPosition,camTarget,XMVectorSet(0.0f,1.0f,0.0f,0.0f));
}

void Camera::SpawnControlWindow() noexcept
{
    if (ImGui::Begin("Camera"))
    {
        ImGui::Text("Position");
        ImGui::SliderFloat("X", &m_pos.x, -80.0f, 80.0f, "%.1f");
        ImGui::SliderFloat("Y", &m_pos.y, -80.0f, 80.0f, "%.1f");
        ImGui::SliderFloat("Z", &m_pos.z, -80.0f, 80.0f, "%.1f");

        ImGui::Text("Orientation");
        ImGui::SliderAngle("Pitch", &m_pitch, m_pitchConstrain * -90.0f, m_pitchConstrain * 90.0f);
        ImGui::SliderAngle("Yaw", &m_yaw, -180.0f, 180.0f);
        if (ImGui::Button("Reset"))
            Reset();
    }
    ImGui::End();
}

void Camera::Reset() noexcept
{
    m_pos = {0.0f,7.5f,-18.0f};
    m_pitch = 0.0f;
    m_yaw = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
    m_yaw = wrap_angle(m_yaw + dx*m_rotationSpeed);
    m_pitch = std::clamp(m_pitch + dy * m_rotationSpeed, m_pitchConstrain * -PI/2.0f, m_pitchConstrain * PI/2.0f);
}

void Camera::Translate(DirectX::XMFLOAT3 translation) noexcept
{
    dx::XMStoreFloat3(
        &translation,
        dx::XMVector3Transform(
            dx::XMLoadFloat3(&translation),
            dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, 0.0f) *
            dx::XMMatrixScaling(m_travelSpeed, m_travelSpeed, m_travelSpeed)));

    m_pos = {m_pos.x + translation.x,
             m_pos.y + translation.y,
             m_pos.z + translation.z};
}