#ifndef __VERTEXBUFFER_H
#define __VERTEXBUFFER_H
#include "Bindable.h"
#include "GraphicsThrowMacroses.h"

class VertexBuffer : public Bindable
{
public:
    template <class V>
    VertexBuffer(Graphics &gfx, const std::vector<V> &vertices) : stride(sizeof(V))
    {
        INFOMAN(gfx);

        D3D11_BUFFER_DESC bd = {};
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = UINT(sizeof(V) * vertices.size());
        bd.StructureByteStride = sizeof(V);

        D3D11_SUBRESOURCE_DATA sd = {};
        sd.pSysMem = vertices.data();

        GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bd, &sd, &pVertexBuffer));
    }

    void Bind( Graphics& gfx ) noexcept override;

protected:
    UINT                                 stride;
    Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};

#endif //__VERTEXBUFFER_H
