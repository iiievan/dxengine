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
        DirectX::XMFLOAT3 pos;
        float padding;      // adds up to 16 bytes
    };
private:
    DirectX::XMFLOAT3 m_pos = {0.0f,0.0f,0.0f};
    mutable SolidSphere m_mesh;
    mutable PixelConstantBuffer<PointLightCbuf> m_cbuf;
};



#endif //__POINTLIGHT_H
