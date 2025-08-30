#include "Pyramid.h"
#include "GraphicsThrowMacroses.h"
#include "bindable/BindableBase.h"
#include "geometry/Cone.h"

Pyramid::Pyramid(
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

    if( !IsStaticInitialized() )
    {
        struct Vertex
        {
            dx::XMFLOAT3 pos;
            struct
            {
                unsigned char r;
                unsigned char g;
                unsigned char b;
                unsigned char a;
            } color;
        };
        auto model = Cone::MakeTesselated<Vertex>( 4 );
        // set vertex colors for mesh
        model.vertices[0].color = { 255,255,0 };
        model.vertices[1].color = { 255,255,0 };
        model.vertices[2].color = { 255,255,0 };
        model.vertices[3].color = { 255,255,0 };
        model.vertices[4].color = { 255,255,80 };
        model.vertices[5].color = { 255,10,0 };
        // deform mesh linearly
        model.Transform( dx::XMMatrixScaling( 1.0f,1.0f,0.7f ) );

        AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );

        auto pvs = std::make_unique<VertexShader>( gfx,L"shaders/ColorBlend.vs.cso" );
        auto pvsbc = pvs->GetBytecode();
        AddStaticBind( std::move( pvs ) );

        AddStaticBind( std::make_unique<PixelShader>( gfx,L"shaders/ColorBlend.ps.cso" ) );

        AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

        const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
        {
            { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
            { "Color",0,DXGI_FORMAT_R8G8B8A8_UNORM,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
        };
        AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

        AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
    }
    else
        SetIndexFromStatic();


    AddBind( std::make_unique<TransformCbuf>( gfx,*this ) );
}

void Pyramid::Update(float dt) noexcept
{
    m_roll += m_droll * dt;
    m_pitch += m_dpitch * dt;
    m_yaw += m_dyaw * dt;
    m_theta += m_dtheta * dt;
    m_phi += m_dphi * dt;
    m_chi += m_dchi * dt;
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
    namespace dx = DirectX;
    return dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) * dx::XMMatrixTranslation(m_r, 0.0f, 0.0f)
           * dx::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi) * dx::XMMatrixTranslation(0.0f, 0.0f, 20.0f);
}