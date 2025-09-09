#pragma enable_d3d11_debug_symbols
#pragma debug

cbuffer CBuf
{
    float4 color;
};

float4 PSMain() : SV_Target
{
    return color;
}