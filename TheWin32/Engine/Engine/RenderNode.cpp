#include "RenderNode.h"

void RenderNode::render(ID3D11DeviceContext * _DeviceContext, DirectX::XMFLOAT4X4 ProView[2], ID3D11RenderTargetView* _BackBuffer, unsigned NumOfVerts)
{
	//set matricies
	Pro_View_World PVW;
	PVW.Pro = ProView[0];
	PVW.Pro = ProView[1];
	PVW.World = _WorldMatrix;
	//set shadder
	_DeviceContext->VSSetShader(_VertexShader, 0, 0);
	_DeviceContext->PSSetShader(_PixelShader, 0, 0);
	//Set Screen to blue
	FLOAT ColorScreen[4] = { 0.0f,0.2f,0.4f,1.0f };
	_DeviceContext->ClearRenderTargetView(_BackBuffer, ColorScreen);
	//set raster state
	if (IsWireFrammed)
		_DeviceContext->RSSetState(_WireFrame);
	else
		_DeviceContext->RSSetState(_SolidFill);
	// set vertext buffer
	UINT stride = sizeof(SmartVert);
	UINT offset = 0;
	//todo: add a number for the size of the array
	//todo: 16 byte align the constant buffers
	_DeviceContext->IASetVertexBuffers(0, 1, &_VertexBuffer, &stride, &offset);
	//set Topology/primitive
	_DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//set constant buffers
	//set new matracies
	D3D11_MAPPED_SUBRESOURCE _PVWResourse;
	_DeviceContext->Map(_MatrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_PVWResourse);
	memcpy(_PVWResourse.pData, &PVW, sizeof(Pro_View_World));
	_DeviceContext->Unmap(_MatrixBuffer, NULL);
	_DeviceContext->VSSetConstantBuffers(1, 1, &_MatrixBuffer);
	//set bone matricies
	D3D11_MAPPED_SUBRESOURCE _BoneResourse;
	_DeviceContext->Map(_BoneBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_BoneResourse);
	memcpy(_BoneResourse.pData, &_Bones[0], sizeof(DirectX::XMFLOAT4X4)*(unsigned)_Bones.size());
	_DeviceContext->Unmap(_BoneBuffer, NULL);
	_DeviceContext->VSSetConstantBuffers(0, 1,&_BoneBuffer);
	//set texture if it exists
	if (_TextureBuffer != nullptr)
	{
		D3D11_MAPPED_SUBRESOURCE _TextureResourse;
		_DeviceContext->Map(_TextureBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_TextureResourse);
		memcpy(_TextureResourse.pData, &_Texture, sizeof(ID3D11Texture2D));
		_DeviceContext->Unmap(_TextureBuffer, NULL);
		_DeviceContext->PSSetConstantBuffers(0, 1, &_TextureBuffer);
	}
	_DeviceContext->Draw(NumOfVerts, 0);
}

