#ifndef __SOLIDSPHERE_H
#define __SOLIDSPHERE_H
#include "Drawable.h"


class SolidSphere : public Drawable
{
public:
    SolidSphere(Graphics &gfx, float radius);
    void SetPos(DirectX::XMFLOAT3 pos) noexcept;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    DirectX::XMFLOAT3 m_pos = {1.0f,1.0f,1.0f};
};

#endif //__SOLIDSPHERE_H
