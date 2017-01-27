#include "FBXRenderer.h"
#include "../FBX Exporter/FBX Exporter.h"
FBXRenderer::FBXRenderer(ID3D11Device &dev, XMFLOAT4X4 &camera,ID3D11DeviceContext &DevCon)
{
	m_dev = &dev;
	m_camera = &camera;
	m_devCon = &DevCon;
}

FBXRenderer::FBXRenderer()
{
}

void FBXRenderer::Create(ID3D11Device &dev, XMFLOAT4X4 &camera, ID3D11DeviceContext &Con)
{
	m_dev = &dev;
	m_camera = &camera;
	m_devCon = &Con;
}

XMFLOAT4X4 PerspectiveProjection()
{
	float xScale, yScale;
	yScale = float(1.0 / tan((.5 * float(verticalFOV)) * PI / 180.0));
	xScale = yScale * aspectratio;
	XMFLOAT4X4 mat(xScale, 0, 0, 0,
		0, yScale, 0, 0,
		0, 0, zFar / (zFar - zNear), 1,
		0, 0, -(zFar * zNear) / (zFar - zNear), 0);
	return mat;
}

// Initializes view parameters when the window size changes.
void FBXRenderer::CreateWindowSizeDependentResources(void)
{
	//float aspectRatio = width / height;
	//float fovAngleY = 70.0f * XM_PI / 180.0f;

	//// This is a simple example of change that can be made when the app is in
	//// portrait or snapped view.
	//if (aspectRatio < 1.0f)
	//{
	//	fovAngleY *= 2.0f;
	//}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	//XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100.0f);

	XMStoreFloat4x4(&m_constantBufferData.Pro, XMMatrixTranspose(XMMatrixPerspectiveFovLH(verticalFOV, aspectratio, zNear, zFar)));

	XMStoreFloat4x4(&m_constantBufferData.View, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(m_camera))));
}
const char* spherename = "../Original Assets/spherical.fbx";

