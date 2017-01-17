#pragma once
#include "Globals.h"
class BoneSphere
{
	ID3D11Buffer* SphereVertexs;
	// vertex data
	ID3D11InputLayout* InputLayout;
	//input layout
	ID3D11RasterizerState* RasterState;
	//raster state
	ID3D11Buffer* pro_view_world;
	//constant buffer for view and projection
	ID3D11VertexShader* VertShad;
	//vert shader
	ID3D11PixelShader* PixShad;
	//pixel shader

	Pro_View_World PVW;
	struct VertColor
	{
		XMFLOAT3 Vector;
		XMFLOAT4 Color;
	};
	std::vector<VertColor> ConstantVerts;
	std::vector<VertColor> DyanmicVerts;
public:
	void Create(Pro_View_World Mat, ID3D11DeviceContext *Context, ID3D11Device *Device, std::vector<SmartVert> verts);
	void set(ID3D11DeviceContext *Context);
	void Update(float x, float y, float z);



	BoneSphere();
	~BoneSphere();
};

