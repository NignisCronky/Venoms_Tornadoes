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
	//position
	float pos[4];
	//rotation
	float rot[4];
	//scale
	float sca[4];
	Bone()
	{

	}
	Bone(float boner[4], float rottenboner[4], float scalyboner[4])
	{
		pos[0] = boner[0];
		pos[1] = boner[1];
		pos[2] = boner[2];
		pos[3] = boner[3];
		rot[0] = rottenboner[0];
		rot[1] = rottenboner[1];
		rot[2] = rottenboner[2];
		rot[3] = rottenboner[3];
		sca[0] = scalyboner[0];
		sca[1] = scalyboner[1];
		sca[2] = scalyboner[2];
		sca[3] = scalyboner[3];

	}
	Bone(float boner0, float boner1, float boner2, float boner3, float rottenboner0, float rottenboner1, float rottenboner2, float rottenboner3, float scalyboner0, float scalyboner1, float scalyboner2, float scalyboner3)
	{
		pos[0] = boner0;
		pos[1] = boner1;
		pos[2] = boner2;
		pos[3] = boner3;
		rot[0] = rottenboner0;
		rot[1] = rottenboner1;
		rot[2] = rottenboner2;
		rot[3] = rottenboner3;
		sca[0] = scalyboner0;
		sca[1] = scalyboner1;
		sca[2] = scalyboner2;
		sca[3] = scalyboner3;
	}
};

static std::vector<Bone> null_fill;

extern __declspec(dllexport)void LoadScene(const char* pFilename, std::vector<MyMesh> &mesh, std::vector<Bone> &boner = null_fill);
