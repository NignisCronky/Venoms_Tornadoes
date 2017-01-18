#include <windows.h> 
#define Width_ 500
#define Height_ 400
#include <d3d11.h>
#pragma comment (lib, "d3d11.lib")
#include "FBXRenderer.h"



LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool WindowsSetup(HWND& WindowHandle, HINSTANCE hInstance, int nShowCmd);
bool Init(HWND &hWnd, FBXRenderer Array[3]);
bool InitCamera();
bool InitGraphics(HWND& hWnd);
void CleanD3D();
void RenderFrame();
bool InitMeshes(FBXRenderer &Box, FBXRenderer &Bear, FBXRenderer &wizard);

IDXGISwapChain *SwapChain;
ID3D11Device *Device;
ID3D11DeviceContext *DeviceContext;
ID3D11RenderTargetView* BackBuffer;
XMFLOAT4X4 Camera;


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	HWND WindowHandle;
	WindowsSetup(WindowHandle, hInstance, nShowCmd);

	FBXRenderer BoxBearWizard[3] = {FBXRenderer(*Device, Camera, *DeviceContext),FBXRenderer(*Device, Camera, *DeviceContext),FBXRenderer(*Device, Camera, *DeviceContext)};
	Init(WindowHandle, BoxBearWizard);

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
		RenderFrame();
	}
}
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	} break;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}
bool WindowsSetup(HWND& WindowHandle, HINSTANCE hInstance, int nShowCmd)
{

	WNDCLASSEX WindowsClassDesc;
	ZeroMemory(&WindowsClassDesc, sizeof(WNDCLASSEX));
	WindowsClassDesc.cbSize = sizeof(WNDCLASSEX);
	WindowsClassDesc.style = CS_HREDRAW | CS_VREDRAW;
	WindowsClassDesc.lpfnWndProc = WindowProc;
	WindowsClassDesc.hInstance = hInstance;
	WindowsClassDesc.hCursor = LoadCursor(NULL, IDC_ARROW);
	WindowsClassDesc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	WindowsClassDesc.lpszClassName = L"WindowClass1";
	RegisterClassEx(&WindowsClassDesc);

	RECT wr = { 0, 0, Width_, Height_ };
	AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
	WindowHandle = CreateWindowEx(NULL, L"WindowClass1", L"Jordan & Logano Schorch", WS_OVERLAPPEDWINDOW, 50, 50, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
	ShowWindow(WindowHandle, nShowCmd);
	return true;
}
bool Init(HWND& hWnd, FBXRenderer Array[3])
{

	InitGraphics(hWnd);
	InitMeshes(Array[0], Array[1], Array[2]);

	return true;
}

bool InitCamera()
{
	return false;
}


bool InitGraphics(HWND& hWnd)
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	ZeroMemory(&SwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	SwapChainDesc.BufferCount = 1;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = hWnd;
	SwapChainDesc.SampleDesc.Count = 4;
	SwapChainDesc.Windowed = TRUE;
	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, NULL, NULL, D3D11_SDK_VERSION, &SwapChainDesc, &SwapChain, &Device, NULL, &DeviceContext);

	ID3D11Texture2D *BackBufferTexture;
	SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBufferTexture);
	Device->CreateRenderTargetView(BackBufferTexture, NULL, &BackBuffer);
	DeviceContext->OMSetRenderTargets(1, &BackBuffer, NULL);


	D3D11_VIEWPORT ViewPort;
	ZeroMemory(&ViewPort, sizeof(ViewPort));
	ViewPort.TopLeftX = 0;
	ViewPort.TopLeftY = 0;
	ViewPort.Width = 800;
	ViewPort.Height = 600;
	DeviceContext->RSSetViewports(1, &ViewPort);
	return true;
}

void CleanD3D()
{
	SwapChain->Release();
	BackBuffer->Release();
	Device->Release();
	DeviceContext->Release();
}

void RenderFrame()
{
	FLOAT RedBackGround[4] = { 1.0f,0.0f,0.0f,1.0f };
	DeviceContext->ClearRenderTargetView(BackBuffer, RedBackGround);

	SwapChain->Present(0, 0);
}

bool InitMeshes(FBXRenderer &Box, FBXRenderer &Bear, FBXRenderer &wizard)
{
	FBXRenderer Temp(*Device, Camera, *DeviceContext);
	// fbx file path, binary path, texture
	Temp.LoadFBXFromFile(
		"../Original Assets//AnimatedBox/Box_Idle.fbx",
		"../Exports/Box_Idle.bin" ,
		L"../Original Assets/AnimatedBox/Box_Idle.fbm/TestCube.dds"
		);
	Box = Temp;
	//todo: fix escape sequences
	Temp.LoadFBXFromFile(
		"../Original Assets/Teddy/Teddy_Idle.fbx",
		"../Exports/Teddy_Idle.bin",
		L"../Original Assets/Teddy/Teddy_Idle.fbm/Teddy_D.dds"
		);
	Bear = Temp;

	Temp.LoadFBXFromFile(
		"../Original Assets/Mage/Idle.fbx",
		"../Exports/Idle.bin",
		L"../Original Assets/Mage/Battle Mage with Rig and textures.fbm/PPG_3D_Player_D.dds"
		);
	wizard = Temp;

	return false;
}


// ;-^--------------------) kill me
#pragma region IsKill


