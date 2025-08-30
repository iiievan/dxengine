cbuffer CBuf
{
  float4 face_colors[8];
};

float4 PSMain(uint tid : SV_PrimitiveID) : SV_Target
{
      return face_colors[(tid/2) % 8]; // cuz one face - 2 triangles
}