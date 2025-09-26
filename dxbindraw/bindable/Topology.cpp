#include "Topology.h"

namespace Bind
{
    Topology::Topology(Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY topology) : type(topology) {}

    void Topology::Bind(Graphics &gfx) noexcept
    {
        GetContext(gfx)->IASetPrimitiveTopology(type);
    }
} // namespace Bind