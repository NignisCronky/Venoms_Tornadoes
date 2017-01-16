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

	return t;
}
//add lighting to this shader
//reformate to take in new layout