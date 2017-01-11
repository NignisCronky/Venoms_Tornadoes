// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float4 vLightDir[4];
	float4 vLightColor[4];
};

texture2D texModel : register(t0);

SamplerState modelFilter : register(s0);
//RGBA
float4 main(float4 pos : SV_POSITION, float4 uv : UV, float4 norm : NORMAL) : SV_TARGET
{
	float4 t = texModel.Sample(modelFilter, (float2)uv);
	if (t.a < .5f)
		discard; 

	//Ambient Light
	float4 ambientLight = vLightColor[0] * t;

	//Directional Light
	float4 lightDir = mul(mul(mul(vLightDir[0], model), view), projection);
	float lightRatio = saturate(dot(-1 * lightDir, norm));
	float4 directionalLight = vLightColor[1] * lightRatio * t;

	//Point Light
	float4 lightPos = mul(mul(mul(vLightDir[1], model), view), projection);
	lightDir = normalize(lightPos - pos);
	lightRatio = saturate(dot(-1 * lightDir, norm));
	float4 pointLight = vLightColor[2] * lightRatio * t;

	//Spot Light
	float4 slightPos = mul(mul(mul(vLightDir[2], model), view), projection);
	float4 coneDir = mul(mul(mul(vLightDir[3], model), view), projection);
	float coneRatio = .1f;
	lightDir = normalize(slightPos - pos);
	float surfaceRatio = saturate(dot(-1 * lightDir, coneDir));
	float spotFactor = (surfaceRatio > coneRatio) ? 1.0f : 0.0f;
	lightRatio = saturate(dot(lightDir, norm));
	float4 spotLight = spotFactor * lightRatio * vLightColor[3] * t;

	//Total Light
	float4 lightTotal = ambientLight + directionalLight + pointLight + spotLight;
	t = lightTotal;
	t.w = 1.0f;

	return t;
}
