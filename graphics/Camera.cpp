#include "Camera.h"
#include "imgui.h"

namespace dx = DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
    const auto pos = dx::XMVector3Transform(
        dx::XMVectorSet(0.0f, 0.0f, -m_r, 0.0f), dx::XMMatrixRotationRollPitchYaw(m_phi, -m_theta, 0.0f));
    return dx::XMMatrixLookAtLH(pos, dx::XMVectorZero(), dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f))
           * dx::XMMatrixRotationRollPitchYaw(m_pitch, -m_yaw, m_roll);
}

void Camera::SpawnControlWindow() noexcept
{
    if (ImGui::Begin("Camera"))
    {
        ImGui::Text("Position");
        ImGui::SliderFloat("R", &m_r, 0.1f, 80.0f, "%.1f");
        ImGui::SliderAngle("Theta", &m_theta, -180.0f, 180.0f);
        ImGui::SliderAngle("Phi", &m_phi, -89.0f, 89.0f);
        ImGui::Text("Orientation");
        ImGui::SliderAngle("Roll", &m_roll, -180.0f, 180.0f);
        ImGui::SliderAngle("Pitch", &m_pitch, -180.0f, 180.0f);
        ImGui::SliderAngle("Yaw", &m_yaw, -180.0f, 180.0f);
        if (ImGui::Button("Reset"))
            Reset();
    }
    ImGui::End();
}

void Camera::Reset() noexcept
{
    m_r = 20.0f;
    m_theta = 0.0f;
    m_phi = 0.0f;
    m_pitch = 0.0f;
    m_yaw = 0.0f;
    m_roll = 0.0f;
}