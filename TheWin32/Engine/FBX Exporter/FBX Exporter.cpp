// FBX Exporter.cpp : Defines the exported functions for the DLL application.
#pragma once
#include <fbxsdk.h>
#include <fbxsdk\fileio\fbxiosettings.h>
#include "FBX Exporter.h"
#include <unordered_map>
#include <fstream>

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

std::unordered_map<unsigned int, CtrlPoint> mControlPoints;

using namespace DirectX;

XMFLOAT3 Vec4ToFloat3(FbxVector4 vec)
{
	return XMFLOAT3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}
XMFLOAT4 Vec4ToFloat4(FbxVector4 vec)
{
	return XMFLOAT4((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], (float)vec.mData[3]);
}

static XMFLOAT4X4 ToXm(const FbxAMatrix& pSrc)
{
	return XMFLOAT4X4(
		static_cast<float>(pSrc.Get(0, 0)), static_cast<float>(pSrc.Get(0, 1)), static_cast<float>(pSrc.Get(0, 2)), static_cast<float>(pSrc.Get(0, 3)),
		static_cast<float>(pSrc.Get(1, 0)), static_cast<float>(pSrc.Get(1, 1)), static_cast<float>(pSrc.Get(1, 2)), static_cast<float>(pSrc.Get(1, 3)),
		static_cast<float>(pSrc.Get(2, 0)), static_cast<float>(pSrc.Get(2, 1)), static_cast<float>(pSrc.Get(2, 2)), static_cast<float>(pSrc.Get(2, 3)),
		static_cast<float>(pSrc.Get(3, 0)), static_cast<float>(pSrc.Get(3, 1)), static_cast<float>(pSrc.Get(3, 2)), static_cast<float>(pSrc.Get(3, 3)));
}

static XMFLOAT4X4 MultiplyFloat(XMFLOAT4X4 a, float b)
{
	return XMFLOAT4X4(
		a._11 * b,
		a._12 * b,
		a._13 * b,
		a._14 * b,
		a._21 * b,
		a._22 * b,
		a._23 * b,
		a._24 * b,
		a._31 * b,
		a._32 * b,
		a._33 * b,
		a._34 * b,
		a._41 * b,
		a._42 * b,
		a._43 * b,
		a._44 * b
	);
}

static XMFLOAT4X4 AddFloat4x4(XMFLOAT4X4 a, XMFLOAT4X4 b)
{
	return XMFLOAT4X4(
		a._11 + b._11,
		a._12 + b._12,
		a._13 + b._13,
		a._14 + b._14,
		a._21 + b._21,
		a._22 + b._22,
		a._23 + b._23,
		a._24 + b._24,
		a._31 + b._31,
		a._32 + b._32,
		a._33 + b._33,
		a._34 + b._34,
		a._41 + b._41,
		a._42 + b._42,
		a._43 + b._43,
		a._44 + b._44
	);
}

FbxAMatrix GetGeometryTransformation(FbxNode* inNode)
{
	if (!inNode)
	{
		throw std::exception("Null for mesh geometry");
	}

	const FbxVector4 lT = inNode->GetGeometricTranslation(FbxNode::eSourcePivot);
	const FbxVector4 lR = inNode->GetGeometricRotation(FbxNode::eSourcePivot);
	const FbxVector4 lS = inNode->GetGeometricScaling(FbxNode::eSourcePivot);

	return FbxAMatrix(lT, lR, lS);
}

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
		return;

	FbxIOSettings* fbxIOSettings = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(fbxIOSettings);

	pScene = FbxScene::Create(pManager, "myScene");
}

void DestroySdkObjects(FbxManager* pManager, FbxScene*& pScene)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
}

void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex, Skeleton* mSkeleton)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currJoint;
		currJoint.mParentIndex = inParentIndex;
		currJoint.mName = inNode->GetName();
		mSkeleton->mJoints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, (int)mSkeleton->mJoints.size(), myIndex, mSkeleton);
	}
}

void ProcessSkeletonHierarchy(FbxNode* inRootNode, Skeleton* mSkeleton)
{
	for (int childIndex = 0; childIndex < inRootNode->GetChildCount(); ++childIndex)
	{
		FbxNode* currNode = inRootNode->GetChild(childIndex);
		ProcessSkeletonHierarchyRecursively(currNode, 0, 0, -1, mSkeleton);
	}
}

