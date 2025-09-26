#ifndef __POINTLIGHT_H
#define __POINTLIGHT_H

#include "Graphics.h"
#include "drawable/SolidSphere.h"
#include "bindable/ConstantBuffer.h"
#include "ConditionalNoexcept.h"

class PointLight
{
public:
    PointLight(Graphics &gfx, float radius = 0.5f);
    void SpawnControlWindow() noexcept;
    void Reset() noexcept;
    void Draw(Graphics &gfx) const NOXND;
    void Bind(Graphics &gfx, DirectX::FXMMATRIX view) const noexcept;
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
    mutable Bind::PixelConstantBuffer<PointLightCbuf> m_cbuf;
};



#endif //__POINTLIGHT_H
