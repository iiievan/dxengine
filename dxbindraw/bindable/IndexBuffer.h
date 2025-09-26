#ifndef __INDEXBUFFER_H
#define __INDEXBUFFER_H

#include "Bindable.h"

namespace Bind
{
    class IndexBuffer : public Bindable
    {
    public:
        IndexBuffer(Graphics &gfx, const std::vector<unsigned short> &indices);
        void Bind(Graphics &gfx) noexcept override;
        UINT GetCount() const noexcept { return count; }

    protected:
        UINT                                 count;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;
    };
}

#endif //__INDEXBUFFER_H