void ProcessControlPoints(FbxNode* inNode)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int ctrlPointCount = currMesh->GetControlPointsCount();
	for (unsigned int i = 0; i < ctrlPointCount; ++i)
	{
		CtrlPoint currCtrlPoint;
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = -1.0f * static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = -1.0f * static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint.mPosition = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}
}

unsigned int FindJointIndexUsingName(const std::string& inJointName, Skeleton* mSkeleton)
{
	for (unsigned int i = 0; i < mSkeleton->mJoints.size(); ++i)
	{
		if (mSkeleton->mJoints[i].mName == inJointName)
		{
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

void ProcessJointsAndAnimations(FbxScene*& pScene, FbxNode* inNode, Skeleton* mSkeleton)
{
	FbxMesh* currMesh = inNode->GetMesh();
	unsigned int numOfDeformers = currMesh->GetDeformerCount();
	// This geometry transform is something I cannot understand
	// I think it is from MotionBuilder
	// If you are using Maya for your models, 99% this is just an
	// identity matrix
	// But I am taking it into account anyways......
	FbxAMatrix geometryTransform = GetGeometryTransformation(inNode);

	// A deformer is a FBX thing, which contains some clusters
	// A cluster contains a link, which is basically a joint
	// Normally, there is only one deformer in a mesh
	for (unsigned int deformerIndex = 0; deformerIndex < numOfDeformers; ++deformerIndex)
	{
		// There are many types of deformers in Maya,
		// We are using only skins, so we see if this is a skin
		FbxSkin* currSkin = reinterpret_cast<FbxSkin*>(currMesh->GetDeformer(deformerIndex, FbxDeformer::eSkin));
		if (!currSkin)
		{
			continue;
		}

		unsigned int numOfClusters = currSkin->GetClusterCount();
		for (unsigned int clusterIndex = 0; clusterIndex < numOfClusters; ++clusterIndex)
		{
			FbxCluster* currCluster = currSkin->GetCluster(clusterIndex);
			std::string currJointName = currCluster->GetLink()->GetName();
			unsigned int currJointIndex = FindJointIndexUsingName(currJointName, mSkeleton);
			FbxAMatrix transformMatrix;
			FbxAMatrix transformLinkMatrix;
			FbxAMatrix globalBindposeInverseMatrix;

			currCluster->GetTransformMatrix(transformMatrix);	// The transformation of the mesh at binding time
			currCluster->GetTransformLinkMatrix(transformLinkMatrix);	// The transformation of the cluster(joint) at binding time from joint space to world space
			globalBindposeInverseMatrix = transformLinkMatrix.Inverse() * transformMatrix * geometryTransform;

			// Update the information in mSkeleton 
			mSkeleton->mJoints[currJointIndex].globalBindposeInverseMatrix = ToXm(globalBindposeInverseMatrix);
			//mSkeleton.mJoints[currJointIndex].translation = Vec4ToFloat3(globalBindposeInverseMatrix.GetT());
			//mSkeleton.mJoints[currJointIndex].rotation = Vec4ToFloat4(globalBindposeInverseMatrix.GetR());
			//mSkeleton.mJoints[currJointIndex].scale = Vec4ToFloat3(globalBindposeInverseMatrix.GetS());

			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				VertexBlendingInfo currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.mBlendingIndex = currJointIndex;
				currBlendingIndexWeightPair.mBlendingWeight = (float)currCluster->GetControlPointWeights()[i];
				mControlPoints[currCluster->GetControlPointIndices()[i]].mBlendingInfo.push_back(currBlendingIndexWeightPair);
			}

			// Get animation information
			// Now only supports one take
			FbxAnimStack* currAnimStack = pScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			mSkeleton->mAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = pScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mSkeleton->mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				Keyframe currAnim;
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				currAnim.mFrameNum = i;
				FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				FbxAMatrix mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				
				currAnim.mGlobalTransform = ToXm(mGlobalTransform);
				//currAnim.translation = Vec4ToFloat3(mGlobalTransform.GetT());
				//currAnim.rotation = Vec4ToFloat4(mGlobalTransform.GetR());
				//currAnim.scale = Vec4ToFloat3(mGlobalTransform.GetS());
				mSkeleton->mJoints[currJointIndex].mAnimation.push_back(currAnim);
			}
		}
	}

	// Some of the control points only have less than 4 joints
	// affecting them.
	// For a normal renderer, there are usually 4 joints
	// I am adding more dummy joints if there isn't enough
	VertexBlendingInfo currBlendingIndexWeightPair;
	currBlendingIndexWeightPair.mBlendingIndex = 0;
	currBlendingIndexWeightPair.mBlendingWeight = 0;
	for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		for (size_t i = itr->second.mBlendingInfo.size(); i <= 4; ++i)
		{
			itr->second.mBlendingInfo.push_back(currBlendingIndexWeightPair);
		}
	}
}

