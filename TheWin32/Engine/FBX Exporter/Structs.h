#pragma once
#include <SDKDDKVer.h>
#include <DirectXMath.h>
#include <vector>
#include <algorithm>

using namespace DirectX;
const XMFLOAT2 vector2Epsilon = XMFLOAT2(0.00001f, 0.00001f);
const XMFLOAT3 vector3Epsilon = XMFLOAT3(0.00001f, 0.00001f, 0.00001f);
bool CompareVector3WithEpsilon(const XMFLOAT3& lhs, const XMFLOAT3& rhs)
{
	return XMVector3NearEqual(XMLoadFloat3(&lhs), XMLoadFloat3(&rhs), XMLoadFloat3(&vector3Epsilon)) == true;
}

bool CompareVector2WithEpsilon(const XMFLOAT2& lhs, const XMFLOAT2& rhs)
{
	return XMVector3NearEqual(XMLoadFloat2(&lhs), XMLoadFloat2(&rhs), XMLoadFloat2(&vector2Epsilon)) == true;
}


struct PNTVertex
{
	XMFLOAT3 mPosition;
	XMFLOAT3 mNormal;
	XMFLOAT2 mUV;

	bool operator==(const PNTVertex& rhs) const
	{
		uint32_t position;
		uint32_t normal;
		uint32_t uv;

		XMVectorEqualR(&position, XMLoadFloat3(&(this->mPosition)), XMLoadFloat3(&rhs.mPosition));
		XMVectorEqualR(&normal, XMLoadFloat3(&(this->mNormal)), XMLoadFloat3(&rhs.mNormal));
		XMVectorEqualR(&uv, XMLoadFloat2(&(this->mUV)), XMLoadFloat2(&rhs.mUV));

		return XMComparisonAllTrue(position) && XMComparisonAllTrue(normal) && XMComparisonAllTrue(uv);
	}
};

struct VertexBlendingInfo
{
	unsigned int mBlendingIndex;
	float mBlendingWeight;

	VertexBlendingInfo() :
		mBlendingIndex(0),
		mBlendingWeight(0.0)
	{}

	bool operator < (const VertexBlendingInfo& rhs)
	{
		return (mBlendingWeight > rhs.mBlendingWeight);
	}
};

struct PNTIWVertex
{
	XMFLOAT3 mPosition;
	XMFLOAT3 mNormal;
	XMFLOAT2 mUV;
	std::vector<VertexBlendingInfo> mVertexBlendingInfos;

	void SortBlendingInfoByWeight()
	{
		std::sort(mVertexBlendingInfos.begin(), mVertexBlendingInfos.end());
	}

	bool operator==(const PNTIWVertex& rhs) const
	{
		bool sameBlendingInfo = true;

		// We only compare the blending info when there is blending info
		if (!(mVertexBlendingInfos.empty() && rhs.mVertexBlendingInfos.empty()))
		{
			// Each vertex should only have 4 index-weight blending info pairs
			for (unsigned int i = 0; i < 4; ++i)
			{
				if (mVertexBlendingInfos[i].mBlendingIndex != rhs.mVertexBlendingInfos[i].mBlendingIndex ||
					abs(mVertexBlendingInfos[i].mBlendingWeight - rhs.mVertexBlendingInfos[i].mBlendingWeight) > 0.001)
				{
					sameBlendingInfo = false;
					break;
				}
			}
		}

		bool result1 = CompareVector3WithEpsilon(mPosition, rhs.mPosition);
		bool result2 = CompareVector3WithEpsilon(mNormal, rhs.mNormal);
		bool result3 = CompareVector2WithEpsilon(mUV, rhs.mUV);

		return result1 && result2 && result3 && sameBlendingInfo;
	}
};

// Each Control Point in FBX is basically a vertex
// in the physical world. For example, a cube has 8
// vertices(Control Points) in FBX
// Joints are associated with Control Points in FBX
// The mapping is one joint corresponding to 4
// Control Points(Reverse of what is done in a game engine)
// As a result, this struct stores a XMFLOAT3 and a 
// vector of joint indices
struct CtrlPoint
{
	XMFLOAT3 mPosition;
	std::vector<VertexBlendingInfo> mBlendingInfo;

	CtrlPoint()
	{
		mBlendingInfo.reserve(4);
	}
};

// This stores the information of each key frame of each joint
// This is a linked list and each node is a snapshot of the
// global transformation of the joint at a certain frame
struct Keyframe
{
	long long mFrameNum;
	XMFLOAT3 translation;
	XMFLOAT4 rotation;
	//XMFLOAT3 scale;

	Keyframe()
	{
	}

	bool operator==(const Keyframe& a) const
	{
		return
			translation.x == a.translation.x &&
			translation.y == a.translation.y &&
			translation.z == a.translation.z &&
			rotation.x == a.rotation.x &&
			rotation.y == a.rotation.y &&
			rotation.z == a.rotation.z &&
			rotation.w == a.rotation.w;
	}
};

// This is the actual representation of a joint in a game engine
struct Joint
{
	std::string mName;
	int mParentIndex;
	XMFLOAT3 translation;
	XMFLOAT4 rotation;
	//XMFLOAT3 scale;
	std::vector<Keyframe> mAnimation;

	Joint()
	{
		translation = XMFLOAT3(1.0f, 1.0f, 1.0f);
		rotation = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		mParentIndex = -1;
	}

	~Joint()
	{
	}
};

struct Skeleton
{
	std::vector<Joint> mJoints;
	long long mAnimationLength;
	std::string mAnimationName;
};