//#pragma once
//#include "Render.h"
//
////&swapchain, &Device, NULL, &Devicecon, backbuffer
//
//IDXGISwapChain* swapchain;
//ID3D11Device* Device;
//ID3D11DeviceContext* Devicecon;
//ID3D11RenderTargetView* backbuffer;
//ID3D11ShaderResourceView* shaderResourceView;
//ID3D11DepthStencilView * DepthStencilView;
//
//Pro_View_World PVW;
//
//
//
//
//#pragma region Globals
//#define PI 3.141592653f
//
////Camera Shit
////Matrix data member for the camera
//XMFLOAT4X4 m_camera;
//bool rightHeld = false;
//Timer m_timer;
//
//struct VERTEX {									// vertex structure
//	FLOAT X, Y, Z;
//	DirectX::XMFLOAT4 Color;
//};
//
//struct Vec4
//{
//	float pos[4];
//	Vec4(float x, float y, float z, float w)
//	{
//		pos[0] = x;
//		pos[1] = y;
//		pos[2] = z;
//		pos[3] = w;
//	}
//	Vec4()
//	{
//		pos[0] = 0;
//		pos[1] = 0;
//		pos[2] = 0;
//		pos[3] = 0;
//	}
//};
//
//DirectX::XMFLOAT4X4 _ProjectionMatrix;			//projection matrix
//DirectX::XMFLOAT4X4 _ViewMatrix;				//view matrix
//DirectX::XMFLOAT4X4 _WorldMatrix;				//world matrix
//
//
//enum ShadingMode
//{
//	SHADING_MODE_WIREFRAME,
//	SHADING_MODE_SHADED,
//};
//
//const char* filename = "../Original Assets/AnimatedBox/Box_Idle.fbx";
//const char* spherename = "../Original Assets/spherical.fbx";
//ID3D11RasterizerState** rasState;
//bool wired = false;
//#pragma endregion
//
//#pragma region Foward Declarations
//
//LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam); // WindowProc Foward Declaritaion
//
//#pragma endregion
//void Register(HINSTANCE Instance, WNDCLASSEX WindowClass)
//{
//
//	// fill in the struct with the needed information
//	WindowClass.cbSize = sizeof(WNDCLASSEX);
//	WindowClass.style = CS_HREDRAW | CS_VREDRAW;
//	WindowClass.lpfnWndProc = WindowProc;
//	WindowClass.hInstance = Instance;
//	WindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
//	// the windows backgound : remove this for fullscreen effects
//	//WindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
//	WindowClass.lpszClassName = L"WindowClass1";
//
//	// register the window class
//	RegisterClassEx(&WindowClass);
//}
//
//#pragma region HelperFunctions
//void SetUpMatrices(Pro_View_World& pvw)
//{
//	float aspectRatio = (float)WIDTH_P / (float)HEIGHT_P;
//	float fovAngleY = 60.0f * DirectX::XM_PI / 180.0f;
//	
//	if (aspectRatio < 1.0f)
//	{
//		fovAngleY *= 2.0f;
//	}
//	XMMATRIX perspectiveMatrix = DirectX::XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.01f, 1000.0f);
//	DirectX::XMStoreFloat4x4(&pvw.Pro, perspectiveMatrix);
//
//	perspectiveMatrix = DirectX::XMMatrixTranslation(0, 2.0f,8.0f);
//	DirectX::XMStoreFloat4x4(&pvw.View, XMMatrixInverse(nullptr,perspectiveMatrix));
//
//	DirectX::XMStoreFloat4x4(&pvw.World, DirectX::XMMatrixIdentity());
//}
//
//void InitD3D(HWND hWnd)
//{
//	// create a struct to hold information about the swap chain
//	DXGI_SWAP_CHAIN_DESC scd;
//
//	// clear out the struct for use
//	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
//
//	// fill the swap chain description struct
//	scd.BufferCount = 1;                                    // one back buffer
//	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
//	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
//	scd.OutputWindow = hWnd;                                // the window to be used
//															//fullscreen/resolution settings
//	scd.BufferDesc.Width = WIDTH_P;
//	scd.BufferDesc.Height = HEIGHT_P;
//
//
//	scd.SampleDesc.Count = 1;                               // how many multisamples
//	scd.Windowed = TRUE;                                    // windowed/full-screen mode
//	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH; // this enables fullscreen switching with alt+enter
//
//														// create a Deviceice, Deviceice context and swap chain using the information in the scd struct
//	D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION, &scd, &swapchain, &Device, NULL, &Devicecon);
//
//	//////////////////////
//	// Back Buffer: renderTarget
//	//////////////////////
//
//	// get the address of the back buffer
//	ID3D11Texture2D* pBackBuffer;
//	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
//
//	// use the back buffer address to create the render target
//	Device->CreateRenderTargetView(pBackBuffer, NULL, &backbuffer);
//
//	ID3D11Texture2D *Resourse2D;
//	D3D11_TEXTURE2D_DESC Resourse2DDesc;
//	ZeroMemory(&Resourse2DDesc, sizeof(Resourse2DDesc));
//	Resourse2DDesc.Width = WIDTH_P;
//	Resourse2DDesc.Height = HEIGHT_P;
//	Resourse2DDesc.MipLevels = 1;
//	Resourse2DDesc.ArraySize = 1;
//	Resourse2DDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
//	Resourse2DDesc.SampleDesc.Count = 1;
//	Resourse2DDesc.SampleDesc.Quality = 0;
//	Resourse2DDesc.Usage = D3D11_USAGE_DEFAULT;
//	Resourse2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
//
//
//	Device->CreateTexture2D(&Resourse2DDesc, NULL, &Resourse2D);
//
//
//	D3D11_DEPTH_STENCIL_VIEW_DESC DepthDesc;
//	ZeroMemory(&DepthDesc, sizeof(DepthDesc));
//	DepthDesc.Format = Resourse2DDesc.Format;
//	DepthDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
//
//
//	Device->CreateDepthStencilView(Resourse2D, &DepthDesc, &DepthStencilView);
//
//	pBackBuffer->Release();
//
//	// set the render target as the back buffer
//	Devicecon->OMSetRenderTargets(1, &backbuffer, DepthStencilView);
//
//	////////////////// ViewPort
//
//	D3D11_VIEWPORT viewport;
//	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
//
//	viewport.TopLeftX = 0;
//	viewport.TopLeftY = 0;
//	viewport.Width = WIDTH_P;
//	viewport.Height = HEIGHT_P;
//	viewport.MaxDepth = 1;
//	Devicecon->RSSetViewports(1, &viewport);
//}
//
//#pragma region Windows Code
//
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//	{
//		WNDCLASSEX WindowClass;// this struct holds information for the window class
//		ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));
//		//////////// 3 window functions that set up the window //////// 
//		Register(hInstance, WindowClass);
//		RECT wr = { 0, 0, WIDTH_W, HEIGHT_W };
//		AdjustWindowRect(&wr, WS_OVERLAPPEDWINDOW, FALSE);
//		HWND hWnd = CreateWindowEx(NULL, L"WindowClass1", L"POISON", WS_OVERLAPPEDWINDOW, 200, 150, wr.right - wr.left, wr.bottom - wr.top, NULL, NULL, hInstance, NULL);
//		ShowWindow(hWnd, nCmdShow);
//		///////////////////////////////////////////////////////////////
//		FBXtoBinary("../Original Assets//AnimatedBox/Box_Idle.fbx", "../Exports/Box_Idle.bin", true);
//		//FBXtoBinary("../Original Assets/Teddy/Teddy_Idle.fbx", "../Exports/Teddy_Idle.bin", false);
//		Skeleton skelly;
//		std::vector<unsigned int> indicies;
//		std::vector<PNTIWVertex> verts;
//		ReadBinary("../Exports/Box_Idle.bin", &skelly, &indicies, &verts);
//		//ReadBinary("../Exports/Teddy_Idle.bin", &skelly, &indicies, &verts);
//		ID3D11Texture2D *k;
//		InitD3D(hWnd);
//		SetUpMatrices(PVW);
//		swapchain->Present(0, 0);
//		CreateDDSTextureFromFile(Device, L"../Original Assets/AnimatedBox/Box_Idle.fbm/TestCube.dds", (ID3D11Resource**)&k, &shaderResourceView);
//		Render Bear(shaderResourceView, PVW, indicies, skelly.mJoints,verts, Devicecon, Device);
//		
//
//
//
//		///////////////
//		//msg has to be zero or else it will error with peekmessage: if you want to not intialize it use getMessage(), but get message is blocking
//		MSG msg = { 0 };
//		
//		m_timer.Reset();
//		while (TRUE)
//		{
//			float delta = m_timer.GetElapsedTime() * 4.0f;
//			m_timer.Reset();
//			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
//			{
//				TranslateMessage(&msg);
//				DispatchMessage(&msg);
//				if (msg.message == WM_QUIT)
//					break;
//			}
//			else
//			{
//				//game code goes here
//			}
//			FLOAT ColorScreen[4] = { 0.0f,0.2f,0.4f,1.0f };
//			Devicecon->ClearRenderTargetView(backbuffer, ColorScreen);
//			Devicecon->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH, 1, 0);
//			Bear.Draw(backbuffer, Devicecon, PVW);
//			swapchain->Present(0, 0);
//		}
//		Bear.Release();
//		return (int)msg.wParam;
//	}
//}
//
//LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//{
//	// sort through and find what code to run for the message given
//	switch (message)
//	{
//		// this message is read when the window is closed
//	case WM_DESTROY:
//	{
//		// close the application entirely
//		PostQuitMessage(0);
//		return 0;
//	}
//	}
//
//	// Handle any messages the switch statement didn't
//	return DefWindowProc(hWnd, message, wParam, lParam);
//}






