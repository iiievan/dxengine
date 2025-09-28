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
    return dx::XMMatrixTranslation(-m_pos.x, -m_pos.y, -m_pos.z) *
           dx::XMMatrixRotationRollPitchYaw(-m_pitch, -m_yaw, 0.0f);
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
        ImGui::SliderAngle("Pitch", &m_pitch, -90.0f, 90.0f);
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
    m_pitch = std::clamp(m_pitch + dy * m_rotationSpeed, -PI/2.0f, PI/2.0f);
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