#include "Render.h"



void Render::Update(Pro_View_World Matricies, ID3D11DeviceContext * Context)
{
	this->ConstantBufferInfo.View = Matricies.View;
	this->ConstantBufferInfo.Pro = Matricies.Pro;
	this->ConstantBufferInfo.World = this->m_WorldMatrix;

	//Update View, World Projection
	D3D11_MAPPED_SUBRESOURCE _ConstanteResourse;
	Context->Map(m_ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_ConstanteResourse);
	memcpy(_ConstanteResourse.pData, &ConstantBufferInfo, sizeof(ConstantBufferInfo));
	Context->Unmap(m_ConstantBuffer, NULL);
}

void Render::Draw(ID3D11RenderTargetView * _BackBuffer, ID3D11DeviceContext * Context, Pro_View_World Matricies)
{//ID3D11DeviceContext * Context, ID3D11Device * Device

	this->Update(Matricies, Context);
	this->Set(Context);
	Context->DrawIndexed((unsigned)this->m_VertIndexContainer.size(), 0, 0);
}

Render::Render()
{

}

Render::Render(ID3D11Texture2D * texture, Pro_View_World Matricies, std::vector<unsigned> VertIndex, std::vector<Joint> Bones, std::vector<PNTIWVertex> Vertexs, ID3D11DeviceContext * Context, ID3D11Device * Device)
{
	// Grabs the view, and projection Passed in
	// sets the world matrix to idenity
	this->ConstantBufferInfo.View = Matricies.View;
	DirectX::XMStoreFloat4x4(&this->ConstantBufferInfo.World, DirectX::XMMatrixIdentity());
	this->ConstantBufferInfo.View = Matricies.View;
	this->ConstantBufferInfo.LightColor = DirectX::XMFLOAT4(0.4f, 0.1f, 0.8f, 1.0f);
	this->ConstantBufferInfo.LightDirection = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	this->ConstantBufferInfo.Boneoffsets = this->CalcBoneOffSets(Bones);
	DirectX::XMStoreFloat4x4(&this->m_WorldMatrix, DirectX::XMMatrixIdentity());

	// creates and sets the vertex shader
	Device->CreateVertexShader(&(_VertShader), ARRAYSIZE((_VertShader)), NULL, &m_VertexShader);
	Device->CreatePixelShader(&(_PixShader), ARRAYSIZE((_PixShader)), NULL, &m_PixelShader);

	// creates input layout 
	D3D11_INPUT_ELEMENT_DESC INPUT_DESC[] =
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVS",          0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES",  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Device->CreateInputLayout(INPUT_DESC, 5, _VertShader, sizeof(_VertShader), &m_InputLayout);


	// Creates vertex buffer
	D3D11_BUFFER_DESC VertBuffDesc;
	ZeroMemory(&VertBuffDesc, sizeof(VertBuffDesc));
	VertBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertBuffDesc.ByteWidth = sizeof(PNTIWVertex) * (unsigned)Vertexs.size();
	VertBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &Vertexs[0];
	Device->CreateBuffer(&VertBuffDesc, &data, &this->m_Vertexs);

	//Creates Index buffer for verts
	D3D11_BUFFER_DESC IndexBuffDesc;
	ZeroMemory(&IndexBuffDesc, sizeof(IndexBuffDesc));
	IndexBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	IndexBuffDesc.ByteWidth = sizeof(unsigned) * (unsigned)VertIndex.size();
	IndexBuffDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IndexBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	data.pSysMem = &VertIndex[0];
	Device->CreateBuffer(&IndexBuffDesc, &data, &this->m_VertIndex);

	//create constant buffer
	D3D11_BUFFER_DESC _Constant;
	D3D11_MAPPED_SUBRESOURCE _ConstanteResourse;
	ZeroMemory(&_Constant, sizeof(_Constant));
	_Constant.Usage = D3D11_USAGE_DYNAMIC;
	_Constant.ByteWidth = sizeof(ConstantBufferInfo);
	_Constant.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	_Constant.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Device->CreateBuffer(&_Constant, NULL, &this->m_ConstantBuffer);
	Context->Map(m_ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_ConstanteResourse);
	memcpy(_ConstanteResourse.pData, &ConstantBufferInfo, sizeof(ConstantBufferInfo));
	Context->Unmap(m_ConstantBuffer, NULL);

	//wire frame
	D3D11_RASTERIZER_DESC _RasterDesc;
	ZeroMemory(&_RasterDesc, sizeof(_RasterDesc));
	_RasterDesc.CullMode = D3D11_CULL_NONE;
	_RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	Device->CreateRasterizerState(&_RasterDesc, &this->m_WireFrame);
	//solid fill
	ZeroMemory(&_RasterDesc, sizeof(_RasterDesc));
	_RasterDesc.CullMode = D3D11_CULL_NONE;
	_RasterDesc.FillMode = D3D11_FILL_SOLID;
	Device->CreateRasterizerState(&_RasterDesc, &m_SolidFill);

	_Primative = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}


Render::~Render()
{

}

void Render::Release()
{
	m_ConstantBuffer->Release();
	m_InputLayout->Release();
	m_PixelShader->Release();
	m_SolidFill->Release();
	m_WireFrame->Release();
	m_Vertexs->Release();
	m_VertIndex->Release();
	m_VertexShader->Release();
}

void Render::Create()
{
}

void Render::Set(ID3D11DeviceContext * Context)
{
	Context->IASetInputLayout(m_InputLayout);
	Context->VSSetShader(m_VertexShader, 0, 0);
	Context->PSSetShader(m_PixelShader, 0, 0);
	Context->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	if (m_IsWireFrammed == true)
		Context->RSSetState(m_WireFrame);
	else
		Context->RSSetState(m_SolidFill);


}
