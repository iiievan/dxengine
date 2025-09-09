#pragma enable_d3d11_debug_symbols
#pragma debug

cbuffer Cbuf
{
    matrix transform;
};

struct VSOut
{
    float2 tex : TexCoord;
    float4 pos : SV_Position;
};

VSOut VSMain(float3 pos : Position, float2 tex : TexCoord)
{
    VSOut vso;
    vso.pos = mul(float4(pos, 1.0f), transform);
    vso.tex = tex;
    return vso;
}
