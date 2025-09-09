#pragma enable_d3d11_debug_symbols
#pragma debug

cbuffer CBuf
{
    matrix model;
    matrix modelViewProj;
};

float4 VSMain(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), modelViewProj);
}