#ifndef __BOX_H
#define __BOX_H

#include <random>

#include "TestObject.h"
#include "bindable/ConstantBuffer.h"
#include "ConditionalNoexcept.h"


class Box : public TestObject<Box>
{
public:
    Box(Graphics &gfx,
        std::mt19937 &rng,
        std::uniform_real_distribution<float> &adist,
        std::uniform_real_distribution<float> &ddist,
        std::uniform_real_distribution<float> &odist,
        std::uniform_real_distribution<float> &rdist,
        std::uniform_real_distribution<float> &bdist,
        DirectX::XMFLOAT3 material);

    DirectX::XMMATRIX GetTransformXM() const noexcept override;
    bool SpawnControlWindow(int id, Graphics& gfx) noexcept;    // return false if findow is closed

private:

    void SyncMaterial(Graphics &gfx) noxnd;

    struct PSMaterialConstant
    {
        DirectX::XMFLOAT3 color;
        float specularIntencity = 0.6f;
        float specularPower = 30.0f;
        float padding[3];
    } m_MaterialConstants;
    using m_MaterialCbuf = Bind::PixelConstantBuffer<PSMaterialConstant>;

    DirectX::XMFLOAT3X3 m_mt; // model transform
};

#endif //__BOX_H
