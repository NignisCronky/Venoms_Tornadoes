#pragma once
#include"Globals.h"
class RenderNode
{
public:
	//members
	char* name;
	const char* filename;
	bool WireFrammed = false;
	ID3D11Device *Dev;
	ID3D11Buffer *_IndexBuffer = nullptr;			// the pointer to the index buffer
	ID3D11Buffer *_Buffer = nullptr;				// the pointer to the vertex buffer
	ID3D11Buffer *_ConstantBuffer = nullptr;		// the pointer to the constant buffer
	ID3D11RasterizerState *RasterState = nullptr;	// the pointer to the raster state
	ID3D11InputLayout *_Layout = nullptr;			// the pointer to the input layout
	ID3D11VertexShader *_VertexShader = nullptr;	// the pointer to the Vertex Shader
	ID3D11PixelShader *_PixelShader = nullptr;		// the pointer to the Pixel Shader
	unsigned Geometry = 0;							// the primative type
	DirectX::XMFLOAT4X4 World;						// the world matrix of the Rendered Object
	float Color[4] = { 1.0f,1.0f,1.0f,1.0f };		// the color for the wire frame
	DirectX::XMFLOAT4X4* m_camera;					// pointer to the camera
													//functions
	void SetLayout(unsigned index);
	void render();
	void renderWireFrame();
	//  no texture
	RenderNode(std::vector<float[3]> Points, std::vector<float[4]> Color, ID3D11Device *dev);
	//  no texture or color
	RenderNode(std::vector<float[3]> Points, float Color[4], ID3D11Device *dev);
	// texture
	RenderNode(std::vector<float[3]> Points, std::vector<float[3]> Norms, std::vector<float[2]> UVs, ID3D11Device* dev);



	RenderNode();
	~RenderNode();
};

