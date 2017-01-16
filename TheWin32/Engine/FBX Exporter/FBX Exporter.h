#pragma once
#include "Structs.h"
#include <unordered_map>

//static std::vector<Bone> null_fill;
std::unordered_map<unsigned int, CtrlPoint*> mControlPoints;

//extern __declspec(dllexport)void LoadScene(const char* pFilename, std::vector<MyMesh> &mesh, std::vector<Bone> &boner = null_fill);
