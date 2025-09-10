#ifndef __POINTLIGHT_H
#define __POINTLIGHT_H

#include "Graphics.h"
#include "drawable/SolidSphere.h"
#include "bindable/ConstantBuffer.h"

class PointLight
{
public:
    PointLight(Graphics &gfx, float radius = 0.5f);
    void SpawnControlWindow() noexcept;
    void Reset() noexcept;
    void Draw(Graphics &gfx) const noexcept(!IS_DEBUG);
    void Bind(Graphics &gfx) const noexcept;
private:
    struct PointLightCbuf
    {
        alignas(16) DirectX::XMFLOAT3 pos;
        alignas(16) DirectX::XMFLOAT3 ambient;
        alignas(16) DirectX::XMFLOAT3 diffuseColor;
        float diffuseIntensity;
        float attConst;
        float attLin;
        float attQuad;
    };
private:
    PointLightCbuf m_cbData;
    mutable SolidSphere m_mesh;
    mutable PixelConstantBuffer<PointLightCbuf> m_cbuf;
};



#endif //__POINTLIGHT_H
