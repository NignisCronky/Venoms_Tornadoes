#include "FBXRenderer.h"
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
	m_constantBufferData.LightDirection = XMFLOAT4(10.0f * frame, 10.0f * frame, 10.0f * frame, 1.0f);
	m_constantBufferData.LightColor = XMFLOAT4(1.5f * frame, 1.5f * frame, 1.5f * frame, 1.0f);
}

void FBXRenderer::LoadFBXFromFile(const char *fbx, const char *bin, const wchar_t *texturePath)
{
	FBXtoBinary(fbx, bin, true);
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
}

void FBXRenderer::CreateDeviceDependentResources(void)
{
	m_dev->CreateVertexShader(&_VertShader, sizeof(_VertShader), nullptr, &m_vertexShader);

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

	CD3D11_BUFFER_DESC constantBufferDesc(sizeof(MVPDCB), D3D11_BIND_CONSTANT_BUFFER);
	m_dev->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer);

	D3D11_SUBRESOURCE_DATA m_vertexBufferData = { 0 };
	m_vertexBufferData.pSysMem = verts.data();
	m_vertexBufferData.SysMemPitch = 0;
	m_vertexBufferData.SysMemSlicePitch = 0;
	CD3D11_BUFFER_DESC m_vertexBufferDesc(verts.size() * sizeof(PNTIWVertex), D3D11_BIND_VERTEX_BUFFER);
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
	m_devCon->Draw(verts.size(), 0);
}

void FBXRenderer::ReleaseDeviceDependentResources(void)
{
	m_inputLayout->Release();
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
	m_vertexShader->Release();
	m_pixelShader->Release();
	m_constantBuffer->Release();
	m_texture2D->Release();
	m_texView->Release();
}