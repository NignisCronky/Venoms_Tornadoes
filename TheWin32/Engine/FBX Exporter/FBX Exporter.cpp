// FBX Exporter.cpp : Defines the exported functions for the DLL application.
#pragma once
#include <fbxsdk.h>
#include <fbxsdk\fileio\fbxiosettings.h>
#include "FBX Exporter.h"
#include <fstream>

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

using namespace DirectX;

XMFLOAT3 Vec4ToFloat3(FbxVector4 vec)
{
	return XMFLOAT3((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2]);
}
XMFLOAT4 Vec4ToFloat4(FbxVector4 vec)
{
	return XMFLOAT4((float)vec.mData[0], (float)vec.mData[1], (float)vec.mData[2], (float)vec.mData[3]);
}

static XMMATRIX ToXm(const FbxAMatrix& pSrc)
{
	return XMMatrixSet(
		static_cast<float>(pSrc.Get(0, 0)), static_cast<float>(pSrc.Get(0, 1)), static_cast<float>(pSrc.Get(0, 2)), static_cast<float>(pSrc.Get(0, 3)),
		static_cast<float>(pSrc.Get(1, 0)), static_cast<float>(pSrc.Get(1, 1)), static_cast<float>(pSrc.Get(1, 2)), static_cast<float>(pSrc.Get(1, 3)),
		static_cast<float>(pSrc.Get(2, 0)), static_cast<float>(pSrc.Get(2, 1)), static_cast<float>(pSrc.Get(2, 2)), static_cast<float>(pSrc.Get(2, 3)),
		static_cast<float>(pSrc.Get(3, 0)), static_cast<float>(pSrc.Get(3, 1)), static_cast<float>(pSrc.Get(3, 2)), static_cast<float>(pSrc.Get(3, 3)));
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
	if (pScene) pScene->Destroy();
}

void ProcessSkeletonHierarchyRecursively(FbxNode* inNode, int inDepth, int myIndex, int inParentIndex, Skeleton mSkeleton)
{
	if (inNode->GetNodeAttribute() && inNode->GetNodeAttribute()->GetAttributeType() && inNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton)
	{
		Joint currJoint;
		currJoint.mParentIndex = inParentIndex;
		currJoint.mName = inNode->GetName();
		mSkeleton.mJoints.push_back(currJoint);
	}
	for (int i = 0; i < inNode->GetChildCount(); i++)
	{
		ProcessSkeletonHierarchyRecursively(inNode->GetChild(i), inDepth + 1, (int)mSkeleton.mJoints.size(), myIndex, mSkeleton);
	}
}

void ProcessSkeletonHierarchy(FbxNode* inRootNode, Skeleton mSkeleton)
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
		CtrlPoint* currCtrlPoint = new CtrlPoint();
		XMFLOAT3 currPosition;
		currPosition.x = static_cast<float>(currMesh->GetControlPointAt(i).mData[0]);
		currPosition.y = static_cast<float>(currMesh->GetControlPointAt(i).mData[1]);
		currPosition.z = static_cast<float>(currMesh->GetControlPointAt(i).mData[2]);
		currCtrlPoint->mPosition = currPosition;
		mControlPoints[i] = currCtrlPoint;
	}
}

unsigned int FindJointIndexUsingName(const std::string& inJointName, Skeleton mSkeleton)
{
	for (unsigned int i = 0; i < mSkeleton.mJoints.size(); ++i)
	{
		if (mSkeleton.mJoints[i].mName == inJointName)
		{
			return i;
		}
	}

	throw std::exception("Skeleton information in FBX file is corrupted.");
}

