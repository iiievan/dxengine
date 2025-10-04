#include "TestPlane.h"
#include "geometry/Plane.h"
#include "bindable/BindableCommon.h"

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

    auto pvs = VertexShader::Resolve(gfx, "shaders\\Phong.vs.cso");
    auto pvsbc = pvs->GetBytecode();
    AddBind(std::move(pvs));
    AddBind(PixelShader::Resolve(gfx, "shaders\\Phong.ps.cso"));

    struct PSMaterialConstant
    {
        float specularIntencity = 0.1f;
        float specularPower = 20.0f;
        float padding[2];
    }pmc;

    AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx,pmc,1u));
    AddBind(InputLayout::Resolve(gfx,model.vertices.GetLayout(),pvsbc ));
    AddBind(Topology::Resolve(gfx, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
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