//// Find all the cameras under this node recursively.
//void FillCameraArrayRecursive(FbxNode* pNode, FbxArray<FbxNode*>& pCameraArray)
//{
//	if (pNode)
//	{
//		if (pNode->GetNodeAttribute())
//		{
//			if (pNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eCamera)
//			{
//				pCameraArray.Add(pNode);
//			}
//		}
//
//		const int lCount = pNode->GetChildCount();
//		for (int i = 0; i < lCount; i++)
//		{
//			FillCameraArrayRecursive(pNode->GetChild(i), pCameraArray);
//		}
//	}
//}
//
//// Find all the cameras in this scene.
//void FillCameraArray(FbxScene* pScene, FbxArray<FbxNode*>& pCameraArray)
//{
//	pCameraArray.Clear();
//
//	FillCameraArrayRecursive(pScene->GetRootNode(), pCameraArray);
//}
//
//// Find all poses in this scene.
//void FillPoseArray(FbxScene* pScene, FbxArray<FbxPose*>& pPoseArray)
//{
//	const int lPoseCount = pScene->GetPoseCount();
//
//	for (int i = 0; i < lPoseCount; ++i)
//	{
//		pPoseArray.Add(pScene->GetPose(i));
//	}
//}
//
////MeshData CreateSphere(float radius, int sliceCount, int stackCount) {
//	MeshData* ret = new MeshData();
//	vec3f ver;
//	ver.x = 0;
//	ver.y = radius;
//	ver.z = 0;
//	ret->Vertices.push_back(ver);
//	float phiStep = PI / stackCount;
//	float thetaStep = 2.0f*PI / sliceCount;
//
//	for (int i = 1; i <= stackCount - 1; i++) {
//		float phi = i*phiStep;
//		for (int j = 0; j <= sliceCount; j++) {
//			float theta = j*thetaStep;
//			vec3f p = vec3f(
//				(radius*sinf(phi)*cosf(theta)),
//				(radius*cosf(phi)),
//				(radius*sinf(phi)*sinf(theta))
//			);
//			
//			vec3f t = vec3f(-radius*sinf(phi)*sinf(theta), 0, radius*sinf(phi)*cosf(theta));
//			t.normalize();
//			vec3f n = p;
//			n.normalize();
//			vec3f uv = vec3f(theta / (PI * 2), phi / PI, 1);
//			ret->Vertices.push_back(vec3f(p, n, t, uv));
//		}
//	}
//	ret.Vertices.Add(new Vertex(0, -radius, 0, 0, -1, 0, 1, 0, 0, 0, 1));
//
//
//	for (int i = 1; i <= sliceCount; i++) {
//		ret.Indices.Add(0);
//		ret.Indices.Add(i + 1);
//		ret.Indices.Add(i);
//	}
//	var baseIndex = 1;
//	var ringVertexCount = sliceCount + 1;
//	for (int i = 0; i < stackCount - 2; i++) {
//		for (int j = 0; j < sliceCount; j++) {
//			ret.Indices.Add(baseIndex + i*ringVertexCount + j);
//			ret.Indices.Add(baseIndex + i*ringVertexCount + j + 1);
//			ret.Indices.Add(baseIndex + (i + 1)*ringVertexCount + j);
//
//			ret.Indices.Add(baseIndex + (i + 1)*ringVertexCount + j);
//			ret.Indices.Add(baseIndex + i*ringVertexCount + j + 1);
//			ret.Indices.Add(baseIndex + (i + 1)*ringVertexCount + j + 1);
//		}
//	}
//	var southPoleIndex = ret.Vertices.Count - 1;
//	baseIndex = southPoleIndex - ringVertexCount;
//	for (int i = 0; i < sliceCount; i++) {
//		ret.Indices.Add(southPoleIndex);
//		ret.Indices.Add(baseIndex + i);
//		ret.Indices.Add(baseIndex + i + 1);
//	}
//	return ret;
//}
//
//// Draw a limb between the node and its parent.
//void DrawSkeleton(FbxNode* pNode, FbxAMatrix& pParentGlobalPosition, FbxAMatrix& pGlobalPosition)
//{
//	FbxSkeleton* lSkeleton = (FbxSkeleton*)pNode->GetNodeAttribute();
//
//	// Only draw the skeleton if it's a limb node and if 
//	// the parent also has an attribute of type skeleton.
//	if (lSkeleton->GetSkeletonType() == FbxSkeleton::eLimbNode &&
//		pNode->GetParent() &&
//		pNode->GetParent()->GetNodeAttribute() &&
//		pNode->GetParent()->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
//	{
//
//		//GlDrawLimbNode(pParentGlobalPosition, pGlobalPosition);
//	}
//}
//
//// Draw the vertices of a mesh.
////void DrawMesh(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
////	FbxAMatrix& pGlobalPosition, FbxPose* pPose, ShadingMode pShadingMode)
////{
////	FbxMesh* lMesh = pNode->GetMesh();
////	const int lVertexCount = lMesh->GetControlPointsCount();
////
////	// No vertex to draw.
////	if (lVertexCount == 0)
////	{
////		return;
////	}
////
////	const VBOMesh * lMeshCache = static_cast<const VBOMesh *>(lMesh->GetUserDataPtr());
////
////	// If it has some defomer connection, update the vertices position
////	const bool lHasVertexCache = lMesh->GetDeformerCount(FbxDeformer::eVertexCache) &&
////		(static_cast<FbxVertexCacheDeformer*>(lMesh->GetDeformer(0, FbxDeformer::eVertexCache)))->Active.Get();
////	const bool lHasShape = lMesh->GetShapeCount() > 0;
////	const bool lHasSkin = lMesh->GetDeformerCount(FbxDeformer::eSkin) > 0;
////	const bool lHasDeformation = lHasVertexCache || lHasShape || lHasSkin;
////
////	FbxVector4* lVertexArray = NULL;
////	if (!lMeshCache || lHasDeformation)
////	{
////		lVertexArray = new FbxVector4[lVertexCount];
////		memcpy(lVertexArray, lMesh->GetControlPoints(), lVertexCount * sizeof(FbxVector4));
////	}
////
////	if (lHasDeformation)
////	{
////		// Active vertex cache deformer will overwrite any other deformer
////		if (lHasVertexCache)
////		{
////			ReadVertexCacheData(lMesh, pTime, lVertexArray);
////		}
////		else
////		{
////			if (lHasShape)
////			{
////				// Deform the vertex array with the shapes.
////				ComputeShapeDeformation(lMesh, pTime, pAnimLayer, lVertexArray);
////			}
////
////			//we need to get the number of clusters
////			const int lSkinCount = lMesh->GetDeformerCount(FbxDeformer::eSkin);
////			int lClusterCount = 0;
////			for (int lSkinIndex = 0; lSkinIndex < lSkinCount; ++lSkinIndex)
////			{
////				lClusterCount += ((FbxSkin *)(lMesh->GetDeformer(lSkinIndex, FbxDeformer::eSkin)))->GetClusterCount();
////			}
////			if (lClusterCount)
////			{
////				// Deform the vertex array with the skin deformer.
////				ComputeSkinDeformation(pGlobalPosition, lMesh, pTime, lVertexArray, pPose);
////			}
////		}
////
////		if (lMeshCache)
////			lMeshCache->UpdateVertexPosition(lMesh, lVertexArray);
////	}
////
////	glPushMatrix();
////	glMultMatrixd((const double*)pGlobalPosition);
////
////	if (lMeshCache)
////	{
////		lMeshCache->BeginDraw(pShadingMode);
////		const int lSubMeshCount = lMeshCache->GetSubMeshCount();
////		for (int lIndex = 0; lIndex < lSubMeshCount; ++lIndex)
////		{
////			if (pShadingMode == SHADING_MODE_SHADED)
////			{
////				const FbxSurfaceMaterial * lMaterial = pNode->GetMaterial(lIndex);
////				if (lMaterial)
////				{
////					const MaterialCache * lMaterialCache = static_cast<const MaterialCache *>(lMaterial->GetUserDataPtr());
////					if (lMaterialCache)
////					{
////						lMaterialCache->SetCurrentMaterial();
////					}
////				}
////				else
////				{
////					// Draw green for faces without material
////					MaterialCache::SetDefaultMaterial();
////				}
////			}
////
////			lMeshCache->Draw(lIndex, pShadingMode);
////		}
////		lMeshCache->EndDraw();
////	}
////	else
////	{
////		// OpenGL driver is too lower and use Immediate Mode
////		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
////		const int lPolygonCount = lMesh->GetPolygonCount();
////		for (int lPolygonIndex = 0; lPolygonIndex < lPolygonCount; lPolygonIndex++)
////		{
////			const int lVerticeCount = lMesh->GetPolygonSize(lPolygonIndex);
////			glBegin(GL_LINE_LOOP);
////			for (int lVerticeIndex = 0; lVerticeIndex < lVerticeCount; lVerticeIndex++)
////			{
////				glVertex3dv((GLdouble *)lVertexArray[lMesh->GetPolygonVertex(lPolygonIndex, lVerticeIndex)]);
////			}
////			glEnd();
////		}
////	}
////
////	glPopMatrix();
////
////	delete[] lVertexArray;
////}
//
//void DrawNode(FbxNode* pNode,
//	FbxTime& pTime,
//	FbxAnimLayer* pAnimLayer,
//	FbxAMatrix& pParentGlobalPosition,
//	FbxAMatrix& pGlobalPosition,
//	FbxPose* pPose, ShadingMode pShadingMode);
//
//// Draw recursively each node of the scene. To avoid recomputing 
//// uselessly the global positions, the global position of each 
//// node is passed to it's children while browsing the node tree.
//// If the node is part of the given pose for the current scene,
//// it will be drawn at the position specified in the pose, Otherwise
//// it will be drawn at the given time.
//void DrawNodeRecursive(FbxNode* pNode, FbxTime& pTime, FbxAnimLayer* pAnimLayer,
//	FbxAMatrix& pParentGlobalPosition, FbxPose* pPose,
//	ShadingMode pShadingMode)
//{
//	FbxAMatrix lGlobalPosition = GetGlobalPosition(pNode, pTime, pPose, &pParentGlobalPosition);
//
//	if (pNode->GetNodeAttribute())
//	{
//		// Geometry offset.
//		// it is not inherited by the children.
//		FbxAMatrix lGeometryOffset = GetGeometry(pNode);
//		FbxAMatrix lGlobalOffPosition = lGlobalPosition * lGeometryOffset;
//
//		DrawNode(pNode, pTime, pAnimLayer, pParentGlobalPosition, lGlobalOffPosition, pPose, pShadingMode);
//	}
//
//	const int lChildCount = pNode->GetChildCount();
//	for (int lChildIndex = 0; lChildIndex < lChildCount; ++lChildIndex)
//	{
//		DrawNodeRecursive(pNode->GetChild(lChildIndex), pTime, pAnimLayer, lGlobalPosition, pPose, pShadingMode);
//	}
//}
//
//// Draw the node following the content of it's node attribute.
//void DrawNode(FbxNode* pNode,
//	FbxTime& pTime,
//	FbxAnimLayer* pAnimLayer,
//	FbxAMatrix& pParentGlobalPosition,
//	FbxAMatrix& pGlobalPosition,
//	FbxPose* pPose, ShadingMode pShadingMode)
//{
//	FbxNodeAttribute* lNodeAttribute = pNode->GetNodeAttribute();
//
//	if (lNodeAttribute)
//	{
//		// All lights has been processed before the whole scene because they influence every geometry.
//		if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMarker)
//		{
//			//DrawMarker(pGlobalPosition);
//		}
//		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eSkeleton)
//		{
//			DrawSkeleton(pNode, pParentGlobalPosition, pGlobalPosition);
//		}
//		// NURBS and patch have been converted into triangluation meshes.
//		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh)
//		{
//			//DrawMesh(pNode, pTime, pAnimLayer, pGlobalPosition, pPose, pShadingMode);
//		}
//		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eCamera)
//		{
//			//DrawCamera(pNode, pTime, pAnimLayer, pGlobalPosition);
//		}
//		else if (lNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull)
//		{
//			//DrawNull(pGlobalPosition);
//		}
//	}
//	else
//	{
//		// Draw a Null for nodes without attribute.
//		//DrawNull(pGlobalPosition);
//	}
//}
//
//
//// Bake node attributes and materials for this scene and load the textures.
//void LoadCacheRecursive(FbxScene * pScene, FbxAnimLayer * pAnimLayer, const char * pFbxFileName, bool pSupportVBO)
//{
//	// Load the textures into GPU, only for file texture now
//	const int lTextureCount = pScene->GetTextureCount();
//	for (int lTextureIndex = 0; lTextureIndex < lTextureCount; ++lTextureIndex)
//	{
//		FbxTexture * lTexture = pScene->GetTexture(lTextureIndex);
//		FbxFileTexture * lFileTexture = FbxCast<FbxFileTexture>(lTexture);
//		if (lFileTexture && !lFileTexture->GetUserDataPtr())
//		{
//			// Try to load the texture from absolute path
//			const FbxString lFileName = lFileTexture->GetFileName();
//
//			// Only TGA textures are supported now.
//			if (lFileName.Right(3).Upper() != "TGA")
//			{
//				FBXSDK_printf("Only TGA textures are supported now: %s\n", lFileName.Buffer());
//				continue;
//			}
//
//			//Load texture here
//			//GLuint lTextureObject = 0;
//			//bool lStatus = LoadTextureFromFile(lFileName, lTextureObject);
//
//			const FbxString lAbsFbxFileName = FbxPathUtils::Resolve(pFbxFileName);
//			const FbxString lAbsFolderName = FbxPathUtils::GetFolderName(lAbsFbxFileName);
//			if (!lStatus)
//			{
//				// Load texture from relative file name (relative to FBX file)
//				const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lFileTexture->GetRelativeFileName());
//				lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
//			}
//
//			if (!lStatus)
//			{
//				// Load texture from file name only (relative to FBX file)
//				const FbxString lTextureFileName = FbxPathUtils::GetFileName(lFileName);
//				const FbxString lResolvedFileName = FbxPathUtils::Bind(lAbsFolderName, lTextureFileName);
//				lStatus = LoadTextureFromFile(lResolvedFileName, lTextureObject);
//			}
//
//			if (!lStatus)
//			{
//				FBXSDK_printf("Failed to load texture file: %s\n", lFileName.Buffer());
//				continue;
//			}
//
//			if (lStatus)
//			{
//				GLuint * lTextureName = new GLuint(lTextureObject);
//				lFileTexture->SetUserDataPtr(lTextureName);
//			}
//		}
//	}
//
//	LoadCacheRecursive(pScene->GetRootNode(), pAnimLayer, pSupportVBO);
//}
//
//
//bool LoadFile()
//{
//	LoadScene(filename);
//
//	// Convert Axis System to what is used in this example, if needed
//	FbxAxisSystem SceneAxisSystem = fsce->GetGlobalSettings().GetAxisSystem();
//	FbxAxisSystem OurAxisSystem(FbxAxisSystem::eDirectX);
//	if (SceneAxisSystem != OurAxisSystem)
//	{
//		OurAxisSystem.ConvertScene(fsce);
//	}
//
//
//	//For some reason this throws errors
//	// Convert Unit System to what is used in this example, if needed
//	//FbxSystemUnit SceneSystemUnit = fsce->GetGlobalSettings().GetSystemUnit();
//	//if (SceneSystemUnit.GetScaleFactor() != 1.0)
//	//{
//	//	//The unit in this example is centimeter.
//	//	FbxSystemUnit::cm.ConvertScene(fsce);
//	//}
//
//	// Get the list of all the animation stack.
//	fsce->FillAnimStackNameArray(mAnimStackNameArray);
//
//	// Get the list of all the cameras in the scene.
//	FillCameraArray(fsce, mCameraArray);
//
//	//Assume Triangulated
//	//// Convert mesh, NURBS and patch into triangle mesh
//	//FbxGeometryConverter lGeomConverter(fman);
//	//lGeomConverter.Triangulate(fsce, /*replace*/true);
//
//	// Bake the scene for one frame
//	//LoadCacheRecursive(fsce, mCurrentAnimLayer, mFileName, false);
//
//	// Convert any .PC2 point cache data into the .MC format for 
//	// vertex cache deformer playback.
//	//PreparePointCacheData(fsce, mCache_Start, mCache_Stop);
//
//	// Get the list of pose in the scene
//	FillPoseArray(fsce, mPoseArray);
//
//	// Initialize the window message.
//	//mWindowMessage = "File ";
//	//mWindowMessage += mFileName;
//	//mWindowMessage += "\nClick on the right mouse button to enter menu.";
//	//mWindowMessage += "\nEsc to exit.";
//
//	// Initialize the frame period.
//	//mFrameTime.SetTime(0, 0, 0, 1, 0, fsce->GetGlobalSettings().GetTimeMode());
//
//	return true;
//}
//
//void UpdateCamera(float const moveSpd, float const rotSpd, float delta_time = 1.0f)
//{
//	//W
//	if (GetAsyncKeyState(0x57))
//	{
//		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, moveSpd * delta_time);
//		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
//		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
//		XMStoreFloat4x4(&m_camera, result);
//	}
//	//S
//	if (GetAsyncKeyState(0x53))
//	{
//		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, 0.0f, -moveSpd * delta_time);
//		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
//		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
//		XMStoreFloat4x4(&m_camera, result);
//	}
//	//A
//	if (GetAsyncKeyState(0x41))
//	{
//		XMMATRIX translation = DirectX::XMMatrixTranslation(moveSpd * delta_time, 0.0f, 0.0f);
//		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
//		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
//		XMStoreFloat4x4(&m_camera, result);
//	}
//	//D
//	if (GetAsyncKeyState(0x44))
//	{
//		XMMATRIX translation = DirectX::XMMatrixTranslation(-moveSpd * delta_time, 0.0f, 0.0f);
//		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
//		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
//		XMStoreFloat4x4(&m_camera, result);
//	}
//	//X
//	if (GetAsyncKeyState(0x58))
//	{
//		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, moveSpd * delta_time, 0.0f);
//		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
//		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
//		XMStoreFloat4x4(&m_camera, result);
//	}
//	//Space
//	if (GetAsyncKeyState(0x20))
//	{
//		XMMATRIX translation = DirectX::XMMatrixTranslation(0.0f, -moveSpd * delta_time, 0.0f);
//		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
//		XMMATRIX result = XMMatrixMultiply(translation, temp_camera);
//		XMStoreFloat4x4(&m_camera, result);
//	}
//	//Right Mouse Button
//	if (GetAsyncKeyState(VK_RBUTTON))
//	{
//		POINT mousePos;
//		GetCursorPos(&mousePos);
//		SetCursorPos(WIDTH_P / 2, HEIGHT_P / 2);
//		float dx = (float)WIDTH_P / 2 - mousePos.x;
//		float dy = (float)HEIGHT_P / 2 - mousePos.y;
//
//		DirectX::XMFLOAT4 pos = DirectX::XMFLOAT4(m_camera._41, m_camera._42, m_camera._43, m_camera._44);
//
//		m_camera._41 = m_camera._42 = m_camera._43 = 0.0f;
//
//		XMMATRIX rotX = DirectX::XMMatrixRotationX(dy * rotSpd * delta_time);
//		XMMATRIX rotY = DirectX::XMMatrixRotationY(dx * rotSpd * delta_time);
//
//		XMMATRIX temp_camera = XMLoadFloat4x4(&m_camera);
//		temp_camera = XMMatrixMultiply(rotX, temp_camera);
//		temp_camera = XMMatrixMultiply(temp_camera, rotY);
//
//		XMStoreFloat4x4(&m_camera, temp_camera);
//
//		m_camera._41 = pos.x;
//		m_camera._42 = pos.y;
//		m_camera._43 = pos.z;
//	}
//
//	//1 Key
//	if (GetAsyncKeyState(0x31) & 1)
//	{
//		wireFram = !wireFram;
//	}
//	//2 Key
//	if (GetAsyncKeyState(0x32) & 1)
//	{
//		planeFram = !planeFram;
//	}
//}

