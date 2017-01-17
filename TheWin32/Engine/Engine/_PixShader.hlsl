texture2D texModel : register(t0);

SamplerState modelFilter : register(s0);
//RGBA
float4 main(float4 pos : SV_POSITION, float4 uv : UV, float4 norm : NORMAL, float4 vLightDir : LDIR, float4 vLightColor : LCOL) : SV_TARGET
{
	float4 t = texModel.Sample(modelFilter, (float2)uv);

	return t;
}
//add lighting to this shader
//reformate to take in new layout