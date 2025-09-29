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

Texture2D tex;
Texture2D spec;
SamplerState smplr;

float4 PSMain(float3 worldPos : Position, float3 n : Normal, float2 uv : Texcoord) : SV_Target
{
    // fragment to light vector data
    const float3 vToL = lightPos - worldPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    // attenuation
    const float att = 1.0f/ (attConst + attLin * distToL + attQuad * (distToL * distToL));

    // diffuse intencity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));
    
	// reflected light vector
	const float3 w = n * dot(vToL, n);	// get projection of normalized light dir on normal
    const float3 r = w * 2.0f - vToL;	// substracion from normalized light dir and his projection get reflected light vector

    const float4 specularSample = spec.Sample(smplr, uv);
    const float3 specularReflectionColor = specularSample.rgb;
    const float specularPower = pow(2.0f,specularSample.a * 13.0f);
    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	// dot(normalize(r), normalize(worldPos)) - give us cos of angle between reflected vector and vector to camera
	const float3 specular = att * (diffuseColor * diffuseIntensity) * pow(max(0.0f, dot(normalize(-r), normalize(worldPos))), specularPower);

    // final color
    return float4(saturate((diffuse + ambient) * tex.Sample(smplr,uv).rgb + specular * specularReflectionColor), 1.0f);
}