cbuffer CBuf
{
    float4 color;
};

float4 PSMain() : SV_Target
{
    return color;
}