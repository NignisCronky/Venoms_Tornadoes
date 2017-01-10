#pragma pack_matrix( row_major )
struct INPUT_VERTEX
{
	float3 coordinate : POSITION;
	float4 color: COLOR;
};

struct OUTPUT_VERTEX
{
	float4 coordiante : SV_POSITION;
	float4 color : COLOR;
};

cbuffer matrices : register(b0)
{
	float4x4 world;
	float4x4 projection;
	float4x4 view;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	float4 pos = float4(fromVertexBuffer.coordinate, 1);
	pos = mul(world,pos);
	pos = mul(view, pos);
	pos = mul(projection, pos);
	sendToRasterizer.coordiante = pos;
	sendToRasterizer.color = fromVertexBuffer.color;
	return sendToRasterizer;
}