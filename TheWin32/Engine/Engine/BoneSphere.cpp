#include "BoneSphere.h"



void BoneSphere::Create(Pro_View_World Mat, ID3D11DeviceContext * Context, ID3D11Device * Device, std::vector<SmartVert> verts)
{
	PVW.Pro = Mat.Pro;
	PVW.View = Mat.View;
	PVW.World = Mat.World;








}

void BoneSphere::set(ID3D11DeviceContext * Context)
{
}

void BoneSphere::Update(float x, float y, float z)
{
}

BoneSphere::BoneSphere()
{
}


BoneSphere::~BoneSphere()
{
}
