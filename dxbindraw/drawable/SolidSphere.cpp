#include "SolidSphere.h"
#include "GraphicsThrowMacroses.h"
#include "bindable/BindableCommon.h"
#include "geometry/Sphere.h"

SolidSphere::SolidSphere(Graphics &gfx, float radius)
{
    using namespace Bind;
    namespace dx = DirectX;

    struct Vertex
    {
        dx::XMFLOAT3 pos;
    };

    auto model = Sphere::Make<Vertex>();
    model.Transform(dx::XMMatrixScaling(radius, radius, radius));
    AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));
    AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

    auto pvs = std::make_shared<VertexShader>(gfx, L"shaders/Solid.vs.cso");
    auto pvscb = pvs->GetBytecode(); //  this is ID3DBlob
    AddBind(std::move(pvs));

    AddBind(std::make_shared<PixelShader>(gfx, L"shaders/Solid.ps.cso"));

    struct PSColorConstant
    {
        dx::XMFLOAT3 color = {1.0f, 1.0f, 1.0f};
        float        padding; // for struct alighning in GRAphics memory
    } color_constant;
    AddBind(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, color_constant));

    const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
        {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

    AddBind(std::make_shared<InputLayout>(gfx, ied, pvscb));
    AddBind(std::make_shared<Topology>(gfx, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    m_pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}