//std::vector<MyMesh> sphere;
//LoadScene(spherename, sphere);
void FBXRenderer::DrawBones(std::vector<MyMesh> Sphere_, float offset_[3])
{
	std::vector<VERTEX> vertextlist;

	for (unsigned i = 0; i < Sphere_.size(); i++)
	{
		VERTEX Temp;
		Temp.Color = { 0.5f,0.5f,0.0f,1.0f };
		Temp.X = Sphere_[i].position[0] + offset_[0];
		Temp.Y = Sphere_[i].position[1] + offset_[1];
		Temp.Z = Sphere_[i].position[2] + offset_[2];
		vertextlist.push_back(Temp);
	}
	//set input layout, set pixel shadder












	ID3D11Buffer *AniBuffer_;
	D3D11_BUFFER_DESC AniBuffDesc;
	ZeroMemory(&AniBuffDesc, sizeof(AniBuffDesc));
	AniBuffDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	AniBuffDesc.ByteWidth = sizeof(VERTEX) * (unsigned)vertextlist.size(); // size is the VERTEX struct
	AniBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	AniBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &vertextlist[0];
HRESULT debug=	m_dev->CreateBuffer(&AniBuffDesc, &data, &AniBuffer_);       // create the buffer

	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	this->m_devCon->IASetVertexBuffers(0, 1, &AniBuffer_, &stride, &offset);


	ID3D11RasterizerState *AniRaster;
	D3D11_RASTERIZER_DESC AniRasDesc;
	ZeroMemory(&AniRasDesc, sizeof(AniRasDesc));
	AniRasDesc.CullMode = D3D11_CULL_NONE;
	AniRasDesc.FillMode = D3D11_FILL_WIREFRAME;
	m_dev->CreateRasterizerState(&AniRasDesc, &AniRaster);
	m_devCon->RSSetState(AniRaster);
	m_devCon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_devCon->Draw((unsigned)vertextlist.size(), 0);

	AniBuffer_->Release();
	AniRaster->Release();
	HRESULT GetDeviceRemovedReason();

	D3D11_RASTERIZER_DESC RasDesc;
	ZeroMemory(&RasDesc, sizeof(RasDesc));
	RasDesc.CullMode = D3D11_CULL_NONE;
	RasDesc.FillMode = D3D11_FILL_SOLID;
	m_dev->CreateRasterizerState(&RasDesc, &RasterState);

	m_devCon->RSSetState(RasterState);
	stride = sizeof(VERTEX);
	m_devCon->IASetVertexBuffers(0, 1, &this->m_vertexBuffer, &stride, &offset);
	m_devCon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
void FBXRenderer::DrawSpheresForbones(std::vector<Bone> vec, std::vector<MyMesh> Sphere_)
{

	D3D11_INPUT_ELEMENT_DESC INPUT_DESC[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	ID3D11InputLayout* _Layout;
	//Device->CreateInputLayout(INPUT_DESC, 2, _VertexShader, sizeof(_VertexShader), &_Layout);
	m_dev->CreateInputLayout(INPUT_DESC, 2, Vshader, sizeof(Vshader), &_Layout);
	m_devCon->IASetInputLayout(_Layout);

	//MVPDCB	m_constantBufferData;
	//XMFLOAT4X4* m_camera;
	
	//set constant buffer
	//set shadders
	m_devCon->VSSetShader(this->ShereVeetShader, nullptr, 0);
	m_devCon->PSSetShader(this->SherepixeShader, nullptr, 0);
	m_devCon->VSSetConstantBuffers(0, 1, &Sphere_constantBuffer);

	for (unsigned i = 0; i < vec.size(); i++)
	{
		DrawBones(Sphere_, vec[i].pos);
	}

	_Layout->Release();


}



void FBXRenderer::Update(long long frame)
{
	// for each bone, create a matrix and push it onto the vector
	for (unsigned i = 0; i < (unsigned)skelly.mJoints.size(); i++)
	{
		if (skelly.mJoints[i].mAnimation.size() == 0)
		{
			XMFLOAT4X4 T;
			XMStoreFloat4x4(&T, DirectX::XMMatrixIdentity());
			m_constantBufferData.Boneoffsets[i] = T;
		}
		else
		{
			XMMATRIX M = XMLoadFloat4x4(&skelly.mJoints[i].globalBindposeInverseMatrix);
			XMMATRIX C = XMLoadFloat4x4(&skelly.mJoints[i].mAnimation[frame % skelly.mJoints[i].mAnimation.size()].mGlobalTransform);
			XMMATRIX Temp = XMMatrixMultiply(M, C);
			XMFLOAT4X4 T;
			XMStoreFloat4x4(&T, XMMatrixTranspose(Temp));
			m_constantBufferData.Boneoffsets[i] = T;
		}
	}
	XMStoreFloat4x4(&m_constantBufferData.World, XMMatrixIdentity());
	XMStoreFloat4x4(&m_constantBufferData.View, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(m_camera))));
	m_constantBufferData.LightDirection = XMFLOAT4(1.0f * (frame % skelly.mJoints[0].mAnimation.size()), -1.0f, 1.0f * (frame % skelly.mJoints[0].mAnimation.size()), 1.0f);
	m_constantBufferData.LightColor = XMFLOAT4(0.0f, 1.5f * frame, 1.5f * frame, 1.0f);
	//m_constantBufferData.LightDirection = XMFLOAT4(1.0f * (frame % skelly.mJoints[0].mAnimation.size()), -1.0f, 1.0f * (frame % skelly.mJoints[0].mAnimation.size()), 1.0f);
	//m_constantBufferData.LightColor = XMFLOAT4(0.0f, 1.5f * frame, 1.5f * frame, 1.0f);
	//
	
	


}

void FBXRenderer::LoadFBXFromFile(const char *fbx, const char *bin, const wchar_t *texturePath)
{
	FBXtoBinary(fbx, bin, false);
	ReadBinary(bin, &skelly, &indexes, &verts);
	if (texturePath != NULL)
	{
		CreateDDSTextureFromFile(
			m_dev,
			texturePath,
			(ID3D11Resource**)&m_texture2D,
			&m_texView);
	}
	CreateDeviceDependentResources();
	
	LoadScene(spherename, this->Sphere);
}

