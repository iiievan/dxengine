cbuffer Cbuf
{
    matrix modelView;            // world matrix for model only for lightning calculation normals and etc.
    matrix modelViewProj;
};

struct VSOut
{
    float3 viewPos : Position;
    float3 normal : Normal;
    float2 uv : Texcoord;
    float4 pos : SV_Position;
};

VSOut VSMain(float3 pos : Position, float3 n : Normal, float2 uv : Texcoord)
{
    VSOut vso;
    vso.viewPos = (float3)mul(float4(pos, 1.0f), modelView);  // transform model only in world space from local for normals for light calculation
    vso.normal = mul(n,(float3x3)modelView);
    vso.pos = mul(float4(pos, 1.0f), modelViewProj);
    vso.uv = uv;
    return vso;
}