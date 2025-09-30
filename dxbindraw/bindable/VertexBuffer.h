#ifndef __VERTEXBUFFER_H
#define __VERTEXBUFFER_H
#include "Bindable.h"
#include "GraphicsThrowMacroses.h"
#include "Vertex.h"

namespace Bind
{
    class VertexBuffer : public Bindable
    {
    public:

        VertexBuffer(Graphics &gfx, const Dvtx::VertexBuffer &vbuf);
        void Bind( Graphics& gfx ) noexcept override;

    protected:
        UINT                                 stride;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    };
}

#endif //__VERTEXBUFFER_H
