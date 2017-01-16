struct INPUT_VERTEX
{
	float4 coordinate : SV_POSITION;
	float4 color: COLOR;
};

float4 main(INPUT_VERTEX fromVertexBuffer) : SV_TARGET
{
	return fromVertexBuffer.color;
}
//todo: reformate to take in new layout
