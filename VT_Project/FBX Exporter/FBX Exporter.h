#pragma once
#include "stdafx.h"
#include <vector>
#include <DirectXMath.h>
#include "fbxsdk.h"

//Exporter/loader I tried writing with base code from the fbxsdk manual and http://www.walkerb.net/blog/dx-4/
//Incomplete, only loads vertexes, but has very good comments. 
//HRESULT LoadFBX(const char* filename, std::vector<DirectX::XMFLOAT4>* pOutVertexVector);

extern __declspec(dllexport) bool LoadScene(fbxsdk::FbxManager* pManager, fbxsdk::FbxDocument* pScene, const char* pFilename);