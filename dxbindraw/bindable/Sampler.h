#ifndef __SAMPLER_H
#define __SAMPLER_H

#include "Bindable.h"

class Sampler : public Bindable
{
public:
    Sampler(Graphics &gfx);
    void Bind(Graphics &gfx) noexcept override;
protected:
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
};



#endif //__SAMPLER_H