void ProcessJointsAndAnimations(FbxScene*& pScene, FbxNode* inNode, Skeleton mSkeleton)
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
			mSkeleton.mJoints[currJointIndex].translation = Vec4ToFloat3(globalBindposeInverseMatrix.GetT());
			mSkeleton.mJoints[currJointIndex].rotation = Vec4ToFloat4(globalBindposeInverseMatrix.GetR());
			//mSkeleton.mJoints[currJointIndex].scale = Vec4ToFloat3(globalBindposeInverseMatrix.GetS());

			// Associate each joint with the control points it affects
			unsigned int numOfIndices = currCluster->GetControlPointIndicesCount();
			for (unsigned int i = 0; i < numOfIndices; ++i)
			{
				VertexBlendingInfo currBlendingIndexWeightPair;
				currBlendingIndexWeightPair.mBlendingIndex = currJointIndex;
				currBlendingIndexWeightPair.mBlendingWeight = (float)currCluster->GetControlPointWeights()[i];
				mControlPoints[currCluster->GetControlPointIndices()[i]]->mBlendingInfo.push_back(currBlendingIndexWeightPair);
			}

			// Get animation information
			// Now only supports one take
			FbxAnimStack* currAnimStack = pScene->GetSrcObject<FbxAnimStack>(0);
			FbxString animStackName = currAnimStack->GetName();
			mSkeleton.mAnimationName = animStackName.Buffer();
			FbxTakeInfo* takeInfo = pScene->GetTakeInfo(animStackName);
			FbxTime start = takeInfo->mLocalTimeSpan.GetStart();
			FbxTime end = takeInfo->mLocalTimeSpan.GetStop();
			mSkeleton.mAnimationLength = end.GetFrameCount(FbxTime::eFrames24) - start.GetFrameCount(FbxTime::eFrames24) + 1;

			for (FbxLongLong i = start.GetFrameCount(FbxTime::eFrames24); i <= end.GetFrameCount(FbxTime::eFrames24); ++i)
			{
				Keyframe currAnim;
				FbxTime currTime;
				currTime.SetFrame(i, FbxTime::eFrames24);
				currAnim.mFrameNum = i;
				FbxAMatrix currentTransformOffset = inNode->EvaluateGlobalTransform(currTime) * geometryTransform;
				FbxAMatrix mGlobalTransform = currentTransformOffset.Inverse() * currCluster->GetLink()->EvaluateGlobalTransform(currTime);
				currAnim.translation = Vec4ToFloat3(mGlobalTransform.GetT());
				currAnim.rotation = Vec4ToFloat4(mGlobalTransform.GetR());
				//currAnim.scale = Vec4ToFloat3(mGlobalTransform.GetS());
				mSkeleton.mJoints[currJointIndex].mAnimation.push_back(currAnim);
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
		for (size_t i = itr->second->mBlendingInfo.size(); i <= 4; ++i)
		{
			itr->second->mBlendingInfo.push_back(currBlendingIndexWeightPair);
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
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inCtrlPointIndex).mData[1]);
		}
		break;

		case FbxGeometryElement::eIndexToDirect:
		{
			int index = vertexUV->GetIndexArray().GetAt(inCtrlPointIndex);
			outUV.x = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[0]);
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(index).mData[1]);
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
			outUV.y = static_cast<float>(vertexUV->GetDirectArray().GetAt(inTextureUVIndex).mData[1]);
		}
		break;

		default:
			throw std::exception("Invalid Reference");
		}
		break;
	}
}