RenderNode::RenderNode(std::vector<SmartVert> Info, ID3D11DeviceContext *devContext, ID3D11Device *Device, Pro_View_World Matricies, std::vector<DirectX::XMFLOAT4X4> Bones, ID3D11Texture2D *texture)
{
#pragma region Creating Shadders and setting them
	Device->CreateVertexShader(&(_VertShader), ARRAYSIZE((_VertShader)), NULL, &_VertexShader);
	Device->CreatePixelShader(&(_PixShader), ARRAYSIZE((_PixShader)), NULL, &_PixelShader);
	devContext->VSSetShader(_VertexShader, 0, 0);
	devContext->PSSetShader(_PixelShader, 0, 0);
#pragma endregion
#pragma region Setting Input Layout
	//todo: make inputlayout work correctly
	D3D11_INPUT_ELEMENT_DESC INPUT_DESC[] =
	{
		{ "POSITION",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMALS",      0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "UVS",          0, DXGI_FORMAT_R32G32_FLOAT,       0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BLENDWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONEINDICES",  0, DXGI_FORMAT_R32G32B32A32_UINT,  0, 48, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	Device->CreateInputLayout(INPUT_DESC, 5, _VertShader, sizeof(_VertShader), &_InputLayout);
	devContext->IASetInputLayout(_InputLayout);
#pragma endregion
#pragma region Buffers
#pragma region Vertex
	D3D11_BUFFER_DESC VertBuffDesc;
	ZeroMemory(&VertBuffDesc, sizeof(VertBuffDesc));
	VertBuffDesc.Usage = D3D11_USAGE_DYNAMIC;
	VertBuffDesc.ByteWidth = sizeof(SmartVert) * (unsigned)Info.size();
	VertBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
	VertBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = &Info[0];
	Device->CreateBuffer(&VertBuffDesc, &data, &this->_VertexBuffer);
#pragma endregion
#pragma region Constants
	//Matrixes for projection view and world
	D3D11_BUFFER_DESC _MatrixDesc;
	D3D11_MAPPED_SUBRESOURCE _MatrixResourse;
	ZeroMemory(&_MatrixDesc, sizeof(_MatrixDesc));
	_MatrixDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
	_MatrixDesc.ByteWidth = sizeof(Pro_View_World);             // size is pro_view_world
	_MatrixDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;       // use as a vertex buffer
	_MatrixDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
	Device->CreateBuffer(&_MatrixDesc, NULL, &this->_MatrixBuffer);       // create the buffer
	devContext->Map(_MatrixBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_MatrixResourse);    // map the buffer
	memcpy(_MatrixResourse.pData, &Matricies, sizeof(Pro_View_World));      // copy the data
	devContext->Unmap(_MatrixBuffer, NULL);                                      // unmap the buffer
	//Bone Matrixes
	D3D11_BUFFER_DESC _BoneDesc;
	D3D11_MAPPED_SUBRESOURCE _BoneResourse;
	ZeroMemory(&_BoneDesc, sizeof(_BoneDesc));
	_BoneDesc.Usage = D3D11_USAGE_DYNAMIC;
	_BoneDesc.ByteWidth = sizeof(DirectX::XMFLOAT4X4) * (unsigned)Bones.size();
	_BoneDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	_BoneDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	Device->CreateBuffer(&_BoneDesc, NULL, &this->_BoneBuffer);
	devContext->Map(_BoneBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_BoneResourse);
	memcpy(_BoneResourse.pData, &Bones, sizeof(DirectX::XMFLOAT4X4)*(unsigned)Bones.size());
	devContext->Unmap(_BoneBuffer, NULL);
	_Bones = Bones;
	//Texture if it exists
	if (texture != nullptr)
	{
		//_TextureBuffer
		D3D11_BUFFER_DESC _TextureDesc;
		D3D11_MAPPED_SUBRESOURCE _TextureResourse;
		ZeroMemory(&_TextureDesc, sizeof(_TextureDesc));
		_TextureDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
		_TextureDesc.ByteWidth = sizeof(ID3D11Texture2D);             // size is pro_view_world
		_TextureDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;       // use as a vertex buffer
		_TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
		Device->CreateBuffer(&_TextureDesc, NULL, &this->_TextureBuffer);       // create the buffer
		devContext->Map(_TextureBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &_TextureResourse);    // map the buffer
		memcpy(_TextureResourse.pData, &texture, sizeof(ID3D11Texture2D));      // copy the data
		devContext->Unmap(_TextureBuffer, NULL);
		_Texture = texture;
	}
#pragma endregion
#pragma endregion
#pragma region Setting WorldMarix
	DirectX::XMStoreFloat4x4(&_WorldMatrix, DirectX::XMMatrixIdentity());
#pragma endregion
#pragma region Setting RasterStates
	//wire frame
	D3D11_RASTERIZER_DESC _RasterDesc;
	ZeroMemory(&_RasterDesc, sizeof(_RasterDesc));
	_RasterDesc.CullMode = D3D11_CULL_NONE;
	_RasterDesc.FillMode = D3D11_FILL_WIREFRAME;
	Device->CreateRasterizerState(&_RasterDesc, &this->_WireFrame);
	//solid fill
	ZeroMemory(&_RasterDesc, sizeof(_RasterDesc));
	_RasterDesc.CullMode = D3D11_CULL_NONE;
	_RasterDesc.FillMode = D3D11_FILL_SOLID;
	Device->CreateRasterizerState(&_RasterDesc, &_SolidFill);
	devContext->RSSetState(_SolidFill);
#pragma endregion
#pragma region Setting Primitive
	_Primative = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
#pragma endregion
}

RenderNode::RenderNode()
{

}

RenderNode::~RenderNode()
{
}
