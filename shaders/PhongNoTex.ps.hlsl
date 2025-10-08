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

cbuffer ObjectCBuf
{
    float3 materialColor;
     float specularIntencity;
     float specularPower;
	 float padding[2];
};

float4 PSMain(float3 viewPos : Position, float3 n : Normal) : SV_Target
{
    // fragment to light vector data
    const float3 vToL = lightPos - viewPos;
    const float distToL = length(vToL);
    const float3 dirToL = vToL / distToL;

    // attenuation
    const float att = 1.0f/ (attConst + attLin * distToL + attQuad * (distToL * distToL));

    // diffuse intencity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, n));

    // reflected light vector
	const float3 w = n * dot(vToL, n);	// get projection of normalized light dir on normal
    const float3 r = w * 2.0f - vToL;	// substracion from normalized light dir and his projection get reflected light vector

    // calculate specular intensity based on angle between viewing vector and reflection vector, narrow with power function
	// dot(normalize(r), normalize(viewPos)) - give us cos of angle between reflected vector and vector to camera
	const float3 specular = att * (diffuseColor * diffuseIntensity ) * specularIntencity * pow(max(0.0f, dot(normalize(-r), normalize(viewPos))), specularPower);

    // final color
    return float4(saturate((diffuse + ambient) * materialColor + specular), 1.0f);
}