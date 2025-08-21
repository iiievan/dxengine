struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

cbuffer Cbuf
{
	row_major matrix transform;
};

VSOut VSMain(float2 pos : Position, float3 color : Color)
{
	VSOut o;

	o.pos = mul(float4(pos.x, pos.y, 0.0f, 1.0f), transform);
	o.color = color;

    return o;
}