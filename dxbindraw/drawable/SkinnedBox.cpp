#include "SkinnedBox.h"
#include "GraphicsThrowMacroses.h"
#include "Surface.h"
#include "Utils.hpp"
#include "bindable/BindableBase.h"
#include "bindable/Texture.h"
#include "geometry/Cube.h"
#include "bindable/Sampler.h"

SkinnedBox::SkinnedBox(
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
        const auto model = Cube::MakeSkinned<Vertex>();

        std::string cubetex_path = findFullPath("cube.png");
        AddStaticBind(std::make_unique<Texture>(gfx, Surface::FromFile(cubetex_path)));

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
void SkinnedBox::Update(float dt) noexcept
{
    m_roll += m_droll * dt;
    m_pitch += m_dpitch * dt;
    m_yaw += m_dyaw * dt;
    m_theta += m_dtheta * dt;
    m_phi += m_dphi * dt;
    m_chi += m_dchi * dt;
}

DirectX::XMMATRIX SkinnedBox::GetTransformXM() const noexcept
{
    namespace dx = DirectX;
    return  dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *            // 3. Локальное вращение
            // pitch (Тангаж) = наклон вперед/назад
            // yaw (Рысканье) = поворот вокруг вертикали
            // roll (Крен) = наклон влево/вправо
            dx::XMMatrixTranslation(m_r, 0.0f, 0.0f) *       // 2. Смещение от центра
            // m_r = расстояние от центральной точки
            dx::XMMatrixRotationRollPitchYaw(m_theta, m_phi, m_chi);     // 1. Орбитальное вращение
    // theta = вращение вокруг оси Y (как рысканье вокруг точки)
    // phi = вращение вокруг оси X (как тангаж вокруг точки)
    // chi = вращение вокруг оси Y (как крен вокруг точки)

    /*
    1. Назначение: Орбитальное вращение вокруг точки (0,0,20)
       Эффект: Объект вращается вокруг смещенного центра
    2. Назначение: Радиальное смещение от центра орбиты
       Эффект: Определяет расстояние от центра вращения до объекта
    3. Назначение: Локальное вращение объекта вокруг собственного центра
       Эффект: Объект вращается вокруг своей оси

    Представьте себе солнечную систему:
    Translation(0,0,20) - Солнце в точке (0,0,20)
    Rotation(m_theta, m_phi, m_chi) - Земля вращается вокруг Солнца
    Translation(m_r,0,0) - Расстояние от Солнца до Земли
    Rotation(m_pitch, m_yaw, m_roll) - Земля вращается вокруг своей оси
     */
}