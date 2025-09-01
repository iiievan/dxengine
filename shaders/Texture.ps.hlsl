Texture2D tex;        // Данные текстуры
SamplerState smpl;    // Правила доступа

float4 PSMain(float2 tc : TexCoord) : SV_Target
{
    return tex.Sample(smpl, tc);    // Используем ОБА компонента
}