void ProcessMesh(FbxNode* inNode, std::vector<unsigned int> indicies, std::vector<PNTIWVertex> mVertices)
{
	FbxMesh* currMesh = inNode->GetMesh();

	unsigned int mTriangleCount = currMesh->GetPolygonCount();
	int vertexCounter = 0;
	indicies.reserve(mTriangleCount);

	for (unsigned int i = 0; i < mTriangleCount; ++i)
	{
		XMFLOAT3 normal[3];
		//XMFLOAT3 tangent[3];
		//XMFLOAT3 binormal[3];
		XMFLOAT2 UV[3][2];

		for (unsigned int j = 0; j < 3; ++j)
		{
			int ctrlPointIndex = currMesh->GetPolygonVertex(i, j);
			CtrlPoint* currCtrlPoint = mControlPoints[ctrlPointIndex];

			ReadNormal(currMesh, ctrlPointIndex, vertexCounter, normal[j]);
			// We only have diffuse texture
			for (int k = 0; k < 1; ++k)
			{
				ReadUV(currMesh, ctrlPointIndex, currMesh->GetTextureUVIndex(i, j), k, UV[j][k]);
			}

			PNTIWVertex temp;
			temp.mPosition = currCtrlPoint->mPosition;
			temp.mNormal = normal[j];
			temp.mUV = UV[j][0];
			// Copy the blending info from each control point
			for (unsigned int i = 0; i < currCtrlPoint->mBlendingInfo.size(); ++i)
			{
				VertexBlendingInfo currBlendingInfo;
				currBlendingInfo.mBlendingIndex = currCtrlPoint->mBlendingInfo[i].mBlendingIndex;
				currBlendingInfo.mBlendingWeight = currCtrlPoint->mBlendingInfo[i].mBlendingWeight;
				temp.mVertexBlendingInfos.push_back(currBlendingInfo);
			}
			// Sort the blending info so that later we can remove
			// duplicated vertices
			temp.SortBlendingInfoByWeight();

			mVertices.push_back(temp);
			indicies.push_back(vertexCounter);
			++vertexCounter;
		}
	}

	// Now mControlPoints has served its purpose
	// We can free its memory
	for (auto itr = mControlPoints.begin(); itr != mControlPoints.end(); ++itr)
	{
		delete itr->second;
	}
	mControlPoints.clear();
}


void ProcessGeometry(FbxScene*& pScene, FbxNode* inNode, Skeleton mSkeleton, std::vector<unsigned int> indicies, std::vector<PNTIWVertex> mVertices)
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
	c.rotation = XMFLOAT4((1 - interp) * a.rotation.x + interp * b.rotation.x, (1 - interp) * a.rotation.y + interp * b.rotation.y, (1 - interp) * a.rotation.z + interp * b.rotation.z, (1 - interp) * a.rotation.w + interp * b.rotation.w);
	c.translation = XMFLOAT3((1 - interp) * a.translation.x + interp * b.translation.x, (1 - interp) * a.translation.y + interp * b.translation.y, (1 - interp) * a.translation.z + interp * b.translation.z);
	return c;
}

