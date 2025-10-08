cbuffer LightCBuf
{
    float3 lightPos;
    float3 ambient;
    float3 diffuseColor;
    float  diffuseIntensity;
    float  attConst;
    float  attLin;
    float  attQuad;
};

cbuffer ObjectCbuf
{
    bool normalMapEnabled;  // bool takes up 4 bytes in hlsl
    float padding[3];
};

Texture2D tex;
Texture2D spec;
Texture2D nmap;
SamplerState smplr;

float4 PSMain(float3 viewPos : Position, float3 n : Normal, float3 tan : Tangent, float3 bitan : Bitangent, float2 uv : Texcoord) : SV_Target
{
    // sample normal from map if normal mapping enabled
    if(normalMapEnabled)
    {
        // build the transform(ritation) into tangent space
        const float3x3 tanToView = float3x3(normalize(tan),
                                            normalize(bitan),
                                            normalize(n));
        // unpack normal data 
        const float3 normalSample = nmap.Sample(smplr, uv).xyz;
        n = normalSample * 2.0f - 1.0f;

        // bring normal from tangspace into view space
        n = mul(n, tanToView);
    }
    // fragment to light vector data
    const float3 vToL = lightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    // attenuation
    const float att = 1.0f/ (attConst + attLin * distToL + attQuad * (distToL * distToL));

    // diffuse intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
	// reflected light vector
	const float3 w = n * dot(vToL, n);	// get projection of normalized light dir on normal
    const float3 r = w * 2.0f - vToL;	// substracion from normalized light dir and his projection get reflected light vector

    const float4 specularSample = spec.Sample(smplr, uv);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f,specularSample.a * 13.0f);
    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	// dot(normalize(r), normalize(viewPos)) - give us cos of angle between reflected vector and vector to camera
	const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);

    // final color
    return float4(saturate((diffuse + ambient) * tex.Sample(smplr,uv).rgb + specular * specularReflectionColor), 1.0f);
}