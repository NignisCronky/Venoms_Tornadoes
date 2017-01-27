#pragma once
#include "Structs.h"

extern __declspec(dllexport)void FBXtoBinary(const char* fbxfile, const char* binfile, bool overwrite = true);

extern __declspec(dllexport)bool ReadBinary(const char* loadfile, Skeleton* skelly, std::vector<unsigned int>* indicies, std::vector<PNTIWVertex>* verts);

static std::vector<Bone> null_fill;
extern __declspec(dllexport)void LoadScene(const char* pFilename, std::vector<MyMesh> &mesh, std::vector<Bone> &boner = null_fill);

