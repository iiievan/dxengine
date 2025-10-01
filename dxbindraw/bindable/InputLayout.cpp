#include "InputLayout.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacroses.h"

namespace Bind
{
    InputLayout::InputLayout(
        Graphics &         gfx,
        Dvtx::VertexLayout layout_in,
        ID3DBlob *         pVSBytecode)
        : layout(std::move(layout_in))
    {
        INFOMAN(gfx);

        const auto d3dlayout = layout.Get3DLayout();

        GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(d3dlayout.data(),
                                                                            (UINT)d3dlayout.size(),
                                                                            pVSBytecode->GetBufferPointer(),
                                                                            pVSBytecode->GetBufferSize(),
                                                                            &pInputLayout));
    }

    void InputLayout::Bind(Graphics &gfx) noexcept
    {
        GetContext(gfx)->IASetInputLayout(pInputLayout.Get());
    }

    std::shared_ptr<Bindable> InputLayout::Resolve(
        Graphics &gfx,
        const Dvtx::VertexLayout &layout,
        ID3DBlob *pVSBytecode)
    {
        return Codex::Resolve<InputLayout>(gfx, layout, pVSBytecode);
    }

    std::string InputLayout::GenerateUID(const Dvtx::VertexLayout &layout, ID3DBlob *pVSBytecode)
    {
        using namespace std::string_literals;
        return typeid(InputLayout).name() + "#"s + layout.GetCode();
    }

    std::string InputLayout::GetUID() const noexcept
    {
        return GenerateUID(layout);
    }
}