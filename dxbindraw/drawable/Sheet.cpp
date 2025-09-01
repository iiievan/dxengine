#include "Sheet.h"
#include "GraphicsThrowMacroses.h"
#include "Surface.h"
#include "Utils.hpp"
#include "bindable/BindableBase.h"
#include "bindable/Sampler.h"
#include "bindable/Texture.h"
#include "geometry/Plane.h"

Sheet::Sheet(
    Graphics                              &gfx,
    std::mt19937                          &rng,
    std::uniform_real_distribution<float> &adist,
    std::uniform_real_distribution<float> &ddist,
    std::uniform_real_distribution<float> &odist,
    std::uniform_real_distribution<float> &rdist)
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
        struct Vertex
        {
            dx::XMFLOAT3 pos;
            struct
            {
                float u;
                float v;
            } tex;
        };

        auto model = Plane::Make<Vertex>();
        model.vertices[0].tex = {0.0f, 0.0f};
        model.vertices[1].tex = {1.0f, 0.0f};
        model.vertices[2].tex = {0.0f, 1.0f};
        model.vertices[3].tex = {1.0f, 1.0f};

        std::string kappa50_path = findFullPath("kappa50.png");
        AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile(kappa50_path)));

        AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

        AddStaticBind(std::make_unique<Sampler>(gfx));

        auto pvs = std::make_unique<VertexShader>(gfx, L"shaders/Texture.vs.cso");
        auto pvsbc = pvs->GetBytecode();
        AddStaticBind(std::move(pvs));

        AddStaticBind(std::make_unique<PixelShader>(gfx, L"shaders/Texture.ps.cso"));

        AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

        const std::vector<D3D11_INPUT_ELEMENT_DESC> ied = {
            {"Position", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
            {"TexCoord", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
        };
        AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));

        AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }
    else
        SetIndexFromStatic();

    AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}

void Sheet::Update(float dt) noexcept
{
    m_roll += m_droll * dt;
    m_pitch += m_dpitch * dt;
    m_yaw += m_dyaw * dt;
    m_theta += m_dtheta * dt;
    m_phi += m_dphi * dt;
    m_chi += m_dchi * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
    namespace dx = DirectX;
    return dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) * dx::XMMatrixTranslation(m_r, 0.0f, 0.0f)
           * dx::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}