//void DrawBones(std::vector<MyMesh> Sphere_, float offset_[3])
//{
//	std::vector<VERTEX> vertextlist;
//
//	for (unsigned i = 0; i < Sphere_.size(); i++)
//	{
//		VERTEX Temp;
//		Temp.Color = { 0.5f,0.5f,0.0f,1.0f };
//		Temp.X = Sphere_[i].position[0] + offset_[0];
//		Temp.Y = Sphere_[i].position[1] + offset_[1];
//		Temp.Z = Sphere_[i].position[2] + offset_[2];
//		vertextlist.push_back(Temp);
//	}
//
//	ID3D11Buffer *AniBuffer_;
//	D3D11_BUFFER_DESC AniBuffDesc;
//	ZeroMemory(&AniBuffDesc, sizeof(AniBuffDesc));
//	AniBuffDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
//	AniBuffDesc.ByteWidth = sizeof(VERTEX) * (unsigned)vertextlist.size(); // size is the VERTEX struct
//	AniBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
//	AniBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
//	D3D11_SUBRESOURCE_DATA data;
//	data.pSysMem = &vertextlist[0];
//	Device->CreateBuffer(&AniBuffDesc, &data, &AniBuffer_);       // create the buffer
//	UINT stride = sizeof(VERTEX);
//	UINT offset = 0;
//	Devicecon->IASetVertexBuffers(0, 1, &AniBuffer_, &stride, &offset);
//
//
//	ID3D11RasterizerState *AniRaster;
//	D3D11_RASTERIZER_DESC AniRasDesc;
//	ZeroMemory(&AniRasDesc, sizeof(AniRasDesc));
//	AniRasDesc.CullMode = D3D11_CULL_NONE;
//	AniRasDesc.FillMode = D3D11_FILL_WIREFRAME;
//	Device->CreateRasterizerState(&AniRasDesc, &AniRaster);
//	Devicecon->RSSetState(AniRaster);
//	Devicecon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	Devicecon->Draw((unsigned)vertextlist.size(), 0);
//
//	AniBuffer_->Release();
//	AniRaster->Release();
//
//	Devicecon->RSSetState(RasterState);
//	stride = sizeof(VERTEX);
//	Devicecon->IASetVertexBuffers(0, 1, &_Buffer, &stride, &offset);
//	Devicecon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//}
//void DrawSpheresForbones(std::vector<Bone> vec, std::vector<MyMesh> Sphere_)
//{
//	std::vector<VERTEX> vertextlist;
//
//	for (unsigned i = 0; i < vec.size(); i += 2)
//	{
//		VERTEX Temp;
//		Temp.Color = { 0.0f,5.0f,5.0f,1.0f };
//		Temp.X = (vec[i].pos[0] + vec[i + 1].pos[0]) / 2.0f;
//		Temp.Y = (vec[i].pos[1] + vec[i + 1].pos[1]) / 2.0f;
//		Temp.Z = (vec[i].pos[2] + vec[i + 1].pos[2]) / 2.0f;
//		vertextlist.push_back(Temp);
//	}
//
//	float ver1[3] = { vertextlist[0].X,vertextlist[0].Y,vertextlist[0].Z };
//	float ver2[3] = { vertextlist[1].X,vertextlist[1].Y,vertextlist[1].Z };
//	float ver3[3] = { vertextlist[2].X,vertextlist[2].Y,vertextlist[2].Z };
//
//	DrawBones(Sphere_, ver1);
//	DrawBones(Sphere_, ver2);
//	DrawBones(Sphere_, ver3);
//	//
//}



