#pragma once
#include "stdafx.h"
#include <vector>

//Exporter/loader I tried writing with base code from the fbxsdk manual and http://www.walkerb.net/blog/dx-4/
//Incomplete, only loads vertexes, but has very good comments. 
//HRESULT LoadFBX(const char* filename, std::vector<DirectX::XMFLOAT4>* pOutVertexVector);

struct MyMesh
{
	float position[4];
	float normals[4];
	float uv[3];

	MyMesh()
	{

	}
	MyMesh(float pos[4], float norm[4], float uvs[4])
	{
		position[0] = pos[0];
		position[1] = pos[1];
		position[2] = pos[2];
		position[3] = pos[3];
		normals[0] = norm[0];
		normals[1] = norm[1];
		normals[2] = norm[2];
		normals[3] = norm[3];
		uv[0] = uvs[0];
		uv[1] = uvs[1];
		uv[2] = uvs[2];
		uv[3] = uvs[3];
	}
};

extern __declspec(dllexport) std::vector<MyMesh> LoadScene(const char* pFilename);
