#ifndef __TESTPLANE_H
#define __TESTPLANE_H
#include "Drawable.h"



class TestPlane : public Drawable
{
public:
    TestPlane(Graphics &gfx, float size);
    void SetPos (DirectX::XMFLOAT3 pos ) noexcept;
    void SetRotation( float roll, float pitch, float yaw ) noexcept;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;

private:
    DirectX::XMFLOAT3 m_pos = {1.0f, 1.0f, 1.0f};
    float m_roll = 0.0f;
    float m_pitch = 0.0f;
    float m_yaw = 0.0f;

};



#endif //__TESTPLANE_H
