#ifndef __SAMPLER_H
#define __SAMPLER_H

#include "Bindable.h"

namespace Bind
{
    class Sampler : public Bindable
    {
    public:
        Sampler(Graphics &gfx);
        void Bind(Graphics &gfx) noexcept override;
        static std::shared_ptr<Bindable> Resolve(Graphics &gfx);
        static std::string GenerateUID();
        std::string GetUID() const noexcept override;

    protected:
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pSampler;
    };
}



#endif //__SAMPLER_H
