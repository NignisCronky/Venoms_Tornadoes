#pragma once
#include "Globals.h"

class Triangle
{
	bool m_IsWireFrammed = false;

	ID3D11Buffer *m_Vertexs = nullptr;
	ID3D11Buffer *m_VertIndex = nullptr;

	ID3D11Buffer *m_ConstantBuffer = nullptr;
	ID3D11Buffer *m_Texture = nullptr;

	ID3D11RasterizerState *m_WireFrame = nullptr;
	ID3D11RasterizerState *m_SolidFill = nullptr;

	ID3D11InputLayout *m_InputLayout;

	ID3D11VertexShader *m_VertexShader = nullptr;
	ID3D11PixelShader *m_PixelShader = nullptr;
	ID3D11SamplerState* SamplerState;
	unsigned _Primative = 0;
	DirectX::XMFLOAT4X4 m_WorldMatrix;

	std::vector<DirectX::XMFLOAT4X4> m_Bones;

	ID3D11DeviceContext *m_Context;
	ID3D11Device *m_Device;
	ID3D11ShaderResourceView* texViews;
	std::vector<unsigned> m_VertIndexContainer;
	BoneContainer m_BonesContainer;
	std::vector<PNTIWVertex> VertexsContainer;
	unsigned Frame;
	MVPDCB ConstantBufferInfo;

	XMFLOAT4 verts[3];

public:
	void Update(Pro_View_World Matricies, ID3D11DeviceContext * Context);

	void Draw(ID3D11RenderTargetView * _BackBuffer, ID3D11DeviceContext * Context, Pro_View_World Matricies);

	Triangle(ID3D11DeviceContext *Context, ID3D11Device *Device);

	~Triangle();
	void Release();
	void Set(ID3D11DeviceContext * Context);
	inline void ToggleWireFram(ID3D11DeviceContext * devContext) {
		m_IsWireFrammed = !m_IsWireFrammed;
	};
};