void ReadNormal(FbxMesh* inMesh, int inCtrlPointIndex, int inVertexCounter, XMFLOAT3& outNormal)
{
	if (inMesh->GetElementNormalCount() < 1)
	{
		throw std::exception("Invalid Normal Number");
	}

	FbxGeometryElementNormal* vertexNormal = inMesh->GetElementNormal(0);
	switch (vertexNormal->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inCtrlPointIndex).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inCtrlPointIndex);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexNormal->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(inVertexCounter).mData[2]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexNormal->GetIndexArray().GetAt(inVertexCounter);
			outNormal.x = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[0]);
			outNormal.y = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[1]);
			outNormal.z = static_cast<float>(vertexNormal->GetDirectArray().GetAt(index).mData[2]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void ReadUV(FbxMesh* inMesh, int inCtrlPointIndex, int inTextureUVIndex, int inUVLayer, XMFLOAT2& outUV)
{
	if (inUVLayer >= 2 || inMesh->GetElementUVCount() <= inUVLayer)
	{
		throw std::exception("Invalid UV Layer Number");
	}
	FbxGeometryElementUV* vertexUV = inMesh->GetElementUV(inUVLayer);

	switch (vertexUV->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[0]);
			outUV.y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;

	case FbxGeometryElement::eByPolygonVertex:
		switch (vertexUV->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		case FbxGeometryElement::eIndexToDirect:
		{
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[0]);
			outUV.y = 1.0f - static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void ProcessMesh(FbxNode* inNode, std::vector<unsigned int>* indicies, std::vector<PNTIWVertex>* mVertices)
{
	FbxMesh* currMesh = inNode->GetMesh();

	unsigned int mTriangleCount = currMesh->GetPolygonCount();
	int vertexCounter = 0;
	indicies->reserve(mTriangleCount);

	for (unsigned int i = 0; i < mTriangleCount; ++i)
	{
		XMFLOAT3 normal[3];
		//XMFLOAT3 tangent[3];
		//XMFLOAT3 binormal[3];
		XMFLOAT2 UV[3][2];

		for (unsigned int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
			CtrlPoint currCtrlPoint = mControlPoints[ctrlPointIndex];

			ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal[j]);
			// We only have diffuse texture
			for (int k = 0; k < 1; ++k)
			{
				ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), k, UV[j][k]);
			}

			PNTIWVertex temp;
			temp.mPosition = currCtrlPoint.mPosition;
			temp.mNormal = normal[j];
			temp.mUV = UV[j][0];
			currCtrlPoint.SortBlendingInfoByWeight();
			size_t k = 0;
			while (k < currCtrlPoint.mBlendingInfo.size())
			{
				if (currCtrlPoint.mBlendingInfo[k].mBlendingWeight == 0.0f)
				{
					currCtrlPoint.mBlendingInfo.erase(currCtrlPoint.mBlendingInfo.begin() + k);
				}
				else
					k++;
			}
			// Copy the blending info from each control point
			for (unsigned int l = 0; l < currCtrlPoint.mBlendingInfo.size(); ++l)
			{
				temp.mBlendingIndex[l] = currCtrlPoint.mBlendingInfo[l].mBlendingIndex;
				temp.mBlendingWeight[l] = currCtrlPoint.mBlendingInfo[l].mBlendingWeight;
			}
			// Sort the blending info so that later we can remove
			// duplicated vertices

			mVertices->push_back(temp);
			indicies->push_back(vertexCounter);
			++vertexCounter;
		}
	}

	// Now mControlPoints has served its purpose
	mControlPoints.clear();
}


