#pragma once
#include "Globals.h"
#include "../FBX Exporter/FBX Exporter.h"

using namespace DirectX;
struct FBXRenderer
{
	FBXRenderer(ID3D11Device &dev, XMFLOAT4X4 &camera);
	FBXRenderer(ID3D11Device &dev, XMFLOAT4X4 &camera, ID3D11DeviceContext &DevCon);
	FBXRenderer();
	void Create(ID3D11Device &dev, XMFLOAT4X4 &camera, ID3D11DeviceContext &Con);
	void CreateDeviceDependentResources(void);
	void CreateWindowSizeDependentResources(void);
	void LoadFBXFromFile(const char *fbx, const char *bin, const wchar_t *texturePath);
	void ReleaseDeviceDependentResources(void);
	void Update(long long frame);
	void Render();
private:
	ID3D11Device				*m_dev;
	ID3D11DeviceContext			*m_devCon;
	ID3D11InputLayout			*m_inputLayout;
	ID3D11Buffer				*m_vertexBuffer;
	ID3D11Buffer				*m_indexBuffer;
	ID3D11VertexShader			*m_vertexShader;
	ID3D11PixelShader			*m_pixelShader;
	ID3D11Buffer				*m_constantBuffer;
	ID3D11Texture2D				*m_texture2D;
	ID3D11ShaderResourceView	*m_texView;
	ID3D11BlendState			*m_blendState;

	MVPDCB	m_constantBufferData;
	XMFLOAT4X4* m_camera;

	std::vector<PNTIWVertex> verts;
	std::vector<unsigned int> indexes;
	Skeleton skelly;
};