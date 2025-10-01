#ifndef __INPUTLAYOUT_H
#define __INPUTLAYOUT_H

#include "Bindable.h"
#include "Vertex.h"

namespace Bind
{
    class InputLayout : public Bindable
    {
    public:
        InputLayout(Graphics &gfx,
                    Dvtx::VertexLayout layout_in,
                    ID3DBlob *pVSBytecode);

        void Bind(Graphics &gfx) noexcept override;
        static std::shared_ptr<InputLayout> Resolve(Graphics &gfx, const Dvtx::VertexLayout &layout, ID3DBlob* pVSBytecode);
        static std::string GenerateUID(const Dvtx::VertexLayout &layout, ID3DBlob* pVSBytecode = nullptr);
        std::string GetUID() const noexcept override;

    protected:
        Dvtx::VertexLayout layout;
        Microsoft::WRL::ComPtr<ID3D11InputLayout> pInputLayout;
    };
}

#endif //__INPUTLAYOUT_H
