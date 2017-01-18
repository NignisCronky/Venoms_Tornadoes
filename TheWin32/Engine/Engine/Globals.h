#pragma once
#include <Windows.h>
#include "Vshader.csh"
#include "Pshader.csh"
#include "_VertShader.csh"
#include "_PixShader.csh"
#include "_LightShader.csh"
#include "NormalVertexShader.csh"
#include "MaskPixelShader.csh"
#include "../FBX Exporter/FBX Exporter.h"
#include <vector>
#include "Timer.h"
#include "DDSTextureLoader.h"
#include <directxcolors.h>

#pragma comment (lib, "d3d11.lib")

#define horizontalFOV 100
#define PI 3.141592653f
#define width 1000
#define height 500
#define total (width * height)
#define aspectratio (float(width) / float(height))
#define verticalFOV (float(horizontalFOV) * float(aspectratio))
#define zNear .1f
#define zFar 100.0f

struct Pro_View_World
{
	DirectX::XMFLOAT4X4 World;
	DirectX::XMFLOAT4X4 Pro;
	DirectX::XMFLOAT4X4 View;
};