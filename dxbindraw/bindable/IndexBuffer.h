#ifndef __INDEXBUFFER_H
#define __INDEXBUFFER_H

#include "Bindable.h"

namespace Bind
{
    class IndexBuffer : public Bindable
    {
    public:
        IndexBuffer(Graphics &gfx, const std::vector<unsigned short> &indices);
        IndexBuffer(Graphics &gfx, std::string tag, const std::vector<unsigned short> &indices);

        void                             Bind(Graphics &gfx) noexcept override;
        UINT                             GetCount() const noexcept { return count; }
        static std::shared_ptr<Bindable> Resolve(Graphics &                         gfx,
                                                 const std::string &                tag,
                                                 const std::vector<unsigned short> &indices);

        template <typename... Ignore>
        static std::string GenerateUID(const std::string &tag, Ignore &&... ignore) { return m_GenerateUID(tag); }

        std::string GetUID() const noexcept override;

    protected:
        std::string                          tag;
        UINT                                 count;
        Microsoft::WRL::ComPtr<ID3D11Buffer> pIndexBuffer;

    private:
        static std::string m_GenerateUID(const std::string &tag);
    };
}

#endif //__INDEXBUFFER_H
