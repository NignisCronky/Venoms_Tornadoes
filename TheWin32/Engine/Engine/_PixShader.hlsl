// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 uv : UV;
	float4 norm : NORMAL;
	float4 vLightDir : LDIR;
	float4 vLightColor : LCOL;
}; 

texture2D texModel : register(t0);

SamplerState modelFilter : register(s0);
//RGBA
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 t = texModel.Sample(modelFilter, input.uv.xy);

	return t;
}
//add lighting to this shader
//reformate to take in new layout