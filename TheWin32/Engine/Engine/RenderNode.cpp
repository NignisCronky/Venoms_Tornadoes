#include "RenderNode.h"



void RenderNode::SetLayout(unsigned index)
{
}

void RenderNode::render()
{
}

void RenderNode::renderWireFrame()
{
}

RenderNode::RenderNode(std::vector<float[3]> Points, std::vector<float[4]> Color, ID3D11Device * Dev)
{

}

RenderNode::RenderNode(std::vector<float[3]> Points, float Color[4], ID3D11Device * Dev)
{
}

RenderNode::RenderNode(std::vector<float[3]> Points, std::vector<float[3]> Norms, std::vector<float[2]> UVs, ID3D11Device * Dev)
{
}

RenderNode::RenderNode()
{
	DirectX::XMStoreFloat4x4(nullptr, DirectX::XMMatrixIdentity());
}


RenderNode::~RenderNode()
{
}
