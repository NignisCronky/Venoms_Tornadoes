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
	float4 blendWeight : BLENDWEIGHT;
	uint4  boneIndice : BLENDINDICE;
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

	//Animate based on bones
	//output.pos = mul(boneOffset[input.boneIndice.x], pos) * input.blendWeight.x;
	//output.pos += mul(boneOffset[input.boneIndice.y], pos) * input.blendWeight.y;
	//output.pos += mul(boneOffset[input.boneIndice.z], pos) * input.blendWeight.z;
	//output.pos += mul(boneOffset[input.boneIndice.w], pos) * input.blendWeight.w;

	//Transform the vertex position into projected space.
	//output.pos = mul(output.pos, model);
	output.pos = mul(pos, model);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	//Pass the color through without modification.
	output.uv = float4(input.uv.x, input.uv.y, 1.0f, 1.0f);
	output.norm = float4(input.norm.x, input.norm.y, input.norm.z, 1.0f);

	output.vLightDir = vLightDir;
	output.vLightColor = vLightColor;

	return output;
}
//reformate to take in new layout
//apply bone offset to vertexs