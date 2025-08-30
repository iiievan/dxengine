#include "Melon.h"
#include "bindable/BindableBase.h"
#include "GraphicsThrowMacroses.h"
#include "geometry/Sphere.h"

Melon::Melon(
    Graphics                              &gfx,
    std::mt19937                          &rng,
    std::uniform_real_distribution<float> &adist,
    std::uniform_real_distribution<float> &ddist,
    std::uniform_real_distribution<float> &odist,
    std::uniform_real_distribution<float> &rdist,
    std::uniform_int_distribution<int>& longdist,
    std::uniform_int_distribution<int>& latdist)
    : m_r(rdist(rng)),
      m_droll(ddist(rng)),
      m_dpitch(ddist(rng)),
      m_dyaw(ddist(rng)),
      m_dphi(odist(rng)),
      m_dtheta(odist(rng)),
      m_dchi(odist(rng)),
      m_chi(adist(rng)),
      m_theta(adist(rng)),
      m_phi(adist(rng))
{
    namespace dx = DirectX;

    if (!IsStaticInitialized())
    {
        auto pvs = std::make_unique<VertexShader>(gfx, L"shaders/ColorIndex.vs.cso");
        auto pvsbc = pvs->GetBytecode();
        AddStaticBind(std::move(pvs));

        AddStaticBind(std::make_unique<PixelShader>(gfx, L"shaders/ColorIndex.ps.cso"));

        struct PixelShaderConstants
        {
            struct
            {
                float r;
                float g;
                float b;
                float a;
            } face_colors[8];
        };
        const PixelShaderConstants cb2 = {{
            {1.0f, 1.0f, 1.0f},
            {1.0f, 0.0f, 0.0f},
            {0.0f, 1.0f, 0.0f},
            {1.0f, 1.0f, 0.0f},
            {0.0f, 0.0f, 1.0f},
            {1.0f, 0.0f, 1.0f},
            {0.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
        }};
        AddStaticBind(std::make_unique<PixelConstantBuffer<PixelShaderConstants>>(gfx, cb2));

        const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
            {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

        AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }

    struct Vertex
    {
        dx::XMFLOAT3 pos;
    };
    auto model = Sphere::MakeTesselated<Vertex>(latdist(rng), longdist(rng));
    // deform vertices of model by linear transformation
    model.Transform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));

    AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

    AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

    AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Melon::Update(float dt) noexcept
{
    m_roll += m_droll * dt;
    m_pitch += m_dpitch * dt;
    m_yaw += m_dyaw * dt;
    m_theta += m_dtheta * dt;
    m_phi += m_dphi * dt;
    m_chi += m_dchi * dt;
}

DirectX::XMMATRIX Melon::GetTransformXM() const noexcept
{
    namespace dx = DirectX;
    return dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) * dx::XMMatrixTranslation(m_r, 0.0f, 0.0f)
           * dx::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}