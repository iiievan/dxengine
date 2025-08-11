#include "vec4model.hpp"

vec4model::vec4model(const XMFLOAT3 &end, const XMFLOAT3 &start)
{
    m_start = XMFLOAT4(start.x, start.y, start.z, 1.0f);
    m_end = XMFLOAT4(end.x, end.y, end.z, 1.0f);
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;
    m_color = color_palette.GetColor(DBG_LIGHT_BLUE);
}

vec4model::vec4model(vec4model&& other) noexcept
    : m_vertexBuffer(std::move(other.m_vertexBuffer)),
      m_indexBuffer(std::move(other.m_indexBuffer)),
      m_vertexCount(other.m_vertexCount),
      m_indexCount(other.m_indexCount),
      m_start(other.m_start),
      m_end(other.m_end),
      m_color(other.m_color)
{
    // Обнуляем ресурсы у исходного объекта
    other.m_vertexBuffer = nullptr;
    other.m_indexBuffer = nullptr;

    // Можно не обнулять XMFLOAT4, но для полноты:
    other.m_start = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    other.m_end = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    other.m_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
}

vec4model& vec4model::operator=(vec4model&& other) noexcept
{
    if (this != &other) // Проверка на самоприсваивание
    {
        // Освобождаем текущие ресурсы
        ReleaseCOM(m_vertexBuffer);
        ReleaseCOM(m_indexBuffer);

        // Перемещаем ресурсы
        m_vertexBuffer = other.m_vertexBuffer;
        m_indexBuffer = other.m_indexBuffer;
        m_start = other.m_start;
        m_end = other.m_end;
        m_color = other.m_color;

        // Обнуляем исходный объект
        other.m_vertexBuffer = nullptr;
        other.m_indexBuffer = nullptr;

        other.m_start = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        other.m_end = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
        other.m_color = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    return *this;
}

bool vec4model::Initialize(ID3D11Device *device, ID3D11DeviceContext *deviceContext)
{
    bool result;
    // Initialize the vertex and index buffers.
    result = InitializeBuffers(device);
    if (!result)
        return false;

    return true;
}
void vec4model::setStart(ID3D11DeviceContext *dc, const XMFLOAT3 &start)
{
    m_start = XMFLOAT4(start.x, start.y, start.z, 1.0f);
    UpdatePositions(dc);
}

void vec4model::setEnd(ID3D11DeviceContext *dc, const XMFLOAT3 &end)
{
    m_end = XMFLOAT4(end.x, end.y, end.z, 1.0f);
    UpdatePositions(dc);
}

void vec4model::setColor(ID3D11DeviceContext *dc, const XMFLOAT4 &color)
{
    m_color = color;
    UpdateColors(dc);
}

void vec4model::setColor(ID3D11DeviceContext *dc, float _x, float _y, float _z, float _w)
{
    m_color = XMFLOAT4(_x, _y, _z, _w);
    UpdateColors(dc);
}

void vec4model::setColor(ID3D11DeviceContext *dc, DEBUG_COLORS color)
{
    m_color = color_palette.GetColor(color);
    UpdateColors(dc);
}

void vec4model::setArrowProperties(float headLength, float headWidth)
{
    m_arrow_length = headLength;
    m_arrow_width = headWidth;
}

XMVECTOR vec4model::getDirectionNorm()
{
    XMVECTOR direction = getDirection();
    XMVECTOR normalizedDir = XMVector3Normalize(direction);

    return normalizedDir;
}

float vec4model::getLength()
{
    float result;

    XMVECTOR direction = getDirection();
    XMVECTOR lengthVec = XMVector3Length(direction);

    XMStoreFloat(&result, lengthVec);

    return result;
}

float vec4model::getLengthSq()
{
    float result;

    XMVECTOR direction = getDirection();
    XMVECTOR lengthVeSq = XMVector3LengthSq(direction);

    XMStoreFloat(&result, lengthVeSq);

    return result;
}

bool vec4model::UpdateBuffers(ID3D11DeviceContext *deviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    SimpleVertex *vertices = (SimpleVertex *)mappedResource.pData;
    m_FillVertices(vertices,true,true);

    deviceContext->Unmap(m_vertexBuffer, 0);
    return true;
}
bool vec4model::UpdatePositions(ID3D11DeviceContext *deviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    SimpleVertex *vertices = (SimpleVertex *)mappedResource.pData;
    m_FillVertices(vertices,true,false);

    deviceContext->Unmap(m_vertexBuffer, 0);
    return true;
}

bool vec4model::UpdateColors(ID3D11DeviceContext *deviceContext)
{
    D3D11_MAPPED_SUBRESOURCE mappedResource;

    HRESULT result = deviceContext->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    if (FAILED(result))
        return false;

    SimpleVertex *vertices = (SimpleVertex *)mappedResource.pData;
    m_FillVertices(vertices,false,true);

    deviceContext->Unmap(m_vertexBuffer, 0);
    return true;
}

bool vec4model::InitializeBuffers(ID3D11Device *device)
{
    SimpleVertex *vertices = new SimpleVertex[m_vertexCount];

    m_FillVertices(vertices,true,true);

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(SimpleVertex) * m_vertexCount;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = vertices;
    initData.SysMemPitch = 0;
    initData.SysMemSlicePitch = 0;

    // Now create the vertex buffer.
    HRESULT result = device->CreateBuffer(&vbd, &initData, &m_vertexBuffer);
    if (FAILED(result))
        return false;

    // Vector with arrow: line (0->1) и arrow (1->2->3)
    unsigned long *indices = new unsigned long[m_indexCount];
    indices[0] = 0;
    indices[1] = 1;
    indices[2] = 3;
    indices[3] = 2;
    indices[4] = 1;

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = sizeof(unsigned long) * m_indexCount;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;

    // Give the subresource structure a pointer to the index data.
    D3D11_SUBRESOURCE_DATA indexData;
    indexData.pSysMem = indices;
    indexData.SysMemPitch = 0;
    indexData.SysMemSlicePitch = 0;

    // Create the index buffer.
    result = device->CreateBuffer(&ibd, &indexData, &m_indexBuffer);
    if (FAILED(result))
        return false;

    delete[] vertices;
    vertices = nullptr;

    delete[] indices;
    indices = nullptr;

    return true;
}

void vec4model::m_FillVertices(SimpleVertex *vertices, bool upd_vertices, bool upd_color)
{
    XMVECTOR      dir = getDirectionNorm();
    XMVECTOR      end = getEnd();
    float         length = getLength();
    XMFLOAT3      right_pos;
    XMFLOAT3      left_pos;

    if (upd_vertices)
    {
        // fill line start and end
        vertices[0].Position = m_start;
        vertices[1].Position = m_end;

        // get arrow lines
        XMVECTOR right = XMVector3Cross(dir, XMVectorSet(0, 1, 0, 0));
        right = XMVector3Normalize(right) * (length * m_arrow_width);
        XMStoreFloat3(&right_pos, end - dir * m_arrow_length + right);
        XMStoreFloat3(&left_pos, end - dir * m_arrow_length - right);

        // fill arrow poins
        vertices[2].Position = XMFLOAT4(right_pos.x, right_pos.y, right_pos.z, 1.0f);
        vertices[3].Position = XMFLOAT4(left_pos.x, left_pos.y, left_pos.z, 1.0f);
    }

    if(upd_color)
    {
        for (int i = 0; i < m_vertexCount; i++)
            vertices[i].Color = m_color;
    }
}

void vec4model::ShutdownBuffers()
{
    if (m_indexBuffer)
        ReleaseCOM(m_indexBuffer);

    if (m_vertexBuffer)
        ReleaseCOM(m_vertexBuffer)
}

void vec4model::RenderBuffers(ID3D11DeviceContext *deviceContext)
{
    unsigned int stride = sizeof(SimpleVertex);
    unsigned int offset = 0;

    deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
    deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);
}
