cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float4 vLightDir;
	float4 vLightColor;
};

texture2D texModel : register(t0);

SamplerState modelFilter : register(s0);
//RGBA
float4 main(float4 pos : SV_POSITION, float4 uv : UV, float4 norm : NORMAL) : SV_TARGET
{
	float4 t = texModel.Sample(modelFilter, (float2)uv);

	//Directional Light
	float4 lightDir = mul(mul(mul(vLightDir, model), view), projection);
	float lightRatio = saturate(dot(-1 * lightDir, norm));
	float4 directionalLight = vLightColor[1] * lightRatio * t;

	return directionalLight;
}
//add lighting to this shader
//reformate to take in new layout
