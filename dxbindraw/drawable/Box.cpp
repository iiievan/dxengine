#include "Box.h"
#include "bindable/BindableBase.h"
#include "GraphicsThrowMacroses.h"
#include "geometry/Cube.h"

Box::Box(
    Graphics                              &gfx,
    std::mt19937                          &rng,
    std::uniform_real_distribution<float> &adist,
    std::uniform_real_distribution<float> &ddist,
    std::uniform_real_distribution<float> &odist,
    std::uniform_real_distribution<float> &rdist,
    std::uniform_real_distribution<float> &bdist,
    DirectX::XMFLOAT3 material)
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
            dx::XMFLOAT3 n;
        };
        auto model = Cube::MakeIndependent<Vertex>();
        model.SetNormalsIndependentFlat();

        AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));

        auto pvs = std::make_unique<VertexShader>(gfx, L"shaders/Phong.vs.cso");
        auto pvsbc = pvs->GetBytecode();

        AddStaticBind(std::move(pvs));
        AddStaticBind(std::make_unique<PixelShader>(gfx, L"shaders/Phong.ps.cso"));

        AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

        const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
        {
            { "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
            { "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
        };

        AddStaticBind(std::make_unique<InputLayout>(gfx, ied, pvsbc));
        AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
    }
    else
        SetIndexFromStatic();

    AddBind(std::make_unique<TransformCbuf>(gfx, *this));

    struct PSMaterialConstant
    {
        alignas(16)DirectX::XMFLOAT3 color;
        float specularIntencity = 0.6f;
        float specularPower = 30.0f;
        float padding[2];
    } colorConst;
    colorConst.color = material;
    AddBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx,colorConst,1u));

    // model deformation transform (per instance, not stored as bind)
    dx::XMStoreFloat3x3(&m_mt, dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng)));
}

void Box::Update(float dt) noexcept
{
    m_roll += m_droll * dt;
    m_pitch += m_dpitch * dt;
    m_yaw += m_dyaw * dt;
    m_theta += m_dtheta * dt;
    m_phi += m_dphi * dt;
    m_chi += m_dchi * dt;
}

DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
    {
        namespace dx = DirectX;
        return dx::XMLoadFloat3x3(&m_mt) *
               dx::XMMatrixRotationRollPitchYaw(m_pitch, m_yaw, m_roll) *            // 3. Локальное вращение
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
}