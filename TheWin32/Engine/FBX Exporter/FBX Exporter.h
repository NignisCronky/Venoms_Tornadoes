#pragma once
#include "stdafx.h"
#include <vector>

struct MyMesh
{
	float position[4];
	float normals[4];
	float uv[4];

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

struct Bone
{
	float bone[4];
	Bone()
	{

	}
	Bone(float boner[4])
	{
		bone[0] = boner[0];
		bone[1] = boner[1];
		bone[2] = boner[2];
		bone[3] = boner[3];
	}
	Bone(float boner0, float boner1, float boner2, float boner3)
	{
		bone[0] = boner0;
		bone[1] = boner1;
		bone[2] = boner2;
		bone[3] = boner3;
	}
};

static std::vector<Bone> null_fill;

extern __declspec(dllexport)void LoadScene(const char* pFilename, std::vector<MyMesh> &mesh, std::vector<Bone> &boner = null_fill);