void ProcessGeometry(FbxScene*& pScene, FbxNode* inNode, Skeleton* mSkeleton, std::vector<unsigned int>* indicies, std::vector<PNTIWVertex>* mVertices)
{
	if (inNode->GetNodeAttribute())
	{
		switch (inNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			ProcessControlPoints(inNode);
			ProcessJointsAndAnimations(pScene, inNode, mSkeleton);
			ProcessMesh(inNode, indicies, mVertices);
			//AssociateMaterialToMesh(inNode);
			//ProcessMaterials(inNode);
			break;
		}
	}

	for (int i = 0; i < inNode->GetChildCount(); ++i)
	{
		ProcessGeometry(pScene, inNode->GetChild(i), mSkeleton, indicies, mVertices);
	}
}

Keyframe Interpolate(Keyframe a, Keyframe b, float interp)
{
	Keyframe c;
	c.mGlobalTransform = AddFloat4x4(MultiplyFloat(a.mGlobalTransform, (1 - interp)), MultiplyFloat(b.mGlobalTransform, interp));
	return c;
}

void ReduceKeyframes(Skeleton* skelly)
{
	for (size_t i = 0; i < skelly->mJoints.size(); i++)
	{
		int j = 0;
		while (j < (int)skelly->mJoints[i].mAnimation.size() - 2)
		{
			if (skelly->mJoints[i].mAnimation[j + 1] == Interpolate(skelly->mJoints[i].mAnimation[j], skelly->mJoints[i].mAnimation[j + 2], .5f))
			{
				skelly->mJoints[i].mAnimation.erase(skelly->mJoints[i].mAnimation.begin() + j + 1);
			}
			else
				j++;
		}
	}
}

void WriteToBinary(const char* savefile, Skeleton skelly, std::vector<unsigned int> indicies, std::vector<PNTIWVertex> verts)
{
	std::fstream f;
	f.open(savefile, std::ios::out | std::ios::binary);

	f.write((char*)&skelly.mAnimationLength, sizeof(skelly.mAnimationLength));
	int len = (int)skelly.mAnimationName.size() + 1;
	f.write((char*)&len, sizeof(len));
	f.write(skelly.mAnimationName.c_str(), len);

	int size = (int)skelly.mJoints.size();
	f.write((char*)&size, sizeof(size));

	for (size_t i = 0; i < size; i++)
	{
		int length = (int)skelly.mJoints[i].mName.size() + 1;
		f.write((char*)&length, sizeof(length));
		f.write(skelly.mJoints[i].mName.c_str(), length);
		f.write((char*)&skelly.mJoints[i].mParentIndex, sizeof(skelly.mJoints[i].mParentIndex));
		f.write((char*)&skelly.mJoints[i].globalBindposeInverseMatrix, sizeof(skelly.mJoints[i].globalBindposeInverseMatrix));
		int animsize = (int)skelly.mJoints[i].mAnimation.size();
		f.write((char*)&animsize, sizeof(animsize));
		for (size_t j = 0; j < animsize; j++)
		{
			f.write((char*)&skelly.mJoints[i].mAnimation[j].mFrameNum, sizeof(skelly.mJoints[i].mAnimation[j].mFrameNum));
			f.write((char*)&skelly.mJoints[i].mAnimation[j].mGlobalTransform, sizeof(skelly.mJoints[i].mAnimation[j].mGlobalTransform));
		}
	}

	int vlen = (int)verts.size();
	f.write((char*)&vlen, sizeof(vlen));
	for (size_t i = 0; i < vlen; i++)
	{
		f.write((char*)&verts[i].mPosition, sizeof(verts[i].mPosition));
		f.write((char*)&verts[i].mNormal, sizeof(verts[i].mNormal));
		f.write((char*)&verts[i].mUV, sizeof(verts[i].mUV));
		f.write((char*)&verts[i].mBlendingWeight[0], sizeof(verts[i].mBlendingWeight));
		f.write((char*)&verts[i].mBlendingIndex[0], sizeof(verts[i].mBlendingIndex));
	}

	int ilen = (int)indicies.size();
	f.write((char*)&ilen, sizeof(ilen));
	for (size_t i = 0; i < ilen; i++)
	{
		f.write((char*)&indicies[i], sizeof(indicies[i]));
	}

	f.close();
}

