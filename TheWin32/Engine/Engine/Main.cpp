// include the basic windows header file
#include <Windows.h>
#include"Globals.h"
#include <d3d11.h> 
#include <D3DX11.h>
#include "Vshader.csh"
#include "Pshader.csh"
#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <directxmath.h>

#include <directxcolors.h>
#pragma comment (lib, "d3d11.lib")
using DirectX::XMMATRIX;
using DirectX::XMFLOAT4X4;

#pragma region Globals

IDXGISwapChain *swapchain;						// the pointer to the swap chain
ID3D11Device *Device;							// the pointer to the Device
ID3D11DeviceContext *Devicecon;					// the pointer to the Device context
ID3D11RenderTargetView *backbuffer;				// the pointer to the backbuffer
ID3D11VertexShader *_VertexShader;				// the pointer to the Vertex Shader
ID3D11PixelShader *_PixelShader;				// the pointer to the Pixel Shader
ID3D11Buffer *_Buffer;							// the pointer to the vertex buffer
ID3D11Buffer *_ConstantBuffer;					// the pointer to the constant buffer
ID3D11InputLayout *_Layout;						// the pointer to the input layout
ID3D11RasterizerState *RasterState;				// the pointer to the raster state
struct VERTEX {									// vertex structure
	FLOAT X, Y, Z;
	DirectX::XMFLOAT4 Color;
};
DirectX::XMFLOAT4X4 _ProjectionMatrix;			//projection matrix
DirectX::XMFLOAT4X4 _ViewMatrix;				//view matrix
DirectX::XMFLOAT4X4 _WorldMatrix;				//world matrix
struct Pro_View_World
{
	XMFLOAT4X4 World;
	XMFLOAT4X4 Pro;
	XMFLOAT4X4 View;
};

#pragma endregion

#pragma region Foward Declarations

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // WindowProc Foward Declaritaion

#pragma endregion

#pragma region HelperFunctions
void SetUpMatrices()
{
	DirectX::XMMATRIX Temp = DirectX::XMMatrixPerspectiveFovLH(60.0f, (4.0f / 3.0f), 0.1f, 1000);
	DirectX::XMStoreFloat4x4(&_ProjectionMatrix, Temp);
	Temp = DirectX::XMMatrixTranslation(0, 0, -2);
	DirectX::XMStoreFloat4x4(&_ViewMatrix, Temp);
	DirectX::XMStoreFloat4x4(&_WorldMatrix, DirectX::XMMatrixIdentity());

}

void Register(HINSTANCE Instance, WNDCLASSEX WindowClass)
{

	// fill in the struct with the needed information
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.hInstance = Instance;
	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	// the windows backgound : remove this for fullscreen effects
	//WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	WindowClass.lpszClassName = L"WindowClass1";

	// register the window class
	RegisterClassEx(&WindowClass);
}

