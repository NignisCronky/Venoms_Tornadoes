#pragma once
#include "Globals.h"
#include "../FBX Exporter/FBX Exporter.h"

using namespace DirectX;

struct FBXRenderer
{
	FBXRenderer(ID3D11Device &dev, XMFLOAT4X4 &camera, ID3D11DeviceContext &DevCon);
	FBXRenderer();
	void Create(ID3D11Device &dev, XMFLOAT4X4 &camera, ID3D11DeviceContext &Con);
	void CreateDeviceDependentResources(void);
	void CreateWindowSizeDependentResources(void);
	void LoadFBXFromFile(const char *fbx, const char *bin, const wchar_t *texturePath);
	void ReleaseDeviceDependentResources(void);
	void Update(long long frame);
	void Render();
	void DrawBones(std::vector<MyMesh> Sphere_, float offset_[3]);
	void DrawSpheresForbones(std::vector<Bone> vec, std::vector<MyMesh> Sphere_);


	std::vector<MyMesh>Sphere;
	std::vector<Bone>TempBones;
private:
	ID3D11RasterizerState *RasterState;				// the pointer to the raster state
	ID3D11Device				*m_dev;
	ID3D11DeviceContext			*m_devCon;
	ID3D11InputLayout			*m_inputLayout;
	ID3D11Buffer				*m_vertexBuffer;
	ID3D11Buffer				*m_indexBuffer;
	ID3D11VertexShader			*m_vertexShader;
	ID3D11VertexShader			*ShereVeetShader;
	ID3D11PixelShader			*SherepixeShader;
	ID3D11PixelShader			*m_pixelShader;
	ID3D11Buffer				*m_constantBuffer;
	ID3D11Buffer				*Sphere_constantBuffer;
	//m_constantBufferData
	Pro_View_World Sphere_constantBufferData;
	ID3D11Texture2D				*m_texture2D;
	ID3D11ShaderResourceView	*m_texView;
	ID3D11BlendState			*m_blendState;

	MVPDCB	m_constantBufferData;
	XMFLOAT4X4* m_camera;

	std::vector<PNTIWVertex> verts;
	std::vector<unsigned int> indexes;
	Skeleton skelly;
};