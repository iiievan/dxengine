#ifndef __VEC4MODEL_H
#define __VEC4MODEL_H

#include <d3d11.h>
#include <directxmath.h>
#include "utils/Utils.hpp"

using namespace DirectX;

class vec4model
{
private:
    struct SimpleVertex
    {
        XMFLOAT4 Position;
        XMFLOAT4 Color;
    };

public:
    vec4model(const XMFLOAT3 &end, const XMFLOAT3 &start = XMFLOAT3(0.0f, 0.0f, 0.0f));

    vec4model(const vec4model &) = delete;
    vec4model &operator=(const vec4model &) = delete;
    vec4model(vec4model &&) noexcept;
    vec4model &operator=(vec4model &&) noexcept;

    ~vec4model() { ShutdownBuffers(); }

    bool Initialize(ID3D11Device *, ID3D11DeviceContext *);

    void setStart(ID3D11DeviceContext *dc, const XMFLOAT3 &start);
    void setEnd(ID3D11DeviceContext *dc, const XMFLOAT3 &end);
    void setColor(ID3D11DeviceContext *dc, const XMFLOAT4 &color);
    void setColor(ID3D11DeviceContext *dc, float _x, float _y, float _z, float _w);
    void setColor(ID3D11DeviceContext *dc,DEBUG_COLORS color = DBG_SCARLET);
    void setArrowProperties(float headLength = 0.2f, float headWidth = 0.05f);

    int             getIndexCount() { return m_indexCount; }
    inline XMVECTOR getStart() const noexcept { return XMLoadFloat4(&m_start); }
    inline XMVECTOR getEnd() const noexcept { return XMLoadFloat4(&m_end); }
    inline XMVECTOR getDirection()
    {
        XMVECTOR vS = getStart();
        XMVECTOR vE = getEnd();
        XMVECTOR direction = XMVectorSubtract(vE, vS);

        return direction;
    }
    XMVECTOR getDirectionNorm();
    float    getLength();
    float    getLengthSq();

    void Shutdown() { ShutdownBuffers(); }
    void Render(ID3D11DeviceContext *deviceContext) { RenderBuffers(deviceContext); }

    bool UpdateBuffers(ID3D11DeviceContext *deviceContext);
    bool UpdatePositions(ID3D11DeviceContext *deviceContext);
    bool UpdateColors(ID3D11DeviceContext *deviceContext);

private:
    bool InitializeBuffers(ID3D11Device *);
    void m_FillVertices(SimpleVertex *vertices,bool upd_vertices, bool upd_color);

    void ShutdownBuffers();

    void RenderBuffers(ID3D11DeviceContext *);

private:
    ID3D11Buffer *m_vertexBuffer;
    ID3D11Buffer *m_indexBuffer;

    const int m_vertexCount {4}; // 2 for Arrow
    const int m_indexCount {5};  // 3 indexes for Arrow

    XMFLOAT4 m_start;
    XMFLOAT4 m_end;
    XMFLOAT4 m_color;

    float m_arrow_length {0.2f};
    float m_arrow_width {0.05f};
};

#endif //__VEC4MODEL_H