#ifndef __SHEET_H
#define __SHEET_H

#include "DrawableBase.h"

class Sheet : public DrawableBase<Sheet>
{
public:
    Sheet( Graphics& gfx,std::mt19937& rng,
        std::uniform_real_distribution<float>& adist,
        std::uniform_real_distribution<float>& ddist,
        std::uniform_real_distribution<float>& odist,
        std::uniform_real_distribution<float>& rdist );
    void Update( float dt ) noexcept override;
    DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
    // positional
    float m_r;
    float m_roll {0.0f};
    float m_pitch {0.0f};
    float m_yaw {0.0f};
    float m_theta;
    float m_phi;
    float m_chi;

    // speed(delta/s)
    float m_droll;      // rotation around model center
    float m_dpitch;     // rotation around model center
    float m_dyaw;       // rotation around model center
    float m_dtheta;     // rotation around world space
    float m_dphi;       // rotation around world space
    float m_dchi;       // rotation around world space
};



#endif //__SHEET_H
