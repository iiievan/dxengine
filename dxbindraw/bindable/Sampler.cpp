#include "Sampler.h"
#include "GraphicsThrowMacroses.h"

Sampler::Sampler(Graphics &gfx)
{
    INFOMAN(gfx);
    /* сэмплер разработан для того чтобы объяснять какие эффекты фильтрации и как обрабатывать данные пикселей для текстуры.
    Фильтрация (Filter)
    Когда текстура отображается не в натуральную величину, видеокарта должна решить:
    Увеличение (Magnification) - берутся несколько соседних текселей и интерполируются
    Уменьшение (Minification) - выбираются или усредняются тексели из мип-уровней
    Обработка границ (Addressing)
    Что делать, если шейдер запрашивает координаты за пределами [0, 1]:
    Wrap - повторять текстуру
    Clamp - растягивать крайние пиксели
    Mirror - зеркально отражать
    */
    D3D11_SAMPLER_DESC sampdesc = {};
    sampdesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;  // Как фильтровать
    sampdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;     // Что делать за границами текстуры
    sampdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    sampdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

    GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&sampdesc, &m_pSampler));
}

void Sampler::Bind(Graphics &gfx) noexcept
{
    GetContext(gfx)->PSSetSamplers(0u, 1u, m_pSampler.GetAddressOf());
}
