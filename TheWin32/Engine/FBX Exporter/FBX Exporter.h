#pragma once
#include "Structs.h"

extern __declspec(dllexport)void FBXtoBinary(const char* loadfile, const char* savefile, bool overwrite = true);

extern __declspec(dllexport)bool ReadBinary(const char* loadfile, Skeleton* skelly, std::vector<unsigned int>* indicies, std::vector<PNTIWVertex>* verts);