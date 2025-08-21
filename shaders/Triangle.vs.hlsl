struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

cbuffer Cbuf
{
	matrix transform;
};

VSOut VSMain(float3 pos : Position, float3 color : Color)
{
	VSOut o;

	o.pos = mul(float4(pos, 1.0f), transform);
	o.color = color;

    return o;
}