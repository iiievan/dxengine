#include "TestPlane.h"
#include "geometry/Plane.h"
#include "bindable/BindableCommon.h"
#include "imgui.h"
#include "bindable/TransformCbufDouble.h"

TestPlane::TestPlane(Graphics &gfx, float size)
{
    using namespace Bind;
    namespace dx = DirectX;

    auto model = Plane::Make();
    model.Transform(dx::XMMatrixScaling(size,size,1.0f));
    const auto geometryTag = "$plane." + std::to_string(size);

    AddBind(VertexBuffer::Resolve(gfx, geometryTag, model.vertices));
    AddBind(IndexBuffer::Resolve(gfx, geometryTag, model.indices));

    AddBind(Texture::Resolve(gfx,"textures\\brickwall.jpg"));
    AddBind(Texture::Resolve(gfx,"textures\\brickwall_normal.jpg", 1u));

    auto pvs = VertexShader::Resolve(gfx, "shaders\\Phong.vs.cso");
    auto pvsbc = pvs->GetBytecode();
    AddBind(std::move(pvs));
    AddBind(PixelShader::Resolve(gfx, "shaders\\PhongNormalMapObject.ps.cso"));

    AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx,m_pmc,1u));
    AddBind(InputLayout::Resolve(gfx,model.vertices.GetLayout(),pvsbc ));
    AddBind(Topology::Resolve(gfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    AddBind(std::make_shared<TransformCbufDouble>(gfx, *this, 0u, 2u));
}

void TestPlane::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    m_pos = pos;
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
    m_roll = roll;
    m_pitch = pitch;
    m_yaw = yaw;
}

DirectX::XMMATRIX TestPlane::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(m_roll, m_pitch, m_yaw) *
           DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}

void TestPlane::SpawnControlWindow(Graphics &gfx) noexcept
{
    if( ImGui::Begin( "Plane" ) )
    {
        ImGui::Text( "Position" );
        ImGui::SliderFloat( "X",&m_pos.x,-80.0f,80.0f,"%.1f" );
        ImGui::SliderFloat( "Y",&m_pos.y,-80.0f,80.0f,"%.1f" );
        ImGui::SliderFloat( "Z",&m_pos.z,-80.0f,80.0f,"%.1f" );
        ImGui::Text( "Orientation" );
        ImGui::SliderAngle( "Roll",&m_roll,-180.0f,180.0f );
        ImGui::SliderAngle( "Pitch",&m_pitch,-180.0f,180.0f );
        ImGui::SliderAngle( "Yaw",&m_yaw,-180.0f,180.0f );
        ImGui::Text( "Shading" );
        bool changed0 = ImGui::SliderFloat( "Spec. Int.",&m_pmc.specularIntencity,0.0f,1.0f );
        bool changed1 = ImGui::SliderFloat( "Spec. Power",&m_pmc.specularPower,0.0f,100.0f );
        bool checkState = m_pmc.normalMapEnabled == TRUE;
        bool changed2 = ImGui::Checkbox( "Enable Normal Map",&checkState );
        m_pmc.normalMapEnabled = checkState ? TRUE : FALSE;
        if( changed0 || changed1 || changed2 )
        {
            QueryBindable<Bind::PixelConstantBuffer<PSMaterialConstant>>()->Update( gfx,m_pmc );
        }
    }
    ImGui::End();
}