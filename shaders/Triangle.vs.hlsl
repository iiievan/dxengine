struct VSOut
{
	float3 color : Color;
	float4 pos : SV_Position;
};

VSOut VSMain(float2 pos : Position, float3 color : Color)
{
	VSOut o;

	o.pos = float4(pos.x, pos.y, 0.0f, 1.0f);
	o.color = color;

    return o;
}