#include "Triangle.h"

Triangle::Triangle(ID3D11DeviceContext * Context, ID3D11Device * Device)
{
	verts[0] = XMFLOAT4(.5f, .5f, .5f, 1.0f);
	verts[1] = XMFLOAT4(-.5f, -.5f, .5f, 1.0f);
	verts[2] = XMFLOAT4(-.5f, .5f, .5f, 1.0f);

	DirectX::XMStoreFloat4x4(&this->ConstantBufferInfo.World, DirectX::XMMatrixIdentity());
	//this->ConstantBufferInfo.View = Matricies.View;
	this->ConstantBufferInfo.LightColor = DirectX::XMFLOAT4(0.4f, 0.1f, 0.8f, 1.0f);
	this->ConstantBufferInfo.LightDirection = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	//this->CalcBoneOffSets(Bones, this->ConstantBufferInfo.Boneoffsets);
	DirectX::XMStoreFloat4x4(&this->m_WorldMatrix, DirectX::XMMatrixIdentity());

	// creates and sets the vertex shader
	Device->CreateVertexShader(&Vshader, sizeof(Vshader), NULL, &m_VertexShader);
	Device->CreatePixelShader(&Pshader, sizeof(Pshader), NULL, &m_PixelShader);
	Context->VSSetShader(m_VertexShader, 0, 0);
	Context->PSSetShader(m_PixelShader, 0, 0);

	// creates input layout 
	D3D11_INPUT_ELEMENT_DESC INPUT_DESC[] =
	{
		{ "POSITION",		0, DXGI_FORMAT_R32G32B32_FLOAT,		0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};


	HRESULT error = Device->CreateInputLayout(INPUT_DESC, ARRAYSIZE(INPUT_DESC), &Vshader, sizeof(Vshader), &m_InputLayout);


	// Creates vertex buffer
	D3D11_BUFFER_DESC VertBuffDesc;
	ZeroMemory(&VertBuffDesc, sizeof(VertBuffDesc));
	VertBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertBuffDesc.ByteWidth = sizeof(XMFLOAT4) * 3;
	VertBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VertBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &verts[0];
	Device->CreateBuffer(&VertBuffDesc, &data, &this->m_Vertexs);

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
	_RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	Device->CreateRasterizerState(&_RasterDesc, &m_SolidFill);

	_Primative = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

Triangle::~Triangle()
{

}

void Triangle::Set(ID3D11DeviceContext * Context)
{
	//Context->PSSetShaderResources(0, 1, &texViews);
	//Context->IASetIndexBuffer(m_VertIndex, DXGI_FORMAT_R32_UINT, 0);
	UINT stride = sizeof(XMFLOAT4);
	UINT offset = 0;
	Context->IASetVertexBuffers(0, 1, &m_Vertexs, &stride, &offset);
	Context->IASetPrimitiveTopology((D3D11_PRIMITIVE_TOPOLOGY)_Primative);
	Context->IASetInputLayout(m_InputLayout);
	Context->VSSetShader(m_VertexShader, 0, 0);
	Context->PSSetShader(m_PixelShader, 0, 0);
	Context->VSSetConstantBuffers(0, 1, &m_ConstantBuffer);
	if (m_IsWireFrammed == true)
		Context->RSSetState(m_WireFrame);
	else
		Context->RSSetState(m_SolidFill);
}

void Triangle::Update(Pro_View_World Matricies, ID3D11DeviceContext * Context)
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

void Triangle::Draw(ID3D11RenderTargetView * _BackBuffer, ID3D11DeviceContext * Context, Pro_View_World Matricies)
{//ID3D11DeviceContext * Context, ID3D11Device * Device

	this->Update(Matricies, Context);
	this->Set(Context);
	//Context->DrawIndexed((unsigned)this->m_VertIndexContainer.size(), 0, 0);
	Context->Draw(3, 0);
}

void Triangle::Release()
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