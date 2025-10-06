#ifndef __VERTEX_H
#define __VERTEX_H
#include <type_traits>
#include <vector>
#include "Graphics.h"
#include "Color.h"
#include "ConditionalNoexcept.h"

namespace Dvtx
{
    class VertexLayout
    {
    public:
        enum ElementType
        {
            Position2D,
            Position3D,
            Texture2D,
            Normal,
            Tangent,
            Bitangent,
            Float3Color,
            Float4Color,
            BGRAColor,
            Count
        };

        template<ElementType> struct Map;

        template<> struct Map<Position2D>
        {
            using SysType = DirectX::XMFLOAT2;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
            static constexpr const char* semantic = "Position";
            static constexpr  const char* code = "P2";
        };

        template<> struct Map<Position3D>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Position";
            static constexpr  const char* code = "P3";
        };

        template<> struct Map<Texture2D>
        {
            using SysType = DirectX::XMFLOAT2;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32_FLOAT;
            static constexpr const char* semantic = "Texcoord";
            static constexpr  const char* code = "T2";
        };

        template<> struct Map<Normal>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Normal";
            static constexpr  const char* code = "N";
        };

        template<> struct Map<Tangent>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Tangent";
            static constexpr  const char* code = "Nt";
        };

        template<> struct Map<Bitangent>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Bitangent";
            static constexpr  const char* code = "Nb";
        };

        template<> struct Map<Float3Color>
        {
            using SysType = DirectX::XMFLOAT3;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32_FLOAT;
            static constexpr const char* semantic = "Color";
            static constexpr  const char* code = "C3";
        };

        template<> struct Map<Float4Color>
        {
            using SysType = DirectX::XMFLOAT4;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
            static constexpr const char* semantic = "Color";
            static constexpr  const char* code = "C4";
        };

        template<> struct Map<BGRAColor>
        {
            using SysType = ::BGRAColor;
            static constexpr DXGI_FORMAT dxgiFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
            static constexpr const char* semantic = "Color";
            static constexpr  const char* code = "C8";
        };

        class Element
        {
        public:
            Element(ElementType type, size_t offset);

            static constexpr size_t SizeOf(ElementType type) NOXND
            {
                switch (type)
                {
                    case Position2D:
                        return sizeof(Map<Position2D>::SysType);
                    case Position3D:
                        return sizeof(Map<Position3D>::SysType);
                    case Texture2D:
                        return sizeof(Map<Texture2D>::SysType);
                    case Normal:
                        return sizeof(Map<Normal>::SysType);
                    case Tangent:
                        return sizeof(Map<Tangent>::SysType);
                    case Bitangent:
                        return sizeof(Map<Bitangent>::SysType);
                    case Float3Color:
                        return sizeof(Map<Float3Color>::SysType);
                    case Float4Color:
                        return sizeof(Map<Float4Color>::SysType);
                    case BGRAColor:
                        return sizeof(Map<BGRAColor>::SysType);
                }
                assert("Invalid element type" && false);
                return 0u;
            }

            D3D11_INPUT_ELEMENT_DESC GetDesc() const NOXND;
            size_t      Size() const NOXND { return SizeOf(m_type); }
            size_t      GetOffsetAfter() const NOXND { return m_offset + Size(); }
            size_t      GetOffset() const { return m_offset; }
            ElementType GetType() const noexcept { return m_type; }
            const char* GetCode() const noexcept;

        private:
            template<ElementType type>
            static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept
            {
                return {Map<type>::semantic,0,Map<type>::dxgiFormat,0,(UINT)offset, D3D11_INPUT_PER_VERTEX_DATA, 0};
            }

            ElementType m_type;
            size_t      m_offset;
        };

    public:
        template <ElementType Type>
        const Element &Resolve() const NOXND
        {
            for (auto &e : m_elements)
            {
                if (e.GetType() == Type)
                    return e;
            }
            assert("Could not resolve element type" && false);
            return m_elements.front();
        }

        const Element &ResolveByIndex(size_t i) const NOXND { return m_elements[i]; }
        size_t Size() const NOXND { return m_elements.empty() ? 0u : m_elements.back().GetOffsetAfter(); }
        size_t GetElementCount() const noexcept { return m_elements.size(); }
        VertexLayout &Append(ElementType type) NOXND;
        std::vector<D3D11_INPUT_ELEMENT_DESC> Get3DLayout() const NOXND;
        std::string GetCode() const NOXND;

    private:
        std::vector<Element> m_elements;
    };



    class Vertex
    {
        friend class VertexBuffer;

    public:
        template <VertexLayout::ElementType Type>
        auto &Attr() NOXND
        {
            auto        pAttribute = m_pData + m_layout.Resolve<Type>().GetOffset();
            return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType *>(pAttribute);
        }

        template <typename T>
        void SetAttributeByIndex(size_t i, T &&val) NOXND
        {
            const auto &element = m_layout.ResolveByIndex(i);
            auto        pAttribute = m_pData + element.GetOffset();
            switch (element.GetType())
            {
                case VertexLayout::Position2D:
                    SetAttribute<VertexLayout::Position2D>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Position3D:
                    SetAttribute<VertexLayout::Position3D>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Texture2D:
                    SetAttribute<VertexLayout::Texture2D>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Normal:
                    SetAttribute<VertexLayout::Normal>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Tangent:
                    SetAttribute<VertexLayout::Tangent>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Bitangent:
                    SetAttribute<VertexLayout::Bitangent>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Float3Color:
                    SetAttribute<VertexLayout::Float3Color>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::Float4Color:
                    SetAttribute<VertexLayout::Float4Color>(pAttribute, std::forward<T>(val));
                    break;
                case VertexLayout::BGRAColor:
                    SetAttribute<VertexLayout::BGRAColor>(pAttribute, std::forward<T>(val));
                    break;
                default:
                    assert("Bad element type" && false);
            }
        }

    protected:
        Vertex(char *pData, const VertexLayout &layout) NOXND;

    private:
        // enables parameter pack setting of multiple parameters by element index( more than one!!!)
        template <typename First, typename... Rest>
        void SetAttributeByIndex(size_t i, First &&first, Rest &&...rest) NOXND
        {
            SetAttributeByIndex(i, std::forward<First>(first));
            SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
        }

        // helper to reduce code duplication in SetAttributeByIndex
        template <VertexLayout::ElementType DestLayoutType, typename SrcType>
        void SetAttribute(char *pAttribute, SrcType &&val) NOXND
        {
            using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;
            if constexpr (std::is_assignable<Dest, SrcType>::value)
                *reinterpret_cast<Dest *>(pAttribute) = val;
            else
                assert("Parameter attribute type mismatch" && false);
        }

    private:
        char               *m_pData = nullptr;
        const VertexLayout &m_layout;
    };

    class ConstVertex
    {
    public:
        ConstVertex(const Vertex &v) NOXND;

        template <VertexLayout::ElementType Type>
        const auto &Attr() const NOXND
        {
            return const_cast<Vertex &>(m_vertex).Attr<Type>();
        }

    private:
        Vertex m_vertex;
    };

    class VertexBuffer
    {
    public:
        VertexBuffer(VertexLayout layout, size_t size = 0u) NOXND;

        const VertexLayout &GetLayout() const noexcept { return m_layout; }
        const char* GetGata() const NOXND { return m_buffer.data(); }
        void Resize(size_t newSize) NOXND; // resize in vertices num
        size_t Size() const NOXND { return m_buffer.size() / m_layout.Size(); } // return this size IS NOT of bytes, but returns size in vertices num.
        size_t SizeBytes() const NOXND { return m_buffer.size(); }

        template <typename... Params>
        void EmplaceBack(Params &&...params) NOXND
        {
            assert(
                sizeof...(params) == m_layout.GetElementCount() && "Param count doesn't match number of vertex elements");
            m_buffer.resize(m_buffer.size() + m_layout.Size());
            Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
        }

        Vertex Back() NOXND;
        Vertex Front() NOXND;
        Vertex operator[](size_t i) NOXND;
        ConstVertex Back() const NOXND { return const_cast<VertexBuffer *>(this)->Back(); }
        ConstVertex Front() const NOXND { return const_cast<VertexBuffer *>(this)->Front(); }
        ConstVertex operator[](size_t i) const NOXND { return const_cast<VertexBuffer &>(*this)[i]; }

    private:
        std::vector<char> m_buffer;
        VertexLayout      m_layout;
    };
} // namespace hw3dexp

#endif //__VERTEX_H
