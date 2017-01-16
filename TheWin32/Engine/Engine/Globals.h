#pragma once

#include <Windows.h>
//#include <d3d11.h> 
//#include <D3DX11.h>
#include "Vshader.csh"
#include "Pshader.csh"
#include "_VertShader.csh"
#include "_PixShader.csh"
#include "_LightShader.csh"
#include "NormalVertexShader.csh"
#include "MaskPixelShader.csh"
//#include <d3d11_1.h>
//#include <d3dcompiler.h>
#include "../FBX Exporter/FBX Exporter.h"
#include <vector>
#include "vec3.h"
#include "Timer.h"
#include "DDSTextureLoader.h"
#include <directxcolors.h>

#pragma comment (lib, "d3d11.lib")



#define WIDTH_W  1024
#define HEIGHT_W 768

#define WIDTH_P  1024
#define HEIGHT_P 768




struct SmartVert {
	float pos[3];
	float norms[3];
	float uv[2];
	float blendWeights[4];
	unsigned boneIndices[4];
};

struct Pro_View_World
{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 Pro;
	DirectX::XMFLOAT4X4 View;
};


