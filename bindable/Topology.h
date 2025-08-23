#ifndef __TOPOLOGY_H
#define __TOPOLOGY_H
#include "Bindable.h"

class Topology :public Bindable
{
public:
    Topology(Graphics &gfx, D3D11_PRIMITIVE_TOPOLOGY topology);
    void Bind(Graphics &gfx) noexcept override;
protected:
    D3D11_PRIMITIVE_TOPOLOGY type;
};


#endif //__TOPOLOGY_H