void SetElement(int index)
{
	switch (index)
	{
	case 0:
	{
		D3D11_INPUT_ELEMENT_DESC INPUT_DESC[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		Device->CreateInputLayout(INPUT_DESC, 2, Vshader, sizeof(Vshader), &_Layout);
		Devicecon->IASetInputLayout(_Layout);
	}
	break;
	default:
		break;
	}
}

#pragma endregion

#pragma region DirectX_Init

void CleanD3D(void)
{
	swapchain->SetFullscreenState(FALSE, NULL);
	// close and release all existing COM objects
	swapchain->Release();
	backbuffer->Release();
	Device->Release();
	Devicecon->Release();

	_VertexShader->Release();
	_PixelShader->Release();
	RasterState->Release();

}

void RenderFrame(void)
{
	// clear the back buffer to a deep blue
	FLOAT ColorScreen[4] = { 0.0f,0.2f,0.4f,1.0f };
	Devicecon->ClearRenderTargetView(backbuffer, ColorScreen);
	///////////////////////////////////////////////////////////////////

	Devicecon->RSSetState(RasterState);
	UINT stride = sizeof(VERTEX);
	UINT offset = 0;
	Devicecon->IASetVertexBuffers(0, 1, &_Buffer, &stride, &offset);
	Devicecon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Devicecon->VSSetConstantBuffers(0, 1, &_ConstantBuffer);
	Devicecon->Draw(3, 0);

	///////////////////////////////////////////////////////////////////
	// switch the back buffer and the front buffer
	swapchain->Present(0, 0);

}

void InitPipeline()
{
	Device->CreateVertexShader(&(Vshader), ARRAYSIZE((Vshader)), NULL, &_VertexShader);
	Device->CreatePixelShader(&(Pshader), ARRAYSIZE((Pshader)), NULL, &_PixelShader);

	Devicecon->VSSetShader(_VertexShader, 0, 0);
	Devicecon->PSSetShader(_PixelShader, 0, 0);

	SetElement(0);
	SetUpMatrices();

	//d3d11_RASTERSTATE_DESC rasterdesc
	D3D11_RASTERIZER_DESC RasDesc;
	ZeroMemory(&RasDesc, sizeof(RasDesc));
	RasDesc.CullMode = D3D11_CULL_NONE;
	RasDesc.FillMode = D3D11_FILL_SOLID;
	Device->CreateRasterizerState(&RasDesc,&RasterState);

}

void InitGraphics()
{
	VERTEX Triangle[] =
	{
		{ 0.0f, 0.0f, 0.5f,	{1.0f, 0.0f, 0.0f, 1.0f} },
		{ 10.0f, 0.0f, 0.5f, {0.0f, 1.0f, 0.0f, 1.0f} },
		{ 5.0f, 10.0f, 0.5f, {0.0f, 0.0f, 1.0f, 1.0f} }
	};


//Buffer
D3D11_BUFFER_DESC BufferDes;
ZeroMemory(&BufferDes, sizeof(BufferDes)); 

BufferDes.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
BufferDes.ByteWidth = sizeof(VERTEX) * 3;             // size is the VERTEX struct * 3
BufferDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
BufferDes.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

D3D11_SUBRESOURCE_DATA data;
data.pSysMem = &Triangle[0];
Device->CreateBuffer(&BufferDes, &data, &_Buffer);       // create the buffer

//D3D11_MAPPED_SUBRESOURCE Resourse;
//Devicecon->Map(_Buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &Resourse);    // map the buffer
//memcpy(Resourse.pData, Triangle, sizeof(Triangle));                 // copy the data
//Devicecon->Unmap(_Buffer, NULL);                                      // unmap the buffer

Pro_View_World MAtrices;
MAtrices.World = _WorldMatrix;
MAtrices.Pro = _ProjectionMatrix;
MAtrices.View = _ViewMatrix;

D3D11_BUFFER_DESC ConstantBuffer;
ZeroMemory(&ConstantBuffer, sizeof(ConstantBuffer));

ConstantBuffer.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
ConstantBuffer.ByteWidth = sizeof(Pro_View_World);             // size is pro_view_world
ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;       // use as a vertex buffer
ConstantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer

Device->CreateBuffer(&ConstantBuffer, NULL, &_ConstantBuffer);       // create the buffer

D3D11_MAPPED_SUBRESOURCE ConsREsorce;
Devicecon->Map(_ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ConsREsorce);    // map the buffer
memcpy(ConsREsorce.pData, &MAtrices, sizeof(Pro_View_World));      // copy the data
Devicecon->Unmap(_ConstantBuffer, NULL);                                      // unmap the buffer




}

void InitD3D(HWND hWnd)
{
	












	// create a struct to hold information about the swap chain
	DXGI_SWAP_CHAIN_DESC scd;

	// clear out the struct for use
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));

	// fill the swap chain description struct
	scd.BufferCount = 1;                                    // one back buffer
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scd.OutputWindow = hWnd;                                // the window to be used
	//fullscreen/resolution settings
	scd.BufferDesc.Width = WIDTH_P;
	scd.BufferDesc.Height = HEIGHT_P;


	scd.SampleDesc.Count = 4;                               // how many multisamples
	scd.Windowed = TRUE;                                    // windowed/full-screen mode
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // this enables fullscreen switching with alt+enter

	// create a Deviceice, Deviceice context and swap chain using the information in the scd struct
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &scd, &swapchain, &Device, NULL, &Devicecon);

	//////////////////////
	// Back Buffer: renderTarget
	//////////////////////

	// get the address of the back buffer
	ID3D11Texture2D *pBackBuffer;
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

	// use the back buffer address to create the render target
	Device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
	pBackBuffer->Release();

	// set the render target as the back buffer
	Devicecon->OMSetRenderTargets(1, &backbuffer, NULL);

	////////////////// ViewPort

	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = WIDTH_P;
	viewport.Height = HEIGHT_P;

	Devicecon->RSSetViewports(1, &viewport);

	InitPipeline();
	InitGraphics();
	////////////////
	Devicecon->RSSetState(RasterState);
}

#pragma endregion

#pragma region Windows Code

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	{
		WNDCLASSEX WindowClass;// this struct holds information for the window class
		ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));
		//////////// 3 window functions that set up the window //////// 
		Register(hInstance, WindowClass);
		RECT wr = { 0, 0, WIDTH_W, HEIGHT_W };
		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
		HWND hWnd = CreateWindowEx(NULL, L"WindowClass1", L"POISON", WS_OVERLAPPEDWINDOW, 200, 150, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
		ShowWindow(hWnd, nCmdShow);
		///////////////////////////////////////////////////////////////
		InitD3D(hWnd);
		///////////////
		//msg has to be zero or else it will error with peekmessage: if you want to not intialize it use getMessage(), but get message is blocking
		MSG msg = { 0 };
		while (TRUE)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					break;
			}
			else
			{
				//game code goes here
			}

			RenderFrame();
		}
		CleanD3D();
		return (int)msg.wParam;
	}
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	// sort through and find what code to run for the message given
	switch (message)
	{
		// this message is read when the window is closed
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	break;

	}

	// Handle any messages the switch statement didn't
	return DefWindowProc(hWnd, message, wParam, lParam);
}

#pragma endregion
