#pragma once
#include <SDKDDKVer.h>
#include <DirectXMath.h>
#include <vector>
#include <algorithm>

using namespace DirectX;
const XMFLOAT2 vector2Epsilon = XMFLOAT2(0.00001f, 0.00001f);
const XMFLOAT3 vector3Epsilon = XMFLOAT3(0.00001f, 0.00001f, 0.00001f);
bool CompareVector3WithEpsilon(const XMFLOAT3& lhs, const XMFLOAT3& rhs);
bool CompareVector2WithEpsilon(const XMFLOAT2& lhs, const XMFLOAT2& rhs);

struct VertexBlendingInfo
{
	float mBlendingWeight;
	unsigned int mBlendingIndex;

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
	float mBlendingWeight[4];
	unsigned int mBlendingIndex[4];

	PNTIWVertex()
	{
		for (size_t i = 0; i < 4; i++)
		{
			mBlendingWeight[i] = 0.0f;
			mBlendingIndex[i] = 0;
		}
	}

	bool operator==(const PNTIWVertex& rhs) const
	{
		bool sameBlendingInfo = true;
		// Each vertex should only have 4 index-weight blending info pairs
		for (unsigned int i = 0; i < 4; ++i)
		{
			if (mBlendingWeight[i] != rhs.mBlendingWeight[i] ||
				fabsf(float(mBlendingIndex[i] - rhs.mBlendingIndex[i])) > 0.001f)
			{
				sameBlendingInfo = false;
				break;
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
		mBlendingInfo.reserve(5);
	}

	void SortBlendingInfoByWeight()
	{
		std::sort(mBlendingInfo.begin(), mBlendingInfo.end());
	}
};

// This stores the information of each key frame of each joint
// This is a linked list and each node is a snapshot of the
// global transformation of the joint at a certain frame
struct Keyframe
{
	long long mFrameNum;
	XMFLOAT4X4 mGlobalTransform;

	Keyframe()
	{
	}

	bool operator==(const Keyframe& a) const
	{
		return (
			mGlobalTransform._11 == a.mGlobalTransform._11 &&
			mGlobalTransform._12 == a.mGlobalTransform._12 &&
			mGlobalTransform._13 == a.mGlobalTransform._13 &&
			mGlobalTransform._14 == a.mGlobalTransform._14 &&
			mGlobalTransform._21 == a.mGlobalTransform._21 &&
			mGlobalTransform._22 == a.mGlobalTransform._22 &&
			mGlobalTransform._23 == a.mGlobalTransform._23 &&
			mGlobalTransform._24 == a.mGlobalTransform._24 &&
			mGlobalTransform._31 == a.mGlobalTransform._31 &&
			mGlobalTransform._32 == a.mGlobalTransform._32 &&
			mGlobalTransform._33 == a.mGlobalTransform._33 &&
			mGlobalTransform._34 == a.mGlobalTransform._34 &&
			mGlobalTransform._41 == a.mGlobalTransform._41 &&
			mGlobalTransform._42 == a.mGlobalTransform._42 &&
			mGlobalTransform._43 == a.mGlobalTransform._43 &&
			mGlobalTransform._44 == a.mGlobalTransform._44);
	}
};

// This is the actual representation of a joint in a game engine
struct Joint
{
	std::string mName;
	int mParentIndex;
	XMFLOAT4X4 globalBindposeInverseMatrix;
	//XMFLOAT3 scale;
	std::vector<Keyframe> mAnimation;

	Joint()
	{
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

struct BoneContainer
{
	unsigned NumOFBones;
	std::vector<XMFLOAT4X4> Bones;
	BoneContainer()
	{

	}
	BoneContainer(unsigned n, std::vector<XMFLOAT4X4> B)
	{
		NumOFBones = n;
		Bones = B;
	}
};

// model matrix, view matrix, projection, light diretion, light color, matrix offset
struct MVPDCB
{
	XMFLOAT4X4 World;
	XMFLOAT4X4 View;
	XMFLOAT4X4 Pro;
	XMFLOAT4 LightDirection;
	XMFLOAT4 LightColor;
	XMFLOAT4X4 Boneoffsets[100];

	void UpdateDir(XMFLOAT4 LightDirection_)
	{
		LightDirection = LightDirection_;
	}
	void UpdateColor(XMFLOAT4 LightColor_)
	{
		LightColor = LightColor_;
	}
	void UpdateView(XMFLOAT4X4 View_)
	{
		View = View_;
	}

};

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