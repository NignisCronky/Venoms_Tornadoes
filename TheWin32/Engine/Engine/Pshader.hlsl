struct INPUT_VERTEX
{
	float2 coordinate : POSITION;
	float4 color: COLOR;
};

struct OUTPUT_VERTEX
{
	
};

cbuffer THIS_IS_VRAM : register(b0)
{
	float4 constantColor;
	float2 constantOffset;
	float2 padding;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	


	return sendToRasterizer;
}