//void AnimateVector(bool wireframe, std::vector<MyMesh> vec)
//{
//	std::vector<VERTEX> vertextlist;
//
//	for (unsigned i = 0; i < vec.size(); i++)
//	{
//		VERTEX Temp;
//		Temp.Color = { 1.0f,0.0f,0.0f,1.0f };
//		Temp.X = vec[i].position[0];
//		Temp.Y = vec[i].position[1];
//		Temp.Z = vec[i].position[2];
//		vertextlist.push_back(Temp);
//	}
//
//	ID3D11Buffer *AniBuffer_;
//	D3D11_BUFFER_DESC AniBuffDesc;
//	ZeroMemory(&AniBuffDesc, sizeof(AniBuffDesc));
//	AniBuffDesc.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
//	AniBuffDesc.ByteWidth = sizeof(VERTEX) * (unsigned)vertextlist.size(); // size is the VERTEX struct * 3
//	AniBuffDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
//	AniBuffDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
//	D3D11_SUBRESOURCE_DATA data;
//	data.pSysMem = &vertextlist[0];
//	Device->CreateBuffer(&AniBuffDesc, &data, &AniBuffer_);       // create the buffer
//	UINT stride = sizeof(VERTEX);
//	UINT offset = 0;
//	Devicecon->IASetVertexBuffers(0, 1, &AniBuffer_, &stride, &offset);
//
//	ID3D11RasterizerState *AniRaster;
//	D3D11_RASTERIZER_DESC AniRasDesc;
//	ZeroMemory(&AniRasDesc, sizeof(AniRasDesc));
//	AniRasDesc.CullMode = D3D11_CULL_NONE;
//	if (wireframe == true)
//		AniRasDesc.FillMode = D3D11_FILL_WIREFRAME;
//	else
//		AniRasDesc.FillMode = D3D11_FILL_SOLID;
//	Device->CreateRasterizerState(&AniRasDesc, &AniRaster);
//	Devicecon->RSSetState(AniRaster);
//	Devicecon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	Devicecon->Draw((unsigned)vec.size(), 0);
//	//change the raster state back to its orignal
//
//	AniBuffer_->Release();
//	AniRaster->Release();
//
//	Devicecon->RSSetState(RasterState);
//	stride = sizeof(VERTEX);
//	Devicecon->IASetVertexBuffers(0, 1, &_Buffer, &stride, &offset);
//	Devicecon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//
//}
//#pragma region DirectX_Init
//
//void CleanD3D(void)
//{
//	swapchain->SetFullscreenState(FALSE, NULL);
//	// close and release all existing COM objects
//	swapchain->Release();
//	backbuffer->Release();
//	Device->Release();
//	Devicecon->Release();
//
//	_VertexShader->Release();
//	_PixelShader->Release();
//	RasterState->Release();
//
//}
//
//D3D11_BUFFER_DESC ConstantBuffer;
//D3D11_MAPPED_SUBRESOURCE ConsREsorce;
//
//void RenderFrame(bool wireframe, std::vector<MyMesh> vec, std::vector<Bone> bones, std::vector<MyMesh> spehre_)
//{
//	Pro_View_World MAtrices;
//	MAtrices.World = _WorldMatrix;
//	MAtrices.Pro = _ProjectionMatrix;
//	MAtrices.View = m_camera;
//
//	Devicecon->Map(_ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ConsREsorce);    // map the buffer
//	memcpy(ConsREsorce.pData, &MAtrices, sizeof(Pro_View_World));      // copy the data
//	Devicecon->Unmap(_ConstantBuffer, NULL);                                      // unmap the buffer
//
//	/////////////////////
//	// clear the back buffer to a deep blue
//	FLOAT ColorScreen[4] = { 0.0f,0.2f,0.4f,1.0f };
//	Devicecon->ClearRenderTargetView(backbuffer, ColorScreen);
//	///////////////////////////////////////////////////////////////////
//
//	// Prepare the constant buffer to send it to the graphics device.
//	//Devicecon->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
//	///////////////////////////////////////////////////////////////////
//
//	Devicecon->IASetIndexBuffer(_IndexBuffer, DXGI_FORMAT_R32G32B32_UINT, 0);
//	Devicecon->RSSetState(RasterState);
//	UINT stride = sizeof(VERTEX);
//	UINT offset = 0;
//	Devicecon->IASetVertexBuffers(0, 1, &_Buffer, &stride, &offset);
//	Devicecon->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//	Devicecon->VSSetConstantBuffers(0, 1, &_ConstantBuffer);
//
//
//	ID3D11RasterizerState *AniRaster;
//	D3D11_RASTERIZER_DESC AniRasDesc;
//	ZeroMemory(&AniRasDesc, sizeof(AniRasDesc));
//	AniRasDesc.CullMode = D3D11_CULL_NONE;
//	if (planeFram == true)
//		AniRasDesc.FillMode = D3D11_FILL_WIREFRAME;
//	else
//		AniRasDesc.FillMode = D3D11_FILL_SOLID;
//	Device->CreateRasterizerState(&AniRasDesc, &AniRaster);
//	Devicecon->RSSetState(AniRaster);
//
//
//	Devicecon->DrawIndexed(6, 0, 0);
//	/////
//	AnimateVector(wireframe, vec);
//	// add spehre
//	DrawSpheresForbones(bones, spehre_);
//	///////////////////////////////////////////////////////////////////
//	// switch the back buffer and the front buffer
//
//	swapchain->Present(0, 0);
//	//////////////////////////////////////
//	AniRaster->Release();
//
//}
//
//
//void InitPipeline()
//{
//	Device->CreateVertexShader(&(Vshader), ARRAYSIZE((Vshader)), NULL, &_VertexShader);
//	Device->CreatePixelShader(&(Pshader), ARRAYSIZE((Pshader)), NULL, &_PixelShader);
//
//	Devicecon->VSSetShader(_VertexShader, 0, 0);
//	Devicecon->PSSetShader(_PixelShader, 0, 0);
//
//	SetElement(0);
//
//
//
//}
//
//void InitGraphics()
//{
//	VERTEX Triangle[] =
//	{
//		{ 0.0f, 0.0f, 5.0f,	{1.0f, 0.0f, 0.0f, 1.0f}}, //bottom left
//		{ 5.0f, 0.0f, 5.0f, {1.0f, 0.0f, 0.0f, 1.0f}}, // bottom right
//		{ 5.0f, 0.0f, 0.0f, {1.0f, 0.0f, 0.0f, 1.0f}},// top right
//		{ 0.0f, 0.0f, 0.0f, {0.0f, 1.0f, 0.0f, 1.0f}} // top left
//	};
//
//
//	//Buffer
//	D3D11_BUFFER_DESC BufferDes;
//	ZeroMemory(&BufferDes, sizeof(BufferDes));
//
//	BufferDes.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
//	BufferDes.ByteWidth = sizeof(VERTEX) * ARRAYSIZE(Triangle);             // size is the VERTEX struct * 3
//	BufferDes.BindFlags = D3D11_BIND_VERTEX_BUFFER;       // use as a vertex buffer
//	BufferDes.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
//
//	D3D11_SUBRESOURCE_DATA data;
//	data.pSysMem = &Triangle[0];
//	Device->CreateBuffer(&BufferDes, &data, &_Buffer);       // create the buffer
//
//	Pro_View_World MAtrices;
//	MAtrices.World = _WorldMatrix;
//	MAtrices.Pro = _ProjectionMatrix;
//	MAtrices.View = _ViewMatrix;
//
//	//////////////////////////////////////////////
//	ZeroMemory(&ConstantBuffer, sizeof(ConstantBuffer));
//	ConstantBuffer.Usage = D3D11_USAGE_DYNAMIC;                // write access access by CPU and GPU
//	ConstantBuffer.ByteWidth = sizeof(Pro_View_World);             // size is pro_view_world
//	ConstantBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;       // use as a vertex buffer
//	ConstantBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;    // allow CPU to write in buffer
//	Device->CreateBuffer(&ConstantBuffer, NULL, &_ConstantBuffer);       // create the buffer
//	Devicecon->Map(_ConstantBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ConsREsorce);    // map the buffer
//	memcpy(ConsREsorce.pData, &MAtrices, sizeof(Pro_View_World));      // copy the data
//	Devicecon->Unmap(_ConstantBuffer, NULL);                                      // unmap the buffer
//	//////////////////////////////////////////////
//
//
//
//
//	D3D11_RASTERIZER_DESC RasDesc;
//	ZeroMemory(&RasDesc, sizeof(RasDesc));
//	RasDesc.CullMode = D3D11_CULL_NONE;
//	RasDesc.FillMode = D3D11_FILL_SOLID;
//	Device->CreateRasterizerState(&RasDesc, &RasterState);
//
//#pragma region IndexBuffer
//
//
//	unsigned indexbuffer[6] = { 0,1,2,0,2,3 };
//	D3D11_BUFFER_DESC IndexDesc;
//	ZeroMemory(&IndexDesc, sizeof(IndexDesc));
//
//	IndexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
//	IndexDesc.Usage = D3D11_USAGE_DYNAMIC;
//	IndexDesc.ByteWidth = sizeof(indexbuffer);
//	IndexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//	Device->CreateBuffer(&IndexDesc, NULL, &_IndexBuffer);
//
//	D3D11_MAPPED_SUBRESOURCE IndexResource;
//	Devicecon->Map(_IndexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &IndexResource);
//	memcpy(IndexResource.pData, &indexbuffer, sizeof(indexbuffer));
//	Devicecon->Unmap(_IndexBuffer, NULL);
//
//#pragma endregion
//
//
//}
//
#pragma endregion