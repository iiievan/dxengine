#ifndef __VEC4MODELSHADERCLASS_HPP
#define __VEC4MODELSHADERCLASS_HPP

#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#pragma comment(lib, "d3dcompiler.lib")

using namespace DirectX;

class vec4modelShaderClass
{
    struct MatrixBufferType
    {
        XMMATRIX world;
        XMMATRIX view;
        XMMATRIX projection;
    };

public:
    vec4modelShaderClass();

    vec4modelShaderClass(const vec4modelShaderClass &);

    ~vec4modelShaderClass();

    bool Initialize(ID3D11Device *, HWND);

    void Shutdown();

    bool Render(ID3D11DeviceContext *, int, XMMATRIX, XMMATRIX, XMMATRIX);

private:
    bool InitializeShader(ID3D11Device *, HWND, WCHAR *, WCHAR *);

    void ShutdownShader();

    void OutputShaderErrorMessage(ID3DBlob *, HWND, WCHAR *);

    bool SetShaderParameters(ID3D11DeviceContext *, XMMATRIX, XMMATRIX, XMMATRIX);

    void RenderShader(ID3D11DeviceContext *, int );

private:
    ID3D11VertexShader *m_vertexShader;
    ID3D11PixelShader  *m_pixelShader;
    ID3D11InputLayout  *m_layout;
    ID3D11Buffer       *m_matrixBuffer;
};

#endif //__VEC4MODELSHADERCLASS_HPP