void FBXtoBinary(const char* fbxfile, const char* binfile, bool overwrite)
{
	std::ifstream f(binfile);
	if (!overwrite && f.good())
	{
		f.close();
		return;
	}
	f.close();

	FbxManager* pManager;
	FbxScene* pScene;
	InitializeSdkObjects(pManager, pScene);

	FbxImporter* fbxImporter = FbxImporter::Create(pManager, "myImporter");

	if (!fbxImporter)
		return;

	if (!fbxImporter->Initialize(fbxfile, -1, pManager->GetIOSettings()))
		return;

	if (!fbxImporter->Import(pScene))
		return;

	// Destroy the importer.
	fbxImporter->Destroy();

	Skeleton skelly;
	ProcessSkeletonHierarchy(pScene->GetRootNode(), &skelly);

	//Uncomment if working with files without animations
	//if (skelly.mJoints.empty())
	//	mHasAnimation = false;

	std::vector<unsigned int> indicies;
	std::vector<PNTIWVertex> verts;

	ProcessGeometry(pScene, pScene->GetRootNode(), &skelly, &indicies, &verts);
	//ReduceKeyframes(&skelly);

	//Output skelleton, indicies, and verts
	WriteToBinary(binfile, skelly, indicies, verts);

	DestroySdkObjects(pManager, pScene);
	return;
}

bool ReadBinary(const char* loadfile, Skeleton* skelly, std::vector<unsigned int>* indicies, std::vector<PNTIWVertex>* verts)
{
	std::fstream f;
	f.open(loadfile, std::ios::in | std::ios::binary);
	if (!f.good())
		return false;

	f.read((char*)&skelly->mAnimationLength, sizeof(skelly->mAnimationLength));
	int len;
	f.read((char*)&len, sizeof(len));
	char* temp = new char[len];
	f.read(temp, len);
	skelly->mAnimationName = temp;
	delete[] temp;

	int size;
	f.read((char*)&size, sizeof(size));
	skelly->mJoints.reserve(size);

	for (size_t i = 0; i < size; i++)
	{
		Joint tj;
		int length;
		f.read((char*)&length, sizeof(length));
		temp = new char[length];
		f.read(temp, length);
		tj.mName = temp;
		delete[] temp;
		f.read((char*)&tj.mParentIndex, sizeof(tj.mParentIndex));
		f.read((char*)&tj.globalBindposeInverseMatrix, sizeof(tj.globalBindposeInverseMatrix));
		int animsize;
		f.read((char*)&animsize, sizeof(animsize));
		tj.mAnimation.reserve(animsize);
		for (size_t j = 0; j < animsize; j++)
		{
			Keyframe tk;
			f.read((char*)&tk.mFrameNum, sizeof(tk.mFrameNum));
			f.read((char*)&tk.mGlobalTransform, sizeof(tk.mGlobalTransform));
			tj.mAnimation.push_back(tk);
		}
		skelly->mJoints.push_back(tj);
	}

	int vlen;
	f.read((char*)&vlen, sizeof(vlen));
	verts->reserve(vlen);
	for (size_t i = 0; i < vlen; i++)
	{
		PNTIWVertex tv;
		f.read((char*)&tv.mPosition, sizeof(tv.mPosition));
		f.read((char*)&tv.mNormal, sizeof(tv.mNormal));
		f.read((char*)&tv.mUV, sizeof(tv.mUV));
		f.read((char*)&tv.mBlendingWeight[0], sizeof(tv.mBlendingWeight));
		f.read((char*)&tv.mBlendingIndex[0], sizeof(tv.mBlendingIndex));
		verts->push_back(tv);
	}

	int ilen;
	f.read((char*)&ilen, sizeof(ilen));
	indicies->reserve(ilen);
	for (size_t i = 0; i < ilen; i++)
	{
		unsigned int t;
		f.read((char*)&t, sizeof(t));
		indicies->push_back(t);
	}

	f.close();
	return true;
}



