#include "TransformCbuf.h"

namespace Bind
{
    TransformCbuf::TransformCbuf(Graphics &gfx, const Drawable &parent, UINT slot)
        : m_parent(parent)
    {
        if (!m_pVcbuf)
            m_pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
    }

    void TransformCbuf::Bind(Graphics &gfx) noexcept
    {
        const auto modelView = m_parent.GetTransformXM() * gfx.GetCamera();

        const Transforms tf =
        {
            DirectX::XMMatrixTranspose(modelView),
            DirectX::XMMatrixTranspose(modelView * gfx.GetProjection())
        };

        m_pVcbuf->Update(gfx,tf);
        m_pVcbuf->Bind(gfx);
    }

    std::unique_ptr<VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::m_pVcbuf;
}