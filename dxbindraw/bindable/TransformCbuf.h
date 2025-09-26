#ifndef __TRANSFORMCBUF_H
#define __TRANSFORMCBUF_H

#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "drawable/Drawable.h"

namespace Bind
{
    class TransformCbuf : public Bindable
    {
    private:
        struct Transforms
        {
            DirectX::XMMATRIX modelView;
            DirectX::XMMATRIX modelViewProj;
        };
    public:
        TransformCbuf(Graphics &gfx, const Drawable &parent, UINT slot = 0u);

        void Bind(Graphics &gfx) noexcept override;

    private:
        static std::unique_ptr<VertexConstantBuffer<Transforms>> m_pVcbuf;
        const Drawable                         &m_parent;
    };
}

#endif //__TRANSFORMCBUF_H
