#ifndef __SOLIDSPHERE_H
#define __SOLIDSPHERE_H
#include "DrawableBase.h"


class SolidSphere : public DrawableBase<SolidSphere>
{
public:
    SolidSphere(Graphics &gfx, float radius);
    void Update(float dt) noexcept override;
    void SetPos(DirectX::XMFLOAT3 pos) noexcept;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
    DirectX::XMFLOAT3 m_pos = {1.0f,1.0f,1.0f};
};

#endif //__SOLIDSPHERE_H
