#pragma once
#include "Globals.h"

class RenderNode
{
public:
	//members
	bool IsWireFrammed = false;
	ID3D11Buffer *_VertexBuffer = nullptr;			// the pointer to the vertex buffer
	ID3D11Buffer *_BoneBuffer = nullptr;			// the pointer to the constant buffer for bones
	ID3D11Buffer *_MatrixBuffer = nullptr;			// the pointer to the constant buffer for view, World matrix, Projection 
	ID3D11Buffer *_TextureBuffer = nullptr;			// the pointer to the constant buffer a texture 

	ID3D11RasterizerState *_WireFrame = nullptr;	// the pointer to the raster state for wireframe
	ID3D11RasterizerState *_SolidFill = nullptr;	// the pointer to the raster state for filled

	ID3D11InputLayout *_InputLayout = nullptr;		// the pointer to the input layout

	ID3D11VertexShader *_VertexShader = nullptr;	// the pointer to the Vertex Shader
	ID3D11PixelShader *_PixelShader = nullptr;		// the pointer to the Pixel Shader
	ID3D11PixelShader *_LightShader = nullptr;		// the pointer to the Pixel Shader with lighting

	unsigned _Primative = 0;						// the primative type
	DirectX::XMFLOAT4X4 _WorldMatrix;				// the world matrix of the Rendered Object

	std::vector<DirectX::XMFLOAT4X4> _Bones;
	ID3D11Texture2D *_Texture;
	void render(ID3D11DeviceContext *_DeviceContext, DirectX::XMFLOAT4X4 ProView[2], ID3D11RenderTargetView *_BackBuffer, unsigned NumOfVerts);


	RenderNode(std::vector<SmartVert> Info, ID3D11DeviceContext *devContext,
		ID3D11Device *Device, Pro_View_World Matricies,
		std::vector<DirectX::XMFLOAT4X4> Bones, ID3D11Texture2D *texture);


	inline void ToggleWireFram(ID3D11DeviceContext * devContext) {
		IsWireFrammed = !IsWireFrammed;
	};




	RenderNode();
	~RenderNode();
};

