// FBX Exporter.cpp : Defines the exported functions for the DLL application.
#pragma once
#include "stdafx.h"
#include <fbxsdk\fileio\fbxiosettings.h>
#include "FBX Exporter.h"
#include "fbxsdk.h"

#ifdef IOS_REF
#undef  IOS_REF
#define IOS_REF (*(pManager->GetIOSettings()))
#endif

//Exporter/loader I tried writing with base code from the fbxsdk manual and http://www.walkerb.net/blog/dx-4/
//Incomplete, only loads vertexes, but has very good comments. 
//HRESULT LoadFBX(const char* filename, std::vector<DirectX::XMFLOAT4>* pOutVertexVector)
//{
//	// Create the FBX SDK manager
//	FbxManager* lSdkManager = FbxManager::Create();
//	// Create an IOSettings object.
//	FbxIOSettings * ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
//	lSdkManager->SetIOSettings(ios);
//
//	// ... Configure the FbxIOSettings object ...
//
//	// Create an importer.
//	FbxImporter* lImporter = FbxImporter::Create(lSdkManager, "");
//	// Initialize the importer.
//	bool lImportStatus = lImporter->Initialize(filename, -1, lSdkManager->GetIOSettings());
//	//If any errors occur in the call to FbxImporter::Initialize(), the method returns false.
//	//To retrieve the error, you must call GetStatus().GetErrorString() from the FbxImporter object.
//	if (!lImportStatus) {
//		printf("Call to FbxImporter::Initialize() failed.\n");
//		printf("Error returned: %s\n\n", lImporter->GetStatus().GetErrorString());
//		exit(-1);
//	}
//	//Once the importer has been initialized, a scene container must be created to load the scene from the file.
//	//Scenes in the FBX SDK are abstracted by the FbxScene class.
//	//Create a new scene so it can be populated by the imported file.
//	FbxScene* lScene = FbxScene::Create(lSdkManager, "myScene");
//	//Import the contents of the file into the scene.
//	lImporter->Import(lScene);
//	//After the importer has populated the scene, it is safe to destroy it to reduce memory usage.
//	//The file has been imported; we can get rid of the importer.
//	lImporter->Destroy();
//	//The FBX file format version is incremented to reflect newly supported features.
//	//The FBX version of the currently imported file can be obtained by calling FbxImporter::GetFileVersion().
//	//File format version numbers to be populated.
//	int lFileMajor, lFileMinor, lFileRevision;
//	// Populate the FBX file format version numbers with the import file.
//	lImporter->GetFileVersion(lFileMajor, lFileMinor, lFileRevision);
//	
//	
//	//When the FBX SDK encounters a new object that it does not recognize during the importing process,
//	  //it creates a new class for it at runtime.
//	
//	  //FbxClassId lShaderClassID = lSdkManager->FindFbxFileClass("Shader", "FlatShader");
//	//for (int i = 0; i < lNode->GetSrcObjectCount(lShaderClassID); i++) {
//	//	FbxObject* lObject = lNode->GetSrcObject(lShaderClassID, i);
//	//}
//
//
//	FbxNode* pFbxRootNode = lScene->GetRootNode();
//	if (pFbxRootNode)
//	{
//		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
//		{
//			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);
//
//			if (pFbxChildNode->GetNodeAttribute() == NULL)
//				continue;
//
//			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();
//
//			if (AttributeType != FbxNodeAttribute::eMesh)
//				continue;
//
//			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();
//			
//			FbxVector4* pVertices = pMesh->GetControlPoints();
//			
//			for (int j = 0; j < pMesh->GetPolygonCount(); j++)
//			{
//				int iNumVertices = pMesh->GetPolygonSize(j);
//				assert(iNumVertices == 3);
//
//				for (int k = 0; k < iNumVertices; k++)
//				{
//					int iControlPointIndex = pMesh->GetPolygonVertex(j, k);
//
//					DirectX::XMFLOAT4 vertex;
//					vertex.x = (float)pVertices[iControlPointIndex].mData[0];
//					vertex.y = (float)pVertices[iControlPointIndex].mData[1];
//					vertex.z = (float)pVertices[iControlPointIndex].mData[2];
//					vertex.w = 0.0f;
//					pOutVertexVector->push_back(vertex);
//				}
//			}
//
//		}
//
//	}
//	return S_OK;
//}

void InitializeSdkObjects(FbxManager*& pManager, FbxScene*& pScene)
{
	//The first thing to do is to create the FBX Manager which is the object allocator for almost all the classes in the SDK
	pManager = FbxManager::Create();
	if (!pManager)
	{
		FBXSDK_printf("Error: Unable to create FBX Manager!\n");
		exit(1);
	}
	else FBXSDK_printf("Autodesk FBX SDK version %s\n", pManager->GetVersion());

	//Create an IOSettings object. This object holds all import/export settings.
	FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
	pManager->SetIOSettings(ios);

	//Load plugins from the executable directory (optional)
	FbxString lPath = FbxGetApplicationDirectory();
	pManager->LoadPluginsDirectory(lPath.Buffer());

	//Create an FBX scene. This object holds most objects imported/exported from/to files.
	pScene = FbxScene::Create(pManager, "My Scene");
	if (!pScene)
	{
		FBXSDK_printf("Error: Unable to create FBX scene!\n");
		exit(1);
	}
}

void DestroySdkObjects(FbxManager* pManager, bool pExitStatus)
{
	//Delete the FBX Manager. All the objects that have been allocated using the FBX Manager and that haven't been explicitly destroyed are also automatically destroyed.
	if (pManager) pManager->Destroy();
	if (pExitStatus) FBXSDK_printf("Program Success!\n");
}

std::vector<MyMesh> GetMyShit(FbxScene* fsce)
{
	std::vector<MyMesh> mesh;
	FbxNode* pFbxRootNode = fsce->GetRootNode();

	if (pFbxRootNode)
	{
		for (int i = 0; i < pFbxRootNode->GetChildCount(); i++)
		{
			FbxNode* pFbxChildNode = pFbxRootNode->GetChild(i);

			if (pFbxChildNode->GetNodeAttribute() == NULL)
				continue;

			FbxNodeAttribute::EType AttributeType = pFbxChildNode->GetNodeAttribute()->GetAttributeType();

			if (AttributeType != FbxNodeAttribute::eMesh)
				continue;

			FbxMesh* pMesh = (FbxMesh*)pFbxChildNode->GetNodeAttribute();

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
					float uvs[3] = { 0.0f, 0.0f, 0.0f };

					MyMesh temp = { vertex, norm, uvs };
					mesh.push_back(temp);
				}
			}
		}
	}
	return mesh;
}

std::vector<MyMesh> LoadScene(const char* pFilename)
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
		std::vector<MyMesh> invalidvec;
		return invalidvec;
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
	
	std::vector<MyMesh> temp = GetMyShit(pScene);
	DestroySdkObjects(pManager, true);
	return temp;
}
