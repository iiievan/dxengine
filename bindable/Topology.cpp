#include "Topology.h"
Topology::Topology(Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY topology)
    :type(topology)
{

}

void Topology::Bind(Graphics &gfx) noexcept
{
    GetContext(gfx)->IASetPrimitiveTopology(type);
}