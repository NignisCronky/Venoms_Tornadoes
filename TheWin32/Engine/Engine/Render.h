#pragma once
#include "Globals.h"
class Render
{
	bool m_IsWireFrammed = false;

	ID3D11Buffer *m_Vertexs = nullptr;
	ID3D11Buffer *m_VertIndex = nullptr;

	ID3D11Buffer *m_ConstantBuffer = nullptr;
	ID3D11Buffer *m_Texture = nullptr;

	ID3D11RasterizerState *m_WireFrame = nullptr;
	ID3D11RasterizerState *m_SolidFill = nullptr;

	ID3D11InputLayout *m_InputLayout = nullptr;

	ID3D11VertexShader *m_VertexShader = nullptr;
	ID3D11PixelShader *m_PixelShader = nullptr;

	unsigned _Primative = 0;
	DirectX::XMFLOAT4X4 m_WorldMatrix;

	std::vector<DirectX::XMFLOAT4X4> m_Bones;
	ID3D11Texture2D *m_Texture;

	ID3D11DeviceContext *m_Context;
	ID3D11Device *m_Device;

	std::vector<unsigned> m_VertIndexContainer;
	BoneContainer m_BonesContainer;
	std::vector<PNTIWVertex> VertexsContainer;
	unsigned Frame;
	MVPDCB ConstantBufferInfo;

public:
	void Update(Pro_View_World Matricies, ID3D11DeviceContext * Context);
	
	void Draw(ID3D11RenderTargetView * _BackBuffer, ID3D11DeviceContext * Context, Pro_View_World Matricies);
	
	Render();

	Render(ID3D11Texture2D *texture, Pro_View_World Matricies,std::vector<unsigned> VertIndex, std::vector<Joint> Bones, std::vector<PNTIWVertex> Vertexs, ID3D11DeviceContext *Context,ID3D11Device *Device);
	
	~Render();

	std::vector<XMFLOAT4X4> CalcBoneOffSets(std::vector<Joint> Bones)
	{
		std::vector<XMFLOAT4X4> MAtrices;

		// for each bone, create a matrix and push it onto the vector
		for (unsigned i = 0; i < (unsigned)Bones.size(); i++)
		{
			XMMATRIX Temp;
			// set to idenity
			Temp = (DirectX::XMMatrixMultiply(DirectX::XMMatrixIdentity(), DirectX::XMMatrixTranslation(Bones[i].translation.x, Bones[i].translation.y, Bones[i].translation.z)));
			Temp = DirectX::XMMatrixMultiply(DirectX::XMMatrixRotationQuaternion(),)
			// translate it
			// rotate it

		}

		//return the vector
		return MAtrices;
	}



	void Release();
	void Create();
	void Set(ID3D11DeviceContext * Context);
	inline void ToggleWireFram(ID3D11DeviceContext * devContext) {
		m_IsWireFrammed = !m_IsWireFrammed;
	};
};

