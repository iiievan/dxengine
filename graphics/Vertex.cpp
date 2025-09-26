#include "Vertex.h"

namespace Dvtx
{
    VertexLayout::Element::Element(ElementType type, size_t offset)
    : m_type(type), m_offset(offset)
    {}

    D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const NOXND
    {
        switch (m_type)
        {
            case Position2D:
                return GenerateDesc<Position2D>(GetOffset());
            case Position3D:
                return GenerateDesc<Position3D>(GetOffset());
            case Texture2D:
                return GenerateDesc<Texture2D>(GetOffset());
            case Normal:
                return GenerateDesc<Normal>(GetOffset());
            case Float3Color:
                return GenerateDesc<Float3Color>(GetOffset());
            case Float4Color:
                return GenerateDesc<Float4Color>(GetOffset());
            case BGRAColor:
                return GenerateDesc<BGRAColor>(GetOffset());
        }
        assert("Invalid element type" && false);
        return {"INVALID", 0, DXGI_FORMAT_UNKNOWN, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
    }

    VertexLayout &VertexLayout::Append(ElementType type) NOXND
    {
        m_elements.emplace_back(type, Size());
        return *this;
    }

    std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::Get3DLayout() const NOXND
    {
        std::vector<D3D11_INPUT_ELEMENT_DESC> result;
        result.reserve(GetElementCount());
        for (const auto &e : m_elements)
        {
            result.push_back(e.GetDesc());
        }
        return result;
    }

    Vertex::Vertex(char *pData, const VertexLayout &layout) NOXND : m_pData(pData), m_layout(layout)
    {
        assert(pData != nullptr);
    }

    ConstVertex::ConstVertex(const Vertex &v) NOXND
    : m_vertex(v)
    {}

    VertexBuffer::VertexBuffer(VertexLayout layout) NOXND
    : m_layout(std::move(layout))
    {}

    Vertex VertexBuffer::Back() NOXND
    {
        assert(m_buffer.size() != 0u);
        return Vertex {m_buffer.data() + m_buffer.size() - m_layout.Size(), m_layout};
    }

    Vertex VertexBuffer::Front() NOXND
    {
        assert(m_buffer.size() != 0u);
        return Vertex {m_buffer.data(), m_layout};
    }

    Vertex VertexBuffer::operator[](size_t i) NOXND
    {
        assert(i < Size());
        return Vertex {m_buffer.data() + m_layout.Size() * i, m_layout};
    }
} // namespace Dvtx
