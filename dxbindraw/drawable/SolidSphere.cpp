#include "SolidSphere.h"
#include "GraphicsThrowMacroses.h"
#include "bindable/BindableCommon.h"
#include "geometry/Sphere.h"

SolidSphere::SolidSphere(Graphics &gfx, float radius)
{
    using namespace Bind;
    namespace dx = DirectX;

    if (!IsStaticInitialized())
    {
        struct Vertex
        {
            dx::XMFLOAT3 pos;
        };

        auto model = Sphere::Make<Vertex>();
        model.Transform(dx::XMMatrixScaling(radius, radius, radius));
        AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
        AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

        auto pvs = std::make_unique<VertexShader>(gfx, L"shaders/Solid.vs.cso");
        auto pvscb = pvs->GetBytecode(); //  this is ID3DBlob
        AddStaticBind(std::move(pvs));

        AddStaticBind(std::make_unique<PixelShader>(gfx, L"shaders/Solid.ps.cso"));

        struct PSColorConstant
        {
            dx::XMFLOAT3 color = {1.0f, 1.0f, 1.0f};
            float        padding; // for struct alighning in GRAphics memory
        } color_constant;
        AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, color_constant));

        const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
            {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0}};

        AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvscb));
    }
    else
        SetIndexFromStatic();

    AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void SolidSphere::Update(float dt) noexcept {}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
    m_pos = pos;
}

DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
    return DirectX::XMMatrixTranslation(m_pos.x, m_pos.y, m_pos.z);
}