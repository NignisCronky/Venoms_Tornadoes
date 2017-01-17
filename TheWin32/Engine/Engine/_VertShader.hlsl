// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
	float4 vLightDir;
	float4 vLightColor;
	matrix boneOffset[100];
};

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
	float2 uv : UV;
	float blendWeight0 : BLENDWEIGHT0;
	uint  boneIndice0 : BLENDINDICE0;
	float blendWeight1 : BLENDWEIGHT1;
	uint  boneIndice1 : BLENDINDICE1;
	float blendWeight2 : BLENDWEIGHT2;
	uint  boneIndice2 : BLENDINDICE2;
	float blendWeight3 : BLENDWEIGHT3;
	uint  boneIndice3 : BLENDINDICE3;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float4 uv : UV;
	float4 norm : NORMAL;
	float4 vLightDir : LDIR;
	float4 vLightColor : LCOL;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.pos.x, input.pos.y, input.pos.z, 1.0f);

	//Transform the vertex position into projected space.
	pos = mul(pos, model);
	pos = mul(pos, view);
	pos = mul(pos, projection);
	//output.pos = pos;

	//Animate based on bones
	output.pos  = mul(boneOffset[input.boneIndice0], pos) * input.blendWeight0;
	output.pos += mul(boneOffset[input.boneIndice1], pos) * input.blendWeight1;
	output.pos += mul(boneOffset[input.boneIndice2], pos) * input.blendWeight2;
	output.pos += mul(boneOffset[input.boneIndice3], pos) * input.blendWeight3;

	//Pass the color through without modification.
	output.uv = float4(input.uv.x, input.uv.y, 1.0f, 1.0f);
	output.norm = float4(input.norm.x, input.norm.y, input.norm.z, 1.0f);

	output.vLightDir = vLightDir;
	output.vLightColor = vLightColor;

	return output;
}
//reformate to take in new layout
//apply bone offset to vertexs