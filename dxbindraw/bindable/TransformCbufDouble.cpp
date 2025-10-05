#include "TransformCbufDouble.h"

namespace Bind
{
    TransformCbufDouble::TransformCbufDouble(Graphics &gfx, const Drawable &parent, UINT slotV, UINT slotP)
        : TransformCbuf(gfx, parent, slotV)
    {
        if (!m_pPCbuf)
            m_pPCbuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
    }

    void TransformCbufDouble::Bind(Graphics &gfx) noexcept
    {
        const auto tf = GetTransforms(gfx);
        TransformCbuf::UpdateBindImpl(gfx, tf);
        UpdateBindImpl(gfx, tf);
    }

    void TransformCbufDouble::UpdateBindImpl(Graphics &gfx, const Transforms &tf) noexcept
    {
        m_pPCbuf->Update(gfx, tf);
        m_pPCbuf->Bind(gfx);
    }

    std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> TransformCbufDouble::m_pPCbuf;
}