void GetMyShit(FbxNode* node, std::vector<MyMesh> &mesh, std::vector<Bone> &boner)
{
	if (node->GetNodeAttribute() == NULL)
		return;

	FbxNodeAttribute::EType AttributeType = node->GetNodeAttribute()->GetAttributeType();

	if (AttributeType == FbxNodeAttribute::eMesh)
	{
		FbxMesh* pMesh = (FbxMesh*)node->GetNodeAttribute();

		FbxVector4* pVertices = pMesh->GetControlPoints();

		for (int j = 0; j < pMesh->GetPolygonCount(); j++)
		{
			for (int k = 0; k < 3; k++)
			{
				int iControlPointIndex = pMesh->GetPolygonVertex(j, k);

				float vertex[4];
				vertex[0] = (float)pVertices[iControlPointIndex].mData[0];
				vertex[1] = (float)pVertices[iControlPointIndex].mData[1];
				vertex[2] = (float)pVertices[iControlPointIndex].mData[2];
				vertex[3] = 1.0f;

				//get the normal element
				FbxGeometryElementNormal* lNormalElement = pMesh->GetElementNormal();
				float norm[4];
				if (lNormalElement)
				{
					//mapping mode is by control points. The mesh should be smooth and soft.
					//we can get normals by retrieving each control point
					if (lNormalElement->GetMappingMode() == FbxGeometryElement::eByControlPoint ||
						lNormalElement->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
					{
						int lNormalIndex = iControlPointIndex;
						//reference mode is direct, the normal index is same as vertex index.
						//get normals by the index of control vertex
						/*if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eDirect)
						lNormalIndex = i;*/

						//reference mode is index-to-direct, get normals by the index-to-direct
						if (lNormalElement->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
							lNormalIndex = lNormalElement->GetIndexArray().GetAt(iControlPointIndex);

						//Got normals of each vertex.
						FbxVector4 lNormal = lNormalElement->GetDirectArray().GetAt(lNormalIndex);
						norm[0] = (float)lNormal[0];
						norm[1] = (float)lNormal[1];
						norm[2] = (float)lNormal[2];
						norm[3] = 1.0f;
					}
				}

				//Get UVs
				FbxVector2 uvCoords;
				FbxStringList uvstring;
				pMesh->GetUVSetNames(uvstring);
				bool tool;
				pMesh->GetPolygonVertexUV(j, k, uvstring.GetStringAt(0), uvCoords, tool);

				float uvs[4] = { static_cast<float>(uvCoords[0]), 1.0f - static_cast<float>(uvCoords[1]), 0.0f, 0.0f };

				MyMesh temp = { vertex, norm, uvs };
				mesh.push_back(temp);
			}
		}
	}//&boner != &null_fill &&
	else if (&boner != &null_fill && AttributeType == FbxNodeAttribute::eSkeleton)
	{
		FbxAMatrix temp = node->EvaluateGlobalTransform(0);
		FbxVector4 tempt = temp.GetT();
		FbxVector4 tempr = temp.GetR();
		FbxVector4 temps = temp.GetS();
		Bone bone = Bone((float)tempt.mData[0], (float)tempt.mData[1], (float)tempt.mData[2], (float)tempt.mData[3], (float)tempr.mData[0], (float)tempr.mData[1], (float)tempr.mData[2], (float)tempr.mData[3], (float)temps.mData[0], (float)temps.mData[1], (float)temps.mData[2], (float)temps.mData[3]);
		boner.push_back(bone);
	}
	return;
}

void GetMyShitRecursive(FbxNode* node, std::vector<MyMesh> &mesh, std::vector<Bone> &boner)
{
	GetMyShit(node, mesh, boner);
	for (int i = 0; i < node->GetChildCount(); i++)
	{
		GetMyShitRecursive(node->GetChild(i), mesh, boner);
	}
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

void LoadScene(const char* pFilename, std::vector<MyMesh> &mesh, std::vector<Bone> &boner)
{
	FbxManager* pManager;
	FbxScene* pScene;

	InitializeSdkObjects(pManager, pScene);

	int lFileMajor, lFileMinor, lFileRevision;
	int lSDKMajor, lSDKMinor, lSDKRevision;
	//int lFileFormat = -1;
	int i, lAnimStackCount;
	bool lStatus;

	// Get the file version number generate by the FBX SDK.
	FbxManager::GetFileFormatVersion(lSDKMajor, lSDKMinor, lSDKRevision);

	// Create an importer.
	FbxImporter* lImporter = FbxImporter::Create(pManager, "");

	// Initialize the importer by providing a filename.
	const bool lImportStatus = lImporter->Initialize(pFilename, -1, pManager->GetIOSettings());
	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);

	if (!lImportStatus)
	{
		FbxString error = lImporter->GetStatus().GetErrorString();
		FBXSDK_printf("Call to FbxImporter::Initialize() failed.\n");
		FBXSDK_printf("Error returned: %s\n\n", error.Buffer());

		if (lImporter->GetStatus().GetCode() == FbxStatus::eInvalidFileVersion)
		{
			FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);
			FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);
		}
		return;
	}

	FBXSDK_printf("FBX file format version for this FBX SDK is %d.%d.%d\n", lSDKMajor, lSDKMinor, lSDKRevision);

	if (lImporter->IsFBX())
	{
		FBXSDK_printf("FBX file format version for file '%s' is %d.%d.%d\n\n", pFilename, lFileMajor, lFileMinor, lFileRevision);

		// From this point, it is possible to access animation stack information without
		// the expense of loading the entire file.

		FBXSDK_printf("Animation Stack Information\n");

		lAnimStackCount = lImporter->GetAnimStackCount();

		FBXSDK_printf("    Number of Animation Stacks: %d\n", lAnimStackCount);
		FBXSDK_printf("    Current Animation Stack: \"%s\"\n", lImporter->GetActiveAnimStackName().Buffer());
		FBXSDK_printf("\n");

		for (i = 0; i < lAnimStackCount; i++)
		{
			FbxTakeInfo* lTakeInfo = lImporter->GetTakeInfo(i);

			FBXSDK_printf("    Animation Stack %d\n", i);
			FBXSDK_printf("         Name: \"%s\"\n", lTakeInfo->mName.Buffer());
			FBXSDK_printf("         Description: \"%s\"\n", lTakeInfo->mDescription.Buffer());

			// Change the value of the import name if the animation stack should be imported 
			// under a different name.
			FBXSDK_printf("         Import Name: \"%s\"\n", lTakeInfo->mImportName.Buffer());

			// Set the value of the import state to false if the animation stack should be not
			// be imported. 
			FBXSDK_printf("         Import State: %s\n", lTakeInfo->mSelect ? "true" : "false");
			FBXSDK_printf("\n");
		}

		// Set the import states. By default, the import states are always set to 
		// true. The code below shows how to change these states.
		IOS_REF.SetBoolProp(IMP_FBX_MATERIAL, true);
		IOS_REF.SetBoolProp(IMP_FBX_TEXTURE, true);
		IOS_REF.SetBoolProp(IMP_FBX_LINK, true);
		IOS_REF.SetBoolProp(IMP_FBX_SHAPE, true);
		IOS_REF.SetBoolProp(IMP_FBX_GOBO, true);
		IOS_REF.SetBoolProp(IMP_FBX_ANIMATION, true);
		IOS_REF.SetBoolProp(IMP_FBX_GLOBAL_SETTINGS, true);
	}

	// Import the scene.
	lStatus = lImporter->Import(pScene);

	//If working with passwords enable
#ifdef false
	char lPassword[1024];
	if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
	{
		FBXSDK_printf("Please enter password: ");

		lPassword[0] = '\0';

		FBXSDK_CRT_SECURE_NO_WARNING_BEGIN
			scanf("%s", lPassword);
		FBXSDK_CRT_SECURE_NO_WARNING_END

			FbxString lString(lPassword);

		IOS_REF.SetStringProp(IMP_FBX_PASSWORD, lString);
		IOS_REF.SetBoolProp(IMP_FBX_PASSWORD_ENABLE, true);

		lStatus = lImporter->Import(pScene);

		if (lStatus == false && lImporter->GetStatus().GetCode() == FbxStatus::ePasswordError)
		{
			FBXSDK_printf("\nPassword is wrong, import aborted.\n");
		}
	}
#endif // 0

	// Destroy the importer.
	lImporter->Destroy();

	GetMyShitRecursive(pScene->GetRootNode(), mesh, boner);
	DestroySdkObjects(pManager, true);
	return;
}
