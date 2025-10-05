#ifndef __TRANSFORMCBUF_H
#define __TRANSFORMCBUF_H

#include <DirectXMath.h>
#include "ConstantBuffer.h"
#include "drawable/Drawable.h"

namespace Bind
{
    class TransformCbuf : public Bindable
    {
    public:
        TransformCbuf(Graphics &gfx, const Drawable &parent, UINT slot = 0u);
        void Bind(Graphics &gfx) noexcept override;

    protected:
        struct Transforms
        {
            DirectX::XMMATRIX modelView;
            DirectX::XMMATRIX modelViewProj;
        };

        void UpdateBindImpl(Graphics &gfx, const Transforms& tf) noexcept;
        Transforms GetTransforms(Graphics &gfx) noexcept;

    private:
        static std::unique_ptr<VertexConstantBuffer<Transforms>> m_pVcbuf;
        const Drawable                         &m_parent;
    };
}

#endif //__TRANSFORMCBUF_H
