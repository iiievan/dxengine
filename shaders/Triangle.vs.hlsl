cbuffer Cbuf
{
	matrix transform;
};

float4 VSMain(float3 pos : Position) : SV_Position
{
    return mul(float4(pos, 1.0f), transform);
}