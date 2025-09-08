#include "PointLight.h"

#include "imgui.h"

PointLight::PointLight(Graphics &gfx, float radius)
: m_mesh(gfx, radius),
  m_cbuf(gfx)
{ }

void PointLight::SpawnControlWindow() noexcept
{
    if (ImGui::Begin("Light"))
    {
        ImGui::Text("Position:");
        ImGui::SliderFloat( "X",&m_pos.x,-60.0f,60.0f,"%.1f" );
        ImGui::SliderFloat( "Y",&m_pos.y,-60.0f,60.0f,"%.1f" );
        ImGui::SliderFloat( "Z",&m_pos.z,-60.0f,60.0f,"%.1f" );
        if( ImGui::Button( "Reset" ) )
            Reset();
    }
    ImGui::End();
}

void PointLight::Reset() noexcept
{
    m_pos = {0.0f,0.0f,0.0f};
}

void PointLight::Draw(Graphics &gfx) const noexcept(!true)
{
    m_mesh.SetPos(m_pos);
    m_mesh.Draw(gfx);
}

void PointLight::Bind(Graphics &gfx) const noexcept
{
    m_cbuf.Update(gfx, PointLightCbuf{m_pos});
}