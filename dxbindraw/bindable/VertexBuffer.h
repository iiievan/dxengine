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
        VertexBuffer(Graphics &gfx, const std::string& tag, const Dvtx::VertexBuffer &vbuf);

        void Bind( Graphics& gfx ) noexcept override;
        static std::shared_ptr<VertexBuffer> Resolve(Graphics &gfx,const std::string &tag, const Dvtx::VertexBuffer &vbuf);
        template<typename... Ignore>
        static std::string GenerateUID(const std::string &tag, Ignore&&... ignore) {  return m_GenerateUID(tag); }
        std::string GetUID() const noexcept override { return GenerateUID(tag); }

    private:
        static std::string m_GenerateUID(const std::string &tag);

    protected:
        std::string                          tag;
        UINT                                 stride;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
    };
}

#endif //__VERTEXBUFFER_H