void ReduceKeyframes(Skeleton skelly)
{
	for (size_t i = 0; i < skelly.mJoints.size(); i++)
	{
		int j = 0;
		while (j < skelly.mJoints[i].mAnimation.size() - 2)
		{
			if (skelly.mJoints[i].mAnimation[j + 1] == Interpolate(skelly.mJoints[i].mAnimation[j], skelly.mJoints[i].mAnimation[j + 2], .5f))
			{
				skelly.mJoints[i].mAnimation.erase(skelly.mJoints[i].mAnimation.begin() + j + 1);
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
		f.write((char*)&skelly.mJoints[i].translation, sizeof(skelly.mJoints[i].translation));
		f.write((char*)&skelly.mJoints[i].rotation, sizeof(skelly.mJoints[i].rotation));
		int animsize = (int)skelly.mJoints[i].mAnimation.size();
		f.write((char*)&animsize, sizeof(animsize));
		for (size_t j = 0; j < animsize; j++)
		{
			f.write((char*)&skelly.mJoints[i].mAnimation[j].mFrameNum, sizeof(skelly.mJoints[i].mAnimation[j].mFrameNum));
			f.write((char*)&skelly.mJoints[i].mAnimation[j].translation, sizeof(skelly.mJoints[i].mAnimation[j].translation));
			f.write((char*)&skelly.mJoints[i].mAnimation[j].rotation, sizeof(skelly.mJoints[i].mAnimation[j].rotation));
		}
	}

	int vlen = (int)verts.size();
	f.write((char*)&vlen, sizeof(vlen));
	for (size_t i = 0; i < vlen; i++)
	{
		f.write((char*)&verts[i].mPosition, sizeof(verts[i].mPosition));
		f.write((char*)&verts[i].mNormal, sizeof(verts[i].mNormal));
		f.write((char*)&verts[i].mUV, sizeof(verts[i].mUV));
		int blen = (int)verts[i].mVertexBlendingInfos.size();
		f.write((char*)&blen, sizeof(blen));
		for (size_t j = 0; j < blen; j++)
		{
			f.write((char*)&verts[i].mVertexBlendingInfos[j].mBlendingIndex, sizeof(verts[i].mVertexBlendingInfos[j].mBlendingIndex));
			f.write((char*)&verts[i].mVertexBlendingInfos[j].mBlendingWeight, sizeof(verts[i].mVertexBlendingInfos[j].mBlendingWeight));
		}
	}

	int ilen = (int)indicies.size();
	f.write((char*)&ilen, sizeof(ilen));
	for (size_t i = 0; i < ilen; i++)
	{
		f.write((char*)&indicies[i], sizeof(indicies[i]));
	}

	f.close();
}

void FBXtoBinary(const char* loadfile, const char* savefile, bool overwrite = true)
{
	std::ifstream f(savefile);
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

	if (!fbxImporter->Initialize(loadfile, -1, pManager->GetIOSettings()))
		return;

	if (!fbxImporter->Import(pScene))
		return;

	// Destroy the importer.
	fbxImporter->Destroy();

	Skeleton skelly;
	ProcessSkeletonHierarchy(pScene->GetRootNode(), skelly);

	//Uncomment if working with files without animations
	//if (skelly.mJoints.empty())
	//	mHasAnimation = false;

	std::vector<unsigned int> indicies;
	std::vector<PNTIWVertex> verts;

	ProcessGeometry(pScene, pScene->GetRootNode(), skelly, indicies, verts);
	ReduceKeyframes(skelly);

	//Output skelleton, indicies, and verts
	WriteToBinary(savefile, skelly, indicies, verts);

	DestroySdkObjects(pManager, pScene);
	return;
}

bool ReadBinary(const char* loadfile, Skeleton skelly, std::vector<unsigned int> indicies, std::vector<PNTIWVertex> verts)
{
	std::fstream f;
	f.open(loadfile, std::ios::in | std::ios::binary);
	if (!f.good())
		return false;

	f.read((char*)&skelly.mAnimationLength, sizeof(skelly.mAnimationLength));
	int len;
	f.read((char*)&len, sizeof(len));
	char* temp = new char[len];
	f.read(temp, len);
	skelly.mAnimationName = temp;
	delete[] temp;

	int size;
	f.read((char*)&size, sizeof(size));
	skelly.mJoints.resize(size);

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
		f.read((char*)&tj.translation, sizeof(tj.translation));
		f.read((char*)&tj.rotation, sizeof(tj.rotation));
		int animsize;
		f.read((char*)&animsize, sizeof(animsize));
		tj.mAnimation.resize(animsize);
		for (size_t j = 0; j < animsize; j++)
		{
			Keyframe tk;
			f.read((char*)&tk.mFrameNum, sizeof(tk.mFrameNum));
			f.read((char*)&tk.translation, sizeof(tk.translation));
			f.read((char*)&tk.rotation, sizeof(tk.rotation));
			tj.mAnimation.push_back(tk);
		}
		skelly.mJoints.push_back(tj);
	}

	int vlen;
	f.read((char*)&vlen, sizeof(vlen));
	verts.resize(vlen);
	for (size_t i = 0; i < vlen; i++)
	{
		PNTIWVertex tv;
		f.read((char*)&tv.mPosition, sizeof(tv.mPosition));
		f.read((char*)&tv.mNormal, sizeof(tv.mNormal));
		f.read((char*)&tv.mUV, sizeof(tv.mUV));
		int blen;
		f.read((char*)&blen, sizeof(blen));
		for (size_t j = 0; j < blen; j++)
		{
			VertexBlendingInfo tvb;
			f.read((char*)&tvb.mBlendingIndex, sizeof(tvb.mBlendingIndex));
			f.read((char*)&tvb.mBlendingWeight, sizeof(tvb.mBlendingWeight));
			tv.mVertexBlendingInfos.push_back(tvb);
		}
		verts.push_back(tv);
	}

	int ilen;
	f.read((char*)&ilen, sizeof(ilen));
	indicies.resize(ilen);
	for (size_t i = 0; i < ilen; i++)
	{
		unsigned int t;
		f.read((char*)&t, sizeof(t));
		indicies.push_back(t);
	}

	f.close();
	return true;
}