void FBXRenderer::CreateDeviceDependentResources(void)
{
	m_dev->CreateVertexShader(&_VertShader, sizeof(_VertShader), nullptr, &m_vertexShader);
	m_dev->CreateVertexShader(&Vshader, sizeof(Vshader), nullptr, &ShereVeetShader);

	//ShereVeetShader
	static const D3D11_INPUT_ELEMENT_DESC vertDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UV", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDINDICE", 0, DXGI_FORMAT_R32G32B32_UINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_dev->CreateInputLayout(vertDesc, ARRAYSIZE(vertDesc), _VertShader, sizeof(_VertShader), &m_inputLayout);

	m_dev->CreatePixelShader(&_PixShader, sizeof(_PixShader), nullptr, &m_pixelShader);
	m_dev->CreatePixelShader(&Pshader, sizeof(Pshader), nullptr, &SherepixeShader);



	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(MVPDCB), D3D11_BIND_CONSTANT_BUFFER);
	m_dev->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer);

	CD3D11_BUFFER_DESC constantBuffSphereerDesc(sizeof(Pro_View_World), D3D11_BIND_CONSTANT_BUFFER);
	m_dev->CreateBuffer(&constantBuffSphereerDesc, nullptr, &Sphere_constantBuffer);




	D3D11_SUBRESOURCE_DATA m_vertexBufferData = { 0 };
	m_vertexBufferData.pSysMem = verts.data();
	m_vertexBufferData.SysMemPitch = 0;
	m_vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC m_vertexBufferDesc((unsigned)verts.size() * sizeof(PNTIWVertex), D3D11_BIND_VERTEX_BUFFER);
	m_dev->CreateBuffer(&m_vertexBufferDesc, &m_vertexBufferData, &m_vertexBuffer);

	/*D3D11_SUBRESOURCE_DATA m_indexBufferData = { 0 };
	m_indexBufferData.pSysMem = sides.data();
	m_indexBufferData.SysMemPitch = 0;
	m_indexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC m_indexBufferDesc(sides.size() * sizeof(unsigned short), D3D11_BIND_INDEX_BUFFER);
	m_deviceResources->GetD3DDevice()->CreateBuffer(&m_indexBufferDesc, &m_indexBufferData, &m_indexBuffer);*/

	CreateWindowSizeDependentResources();
}

void FBXRenderer::Render()
{
	XMStoreFloat4x4(&m_constantBufferData.View, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(m_camera))));

	Sphere_constantBufferData.Pro = m_constantBufferData.Pro;
	Sphere_constantBufferData.World = m_constantBufferData.World;
	Sphere_constantBufferData.View = m_constantBufferData.View;
	//Sphere_constantBufferData


	// Prepare the constant buffer to send it to the graphics device.
	m_devCon->UpdateSubresource(m_constantBuffer, 0, NULL, &m_constantBufferData, 0, 0);
	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(PNTIWVertex);
	UINT offset = 0;

	ID3D11ShaderResourceView* texViews[] = { m_texView };
	m_devCon->PSSetShaderResources(0, 1, texViews);

	m_devCon->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	// Each index is one 16-bit unsigned integer (short).
	//m_devCon->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R16_UINT, 0);
	m_devCon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	m_devCon->IASetInputLayout(m_inputLayout);
	// Attach our vertex shader.
	m_devCon->VSSetShader(m_vertexShader, nullptr, 0);
	// Send the constant buffer to the graphics device.
	m_devCon->VSSetConstantBuffers(0, 1, &m_constantBuffer);
	//m_devCon->PSSetConstantBuffers(0, 1, m_constantBuffer);
	// Attach our pixel shader.
	m_devCon->PSSetShader(m_pixelShader, nullptr, 0);
	//context->DrawIndexed(m_indexCount, 0, 0);
	m_devCon->Draw((unsigned)verts.size(), 0);





}

void FBXRenderer::ReleaseDeviceDependentResources(void)
{
	m_dev->Release();
	m_devCon->Release();
	m_inputLayout->Release();
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_constantBuffer->Release();
	m_texture2D->Release();
	m_texView->Release();
	m_blendState->Release();
	Sphere_constantBuffer->Release();
	this->ShereVeetShader->Release();
	this->SherepixeShader->Release();

}