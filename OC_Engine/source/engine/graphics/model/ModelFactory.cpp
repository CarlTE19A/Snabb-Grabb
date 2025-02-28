#include "stdafx.h"

#include "ModelFactory.h"
#include "../GraphicsEngine.h"
#include "Mesh.h"
#include "assimp/mesh.h"
#include "AnimatedMesh.h"
#include "graphics\Animation\Animation.h"
#include "../Animation/AnimationPlayer.h"
#include "../Animation/Skeleton.h"
#include <filesystem>

#include "../../external/assimp/Importer.hpp"
#include "../../external/assimp/postprocess.h"
#include "../../external/assimp/scene.h"

#include "../FBXSDK/include/fbxsdk.h"

#include "PhysX\foundation\PxFoundation.h"

#include "PhysX/geometry/PxConvexMesh.h"
#include "PhysX/geometry/PxTriangleMesh.h"
//#include "PhysX/geometry/PxConvexMeshGeometry.h"
//#include "PhysX/cooking/PxCooking.h"

#include "mainSingelton\MainSingelton.h"
#include "PhysXCode\PhysXManager.h"


void ModelFactory::Process(
	const std::string& aFileName,
	const aiNode* node,
	const aiScene* scene,
	std::vector<Mesh*>& aMeshVector,
	const std::string aNodeName
)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (aNodeName != "None")	//If is "None" then we load
		{
			if (aNodeName != node->mName.C_Str())	//If its not "None" and the name is not the same as the node name then we skip
			{
				//LogWarning("Node name is not the same as the node name we are looking for");
				continue;
			}
		}
		//LogInfo(aFileName + " | Mesh Name: " + mesh->mName.C_Str() + " | Node Name: " + node->mName.C_Str());

		std::vector<MeshVertex> vertices;
		std::vector<unsigned int> indices;

		if (!mesh->HasTangentsAndBitangents())
		{
			LogError("Mesh does not have tangents and bitangents: " + (std::string)mesh->mName.C_Str());
			return;
		}

		//process vertices
		for (unsigned int j = 0; j < mesh->mNumVertices; j++)
		{

			const aiVector3D& position = mesh->mVertices[j];
			const aiVector3D& uv = mesh->mTextureCoords[0][j];
			const aiVector3D& normal = mesh->mNormals[j];
			const aiVector3D& bitangent = mesh->mBitangents[j];
			const aiVector3D& tangent = mesh->mTangents[j];


			MeshVertex vertex;
			vertex.position.x = position.x;
			vertex.position.y = position.y;
			vertex.position.z = position.z;
			vertex.position.w = 1.0f;

			vertex.normal.x = normal.x;
			vertex.normal.y = normal.y;
			vertex.normal.z = normal.z;
			//vertex.normal._ = 1.0f;

			vertex.binormal.x = bitangent.x;
			vertex.binormal.y = bitangent.y;
			vertex.binormal.z = bitangent.z;


			vertex.tangent.x = tangent.x;
			vertex.tangent.y = tangent.y;
			vertex.tangent.z = tangent.z;

			vertex.texCoord.x = uv.x;
			vertex.texCoord.y = uv.y;

			vertex.color.x = 1.0f;
			vertex.color.y = 1.0f;
			vertex.color.z = 1.0f;
			vertex.color.w = 1.0f;

			vertices.push_back(vertex);
		}

		//process indices

		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			const aiFace& face = mesh->mFaces[j];

			for (unsigned int k = 0; k < face.mNumIndices; k++)
			{
				indices.push_back(face.mIndices[k]);
			}
		}

		Mesh* newMesh = new Mesh();

		//std::vector<aiString>

		//std::string materialName = (std::string)scene->mMaterials[i]->GetName().C_Str();

		std::string materialName = (std::string)scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();

		if (!newMesh->Initialize(vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size(), aFileName, materialName, myMaterialCache))
		{
			LogError("Failed to Initialize" + aFileName);
			delete newMesh;
			newMesh = nullptr;
			return;
		}

		aMeshVector.push_back(newMesh);
	}

	//Process child nodes recursively
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		Process(aFileName, node->mChildren[i], scene, aMeshVector, aNodeName);
	}
}

bool ModelFactory::Process(const std::string& aFileName, const aiNode* node, const aiScene* scene, std::vector<physx::PxTriangleMesh*>& aTriangleMeshes,
	std::vector<physx::PxConvexMesh*>& convexMeshes, const std::string aNodeName, bool isConvex)
{

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		if (aNodeName != "None")	//If is "None" then we load
		{
			if (aNodeName != node->mName.C_Str())	//If its not "None" and the name is not the same as the node name then we skip
			{
				//LogWarning("Node name is not the same as the node name we are looking for");
				continue;
			}
		}
		//LogInfo(aFileName + " | Mesh Name: " + mesh->mName.C_Str() + " | Node Name: " + node->mName.C_Str());

		std::vector<MeshVertex> vertices;
		std::vector<unsigned int> indices;

		if (!mesh->HasTangentsAndBitangents())
		{
			LogError("PhysX mesh does not have tangents and bitangents: " + (std::string)mesh->mName.C_Str());
			return false;
		}

		//process vertices
		for (unsigned int j = 0; j < mesh->mNumVertices; j++)
		{

			const aiVector3D& position = mesh->mVertices[j];
			const aiVector3D& uv = mesh->mTextureCoords[0][j];
			const aiVector3D& normal = mesh->mNormals[j];
			const aiVector3D& bitangent = mesh->mBitangents[j];
			const aiVector3D& tangent = mesh->mTangents[j];


			MeshVertex vertex;
			vertex.position.x = position.x;
			vertex.position.y = position.y;
			vertex.position.z = position.z;
			vertex.position.w = 1.0f;

			vertex.normal.x = normal.x;
			vertex.normal.y = normal.y;
			vertex.normal.z = normal.z;
			//vertex.normal._ = 1.0f;


			vertex.binormal.x = bitangent.x;
			vertex.binormal.y = bitangent.y;
			vertex.binormal.z = bitangent.z;


			vertex.tangent.x = tangent.x;
			vertex.tangent.y = tangent.y;
			vertex.tangent.z = tangent.z;

			vertex.texCoord.x = uv.x;
			vertex.texCoord.y = uv.y;


			vertex.color.x = 1.0f;
			vertex.color.y = 1.0f;
			vertex.color.z = 1.0f;
			vertex.color.w = 1.0f;

			vertices.push_back(vertex);
		}

		//process indices

		for (unsigned int j = 0; j < mesh->mNumFaces; j++)
		{
			const aiFace& face = mesh->mFaces[j];

			for (unsigned int k = 0; k < face.mNumIndices; k++)
			{
				indices.push_back(face.mIndices[k]);
			}
		}

		if (isConvex)
		{
			physx::PxConvexMeshDesc convexMeshDesc;
			convexMeshDesc.flags = physx::PxConvexFlag::eCOMPUTE_CONVEX;

			convexMeshDesc.points.count = mesh->mNumVertices;
			convexMeshDesc.points.stride = sizeof(MeshVertex);
			convexMeshDesc.points.data = vertices.data();

			convexMeshDesc.indices.count = (int)indices.size();
			convexMeshDesc.indices.stride = sizeof(unsigned int);
			convexMeshDesc.indices.data = indices.data();

			physx::PxDefaultMemoryOutputStream writeBuffer;
			physx::PxConvexMeshCookingResult::Enum result;

			physx::PxCookingParams cookingParams = MainSingleton::GetInstance().GetPhysXManager().GetPxCookingParams();
			if (!PxCookConvexMesh(cookingParams, convexMeshDesc, writeBuffer, &result))
			{
				LogError("Failed to cook convex mesh, you stupid");
			}
			else
			{
				//LogSuccess("you not stupid");
				physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
				physx::PxConvexMesh* convexMesh = MainSingleton::GetInstance().GetPhysXManager().GetPhysics()->createConvexMesh(readBuffer);
				convexMeshes.push_back(convexMesh);
			}
		}
		else
		{
			physx::PxTriangleMeshDesc triangleMeshDesc;

			triangleMeshDesc.points.count = (int)mesh->mNumVertices;
			triangleMeshDesc.points.stride = sizeof(MeshVertex);
			triangleMeshDesc.points.data = vertices.data();

			triangleMeshDesc.triangles.count = (int)indices.size() / 3;
			triangleMeshDesc.triangles.stride = 3 * sizeof(int);
			triangleMeshDesc.triangles.data = indices.data();

			physx::PxDefaultMemoryOutputStream writeBuffer;
			physx::PxTriangleMeshCookingResult::Enum result;


			physx::PxCookingParams& cookingParams = MainSingleton::GetInstance().GetPhysXManager().GetPxCookingParams();

			bool res = PxValidateTriangleMesh(cookingParams, triangleMeshDesc);

			PX_ASSERT(res);
			res;


			if (!PxCookTriangleMesh(cookingParams, triangleMeshDesc, writeBuffer, &result))
			{
				LogError("Failed to cook Triangle mesh, you very stupid -Carl 2024");
			}
			else
			{
				//LogSuccess("!(you not stupid)");
				physx::PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
				physx::PxTriangleMesh* trinagleMesh = MainSingleton::GetInstance().GetPhysXManager().GetPhysics()->createTriangleMesh(readBuffer);

				aTriangleMeshes.push_back(trinagleMesh);
			}
		}



		//std::vector<aiString>

		//std::string materialName = (std::string)scene->mMaterials[i]->GetName().C_Str();

	}

	//Process child nodes recursively
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		Process(aFileName, node->mChildren[i], scene, aTriangleMeshes, convexMeshes, aNodeName, isConvex);
		//if (i > 0)
		//{
		//	LogSuccess(std::to_string(i) + std::to_string(node->mChildren[i].));
		//}
	}

	return true;
}


bool ModelFactory::Process(const std::string& aFilePath, FBXMesh& aMesh)
{
	//if (anAnimatedMesh)
	FbxManager* lSdkManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, "OC");
	lSdkManager->SetIOSettings(ios);

	FbxImporter* fbxImporter = FbxImporter::Create(lSdkManager, "");

	std::string filePath = std::string(RELATIVE_ASSET_PATH) + aFilePath.c_str();
	bool lImportStatus = fbxImporter->Initialize(filePath.c_str(), -1, lSdkManager->GetIOSettings());
	if (!lImportStatus)
	{
		LogError("Failed to import skinned mesh in FBX importer stage!");
		return false;
	}

	FbxScene* fbxScene = FbxScene::Create(fbxImporter, "Scene");
	if (!fbxScene)
	{
		LogError("Failed to create scene when loading skinned mesh");
		return false;
	}

	size_t lastindex = aFilePath.find_last_of(".");
	std::string fileName = aFilePath.substr(0, lastindex);
	//FBXMesh loadMesh;
	if (fbxImporter->Import(fbxScene))
	{
		//auto sysUnit = fbxScene->GetGlobalSettings().GetSystemUnit();
		//if (sysUnit != FbxSystemUnit::cm)
		//{
		//	/*constexpr FbxSystemUnit::ConversionOptions sysUnitConversion =
		//	{
		//		false,
		//		true,
		//		true,
		//		true,
		//		true,
		//		true
		//	};*/

		//	//FbxSystemUnit::cm.ConvertScene(fbxScene, sysUnitConversion);
		//	//assert(fbxScene->GetGlobalSettings().GetSystemUnit() == FbxSystemUnit::cm);
		//}
		fbxScene->ConnectMaterials();
		FbxGeometryConverter* fbxConverter = new FbxGeometryConverter(lSdkManager);

		// Triangulate the scene and replace models with triangulated versions.
		if (!fbxConverter->Triangulate(fbxScene, true, true))
		{
			// Failed to use the faster legacy method so try to use the new method
			fbxConverter->Triangulate(fbxScene, true, false);
		}

		const unsigned int numSceneMaterials = fbxScene->GetMaterialCount();
		for (unsigned int i = 0; i < numSceneMaterials; i++)
		{
			FbxSurfaceMaterial* fbxMaterial = fbxScene->GetMaterial(i);
			aMesh.MaterialName.push_back(fbxMaterial->GetName());
		}
		//NOTE: Should gather LOD group too??
		std::vector<FbxNode*> meshNodes;
		meshNodes.reserve(numSceneMaterials);
		GatherMeshNodes(fbxScene->GetRootNode(), meshNodes);

		std::vector<FbxNode*> sceneLODGroups;


		// We need to make sure all models have proper BiNormal / Tangent data.
		for (FbxNode* meshNode : meshNodes)
		{
			FbxMesh* fbxMesh = meshNode->GetMesh();

			const int numUvLayers = fbxMesh->GetUVLayerCount();
			if (numUvLayers == 0)
			{
				std::throw_with_nested(std::runtime_error("Cannot load FBX file! No UV layers are present in the file."));
			}

			if (fbxMesh->GetElementBinormalCount() == 0 || fbxMesh->GetElementTangentCount() == 0)
			{
				if (!fbxMesh->GenerateTangentsData(0, false, false))
				{
					const FbxStatus& Status = fbxImporter->GetStatus();
					//const bool StatusError = Status.Error();
					std::throw_with_nested(std::runtime_error("Failed to generate Tangent/BiNormal data for the mesh " + std::string(fbxMesh->GetName()) + "! FBX Error was: " + Status.GetErrorString()));
				}
			}
		}

		FbxAxisSystem fbxAxisSystem;
		fbxAxisSystem = FbxAxisSystem::eDirectX;
		fbxAxisSystem.DeepConvertScene(fbxScene);

		std::vector<FBXMesh::LODGroup> mdlLodGroups;
		GatherLODGroups(fbxScene->GetRootNode(), sceneLODGroups);

		FBXSkeleton mdlSkeleton;
		std::vector<FBXSkeleton::FBXBone> skeletonEventBones;
		std::vector<FBXSkeleton::FBXSocket> skeletonSockets;
		const bool fbxHasBones = GatherSkeleton(fbxScene->GetRootNode(), mdlSkeleton, skeletonEventBones, skeletonSockets, -1);
		if (fbxHasBones)
		{
			// Try to extract a sensible skeleton name.
			if (const FBXSkeleton::FBXBone* rootBone = mdlSkeleton.GetRoot())
			{
				const std::string rootBoneName = rootBone->NamespaceName;
				if (const size_t pos = rootBoneName.find_first_of(':'); pos != std::string::npos)
				{
					mdlSkeleton.Name = rootBoneName.substr(0, pos);
				}
				else
				{
					// We have no namespace to use so we'll just use the model name.
					std::string filePathCopy = fileName;
					std::replace(filePathCopy.begin(), filePathCopy.end(), '/', '\\');
					if (const size_t slashPos = filePathCopy.find_last_of('\\'); slashPos != std::string::npos)
					{
						filePathCopy = filePathCopy.substr(slashPos + 1, filePathCopy.length() - (slashPos - 1));
					}
					mdlSkeleton.Name = filePathCopy.substr(0, filePathCopy.length());
				}
			}


			for (FBXSkeleton::FBXSocket& socket : skeletonSockets)
			{
				mdlSkeleton.Sockets.insert({ socket.Name, socket });
			}
		}

		for (FbxNode* lodGroupNode : sceneLODGroups)
		{
			FBXMesh::LODGroup lodGroup;

			// Go through all LOD groups and process meshes found there, and make LOD structs of them.
			// Remove the node from the sceneMeshNodes list.
			// Any meshes left in that list need to be handled separately.
			FbxLODGroup* fbxLodGroup = lodGroupNode->GetLodGroup();
			const int numLodLevels = lodGroupNode->GetChildCount();

			for (int lodLv = 0; lodLv < numLodLevels; lodLv++)
			{
				FBXMesh::LODGroup::LODLevel lodLevel;
				lodLevel.Level = lodLv;

				FbxDistance lodDistance;
				if (lodLv != 0)
				{
					fbxLodGroup->GetThreshold(lodLv - 1, lodDistance);
				}

				FbxNode* lodLevelRoot = lodGroupNode->GetChild(lodLv);

				lodLevel.Distance = lodDistance.value();

				std::vector<FbxNode*> lodLevelMeshNodes;

				const size_t numLodLevelMeshes = GatherMeshNodes(lodLevelRoot, lodLevelMeshNodes);
				lodLevel.Elements.reserve(numLodLevelMeshes);
				for (FbxNode* lodLevelMeshNode : lodLevelMeshNodes)
				{
					for (auto it = meshNodes.begin(); it != meshNodes.end(); ++it)
					{
						if ((*it)->GetUniqueID() == lodLevelMeshNode->GetUniqueID())
						{
							meshNodes.erase(it);
							break;
						}
					}

					// Convert the mesh here.
					std::vector<FBXMesh::Element> lodLevelNodeMeshes;
					FbxMeshToMesh(fbxScene, lodLevelMeshNode, mdlSkeleton, lodLevelNodeMeshes);

					for (auto meshIt = lodLevelNodeMeshes.begin(); meshIt != lodLevelNodeMeshes.end(); ++meshIt)
					{
						/*const BoxSphereBounds& bounds = meshIt->BoxSphereBounds;
						mdlBounds = mdlBounds + bounds;*/
						lodLevel.Elements.push_back(*meshIt);
					}
				}

				lodGroup.Levels.push_back(lodLevel);
			}

			mdlLodGroups.push_back(lodGroup);
		}

		// Handle any meshes not part of a LOD group
		std::vector<FBXMesh::Element> mdlMeshes;

		for (FbxNode* meshNode : meshNodes)
		{
			std::vector<FBXMesh::Element> nodeMeshes;
			FbxMeshToMesh(fbxScene, meshNode, mdlSkeleton, nodeMeshes);

			/*for (auto meshIt = nodeMeshes.begin(); meshIt != nodeMeshes.end(); ++meshIt)
			{
				const BoxSphereBounds& bounds = meshIt->BoxSphereBounds;
				mdlBounds = mdlBounds + bounds;
			}*/

			mdlMeshes.insert(mdlMeshes.end(), nodeMeshes.begin(), nodeMeshes.end());
		}

		if (!mdlMeshes.empty() || !mdlLodGroups.empty())
		{

			aMesh.Name = fileName;
			aMesh.Elements = std::move(mdlMeshes);
			aMesh.LODGroups = std::move(mdlLodGroups);

			aMesh.Skeleton = std::move(mdlSkeleton);

			//fbxMesh.Materials = std::move(mdlMaterials);
			/*fbxMesh.BoxSphereBounds = mdlBounds;
			fbxMesh.BoxBounds = Box::FromAABB(mdlBounds.Center, mdlBounds.BoxExtents);*/
		}
	}

	fbxScene->Destroy(false);
	fbxImporter->Destroy();
	return true;
}

void ModelFactory::LoadMaterial(const std::string aMaterialPath)
{
	size_t lastindex = 0;
	size_t firstindex = 0;
	std::string materialname = "";
	Texture tex;
	tex.Initialize(TextureInitializationType::DDS, STATIC_ASSET_PATH + (std::string)"EditorAssets/Missing_Texture.dds", false);
	myMaterialCache.insert({ "missing_texture", tex });
	for (const auto& entry : std::filesystem::directory_iterator(aMaterialPath))
	{
		if (entry.exists())
		{
			//it did not like .meta so we jump those
			if (entry.path().extension() == ".meta") continue;

			//check subfolders for material
			for (const auto& subEntry : std::filesystem::directory_iterator(entry))
			{
				std::filesystem::path path = subEntry.path();
				if (path.extension() == ".dds")
				{
					lastindex = path.string().find_last_of(".");
					firstindex = path.string().find_last_of("\\");
					materialname = path.string().substr(firstindex + 1, lastindex - firstindex - 1);
					std::transform(materialname.begin(), materialname.end(), materialname.begin(),
						[](unsigned char c) { return std::tolower(c); });
					//check if material already exists
					auto item = myMaterialCache.find(path.string());
					if (item != myMaterialCache.end())
					{
						continue;
					}

					if (!tex.Initialize(TextureInitializationType::DDS, path.string(), false))
					{
						LogError("Failed to init texture: " + materialname);
						continue;
					}
					myMaterialCache.insert({ materialname, tex });
				}
			}
			//check for material in material folder
			if (entry.path().extension() == ".dds")
			{
				std::filesystem::path path = entry.path();
				if (path.extension() == ".dds")
				{
					lastindex = path.string().find_last_of(".");
					firstindex = path.string().find_last_of("\\");
					materialname = path.string().substr(firstindex + 1, lastindex - firstindex - 1);
					//check if material already exists
					auto item = myMaterialCache.find(path.string());
					if (item != myMaterialCache.end())
					{
						continue;
					}

					if (!tex.Initialize(TextureInitializationType::DDS, path.string(), false))
					{
						LogError("Failed to init texture: " + materialname);
						continue;
					}
					myMaterialCache.insert({ materialname, tex });
				}
			}
		}
	}
}


bool ModelFactory::LoadTriangleMeshData(std::string aFilePath, bool& staticPath, bool& isConvex, std::string aNodeName)
{
	std::string cacheName = aFilePath + "-" + aNodeName;
	//Use cache
	//auto item = myMeshCache.find(cacheName);
	//if (item != myMeshCache.end())
	//{
	//	return item->second;
	//}

	size_t lastindex = aFilePath.find_last_of(".");
	std::string fileName = aFilePath.substr(0, lastindex);
	if (!staticPath)
		aFilePath = std::string(RELATIVE_ASSET_PATH) + aFilePath;

	std::vector<Mesh*> meshVector;
	std::vector<physx::PxConvexMesh*> convexMeshes;
	std::vector<physx::PxTriangleMesh*> triangleMeshes;

	Assimp::Importer importer;

	const aiScene* scene = importer.ReadFile(aFilePath,
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |  //Maybe should be removed
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_FindInvalidData |
		aiProcess_OptimizeMeshes |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PopulateArmatureData);


	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LogWarning("Failed to load model at path: " + aFilePath + " | " + importer.GetErrorString());
		return false;
	}

	Process(fileName, scene->mRootNode, scene, triangleMeshes, convexMeshes, aNodeName, isConvex);

	if (convexMeshes.size() > 0)
	{
		myConvexMeshCache.insert({ cacheName ,convexMeshes });
	}

	if (triangleMeshes.size() > 0)
	{
		myTriangleMeshCache.insert({ cacheName, triangleMeshes });
	}

	return true;
}

std::vector<Mesh*> ModelFactory::Load(std::string aFilePath, std::string aNodeName, bool staticPath)
{
	std::string cacheName = aFilePath + "-" + aNodeName;
	//Use cache
	auto item = myMeshCache.find(cacheName);
	if (item != myMeshCache.end())
	{
		return item->second;
	}

	size_t lastindex = aFilePath.find_last_of(".");
	std::string fileName = aFilePath.substr(0, lastindex);
	if (!staticPath)
		aFilePath = std::string(RELATIVE_ASSET_PATH) + aFilePath;

	std::vector<Mesh*> meshVector;
	std::vector<physx::PxConvexMesh*> convexMeshes;

	Assimp::Importer importer;
	//aiProcessPreset_TargetRealtime_Fast (aiProcess_CalcTangentSpace | aiProcess_GenNormals | aiProcess_JoinIdenticalVertices | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType | 0);
	//aiProcessPreset_TargetRealtime_Quality == (aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights | aiProcess_RemoveRedundantMaterials | aiProcess_SplitLargeMeshes | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType | aiProcess_FindDegenerates | aiProcess_FindInvalidData | 0);
	//aiProcessPreset_TargetRealtime_MaxQuality == ((aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices | aiProcess_ImproveCacheLocality | aiProcess_LimitBoneWeights | aiProcess_RemoveRedundantMaterials | aiProcess_SplitLargeMeshes | aiProcess_Triangulate | aiProcess_GenUVCoords | aiProcess_SortByPType | aiProcess_FindDegenerates | aiProcess_FindInvalidData | 0) | aiProcess_FindInstances | aiProcess_ValidateDataStructure | aiProcess_OptimizeMeshes | 0);
	const aiScene* scene = importer.ReadFile(aFilePath,
		aiProcess_CalcTangentSpace |
		aiProcess_GenSmoothNormals |
		aiProcess_JoinIdenticalVertices |
		aiProcess_LimitBoneWeights |
		aiProcess_SplitLargeMeshes |  //Maybe should be removed
		aiProcess_Triangulate |
		aiProcess_GenUVCoords |
		aiProcess_FindInvalidData |
		aiProcess_OptimizeMeshes |
		aiProcess_ConvertToLeftHanded |
		aiProcess_PopulateArmatureData);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		LogWarning("Failed to load model at path: " + aFilePath + " | " + importer.GetErrorString());
		return meshVector;
	}

	Process(fileName, scene->mRootNode, scene, meshVector, aNodeName);
	myMeshCache.insert({ cacheName, meshVector });

	return meshVector;
}

std::vector<AnimatedMesh*> ModelFactory::Load(std::string aFilePath, const bool /*anAnimatedMesh*/)
{
	FBXMesh myMesh;
	std::vector<AnimatedMesh*> meshVector;
	//Use cache
	std::string path = aFilePath;
	auto item = myAnimatedMeshCache.find(path);
	if (item != myAnimatedMeshCache.end())
	{
		myMesh = item->second;
	}
	else
	{
		if (!Process(aFilePath, myMesh))
		{
			return meshVector;
		}
		myAnimatedMeshCache.insert({ path, myMesh });
	}



	Skeleton skeleton;
	skeleton.Name = myMesh.Skeleton.Name;
	//size_t size = loadMesh.LODGroups.size();
	//size;
	//skeleton.BoneNameToIndex = loadMesh.Skeleton.BoneNameToIndex;
	for (int i = 0; i < myMesh.Skeleton.Bones.size(); i++)
	{
		Skeleton::Bone bone;
		bone.Name = myMesh.Skeleton.Bones[i].Name;
		bone.Parent = myMesh.Skeleton.Bones[i].ParentIdx;
		bone.BindPoseInverse = myMesh.Skeleton.Bones[i].BindPoseInverse;
		for (int j = 0; j < myMesh.Skeleton.Bones[i].Children.size(); j++)
		{
			bone.Children.push_back(myMesh.Skeleton.Bones[i].Children[j]);
		}
		skeleton.BoneName.push_back(bone.Name);
		skeleton.Bones.push_back(bone);
		skeleton.BoneNameToIndex.insert({ bone.Name, i });
	}

	for (int i = 0; i < myMesh.Elements.size(); i++)
	{
		std::vector<AnimatedMeshVertex> vertices;
		std::vector<unsigned int> indices;
		AnimatedMesh* animMesh = new AnimatedMesh();
		for (int j = 0; j < myMesh.Elements[i].Vertices.size(); j++)
		{
			AnimatedMeshVertex vertex;
			vertex.position.x = myMesh.Elements[i].Vertices[j].Position[0];
			vertex.position.y = myMesh.Elements[i].Vertices[j].Position[1];
			vertex.position.z = myMesh.Elements[i].Vertices[j].Position[2];
			vertex.position.w = 1.0f;

			vertex.normal.x = myMesh.Elements[i].Vertices[j].Normal[0];
			vertex.normal.y = myMesh.Elements[i].Vertices[j].Normal[1];
			vertex.normal.z = myMesh.Elements[i].Vertices[j].Normal[2];


			vertex.binormal.x = myMesh.Elements[i].Vertices[j].BiNormal[0];
			vertex.binormal.y = myMesh.Elements[i].Vertices[j].BiNormal[1];
			vertex.binormal.z = myMesh.Elements[i].Vertices[j].BiNormal[2];


			vertex.tangent.x = myMesh.Elements[i].Vertices[j].Tangent[0];
			vertex.tangent.y = myMesh.Elements[i].Vertices[j].Tangent[1];
			vertex.tangent.z = myMesh.Elements[i].Vertices[j].Tangent[2];

			vertex.texCoord.x = myMesh.Elements[i].Vertices[j].UVs[0][0];
			vertex.texCoord.y = myMesh.Elements[i].Vertices[j].UVs[0][1];

			vertex.BoneIDs[0] = (float)myMesh.Elements[i].Vertices[j].BoneIDs[0];
			vertex.BoneIDs[1] = (float)myMesh.Elements[i].Vertices[j].BoneIDs[1];
			vertex.BoneIDs[2] = (float)myMesh.Elements[i].Vertices[j].BoneIDs[2];
			vertex.BoneIDs[3] = (float)myMesh.Elements[i].Vertices[j].BoneIDs[3];

			vertex.BoneWeights[0] = myMesh.Elements[i].Vertices[j].BoneWeights[0];
			vertex.BoneWeights[1] = myMesh.Elements[i].Vertices[j].BoneWeights[1];
			vertex.BoneWeights[2] = myMesh.Elements[i].Vertices[j].BoneWeights[2];
			vertex.BoneWeights[3] = myMesh.Elements[i].Vertices[j].BoneWeights[3];

			vertex.color.x = 1.0f;
			vertex.color.y = 1.0f;
			vertex.color.z = 1.0f;
			vertex.color.w = 1.0f;

			vertices.push_back(vertex);
		}

		for (int j = 0; j < myMesh.Elements[i].Indices.size(); j++)
		{
			indices.push_back(myMesh.Elements[i].Indices[j]);
		}
		std::string materialName = myMesh.MaterialName[myMesh.Elements[i].MaterialIndex];

		if (skeleton.Bones.size() > 0)
		{
			animMesh->SetSkeleton(skeleton);
		}

		if (!animMesh->Initialize(vertices.data(), (int)vertices.size(), indices.data(), (int)indices.size(), path, materialName, myMaterialCache))
		{
			delete animMesh;
			animMesh = nullptr;
			return meshVector;
		}
		meshVector.push_back(animMesh);
	}

	return meshVector;
}

std::vector<physx::PxConvexMesh*> ModelFactory::GetConvexPhysxMesh(std::string aCasheName)
{
	auto item = myConvexMeshCache.find(aCasheName);
	if (item != myConvexMeshCache.end())
	{
		return item->second;
	}

	return std::vector<physx::PxConvexMesh*>();
}

std::vector<physx::PxTriangleMesh*> ModelFactory::GetTrianglePhysxMesh(std::string aCasheName)
{
	auto item = myTriangleMeshCache.find(aCasheName);
	if (item != myTriangleMeshCache.end())
	{
		return item->second;
	}
	return std::vector<physx::PxTriangleMesh*>();
}

size_t ModelFactory::GatherMeshNodes(FbxNode* aRootNode, std::vector<FbxNode*>& someSceneMeshNodes)
{
	// For each child of our current node...
	for (int i = 0; i < aRootNode->GetChildCount(); i++)
	{
		// Get the node attribute if it has one, the data that denotes what type this node is.
		// If there is no type it's usually just an organizational node (folder).
		FbxNode* childNode = aRootNode->GetChild(i);
		const FbxNodeAttribute* childAttribute = childNode->GetNodeAttribute();
		if (!childAttribute)
		{
			// Even if it's not a valid node, step down since it may contain meshes.
			GatherMeshNodes(childNode, someSceneMeshNodes);
		}
		else
		{
			const FbxNodeAttribute::EType childAttributeType = childAttribute->GetAttributeType();

			// Check if this is a mesh node.			        
			if (childAttributeType == FbxNodeAttribute::eMesh)
			{
				// If it is, gather the node.
				someSceneMeshNodes.push_back(childNode);
			}

			GatherMeshNodes(childNode, someSceneMeshNodes);
		}
	}

	// Good idea to return how many meshes we found. Might be useful later.
	return static_cast<unsigned int>(someSceneMeshNodes.size());
}

size_t ModelFactory::GatherLODGroups(FbxNode* aRootNode, std::vector<FbxNode*>& outNodes)
{
	for (int i = 0; i < aRootNode->GetChildCount(); i++)
	{
		FbxNode* childNode = aRootNode->GetChild(i);
		const FbxNodeAttribute* childAttribute = childNode->GetNodeAttribute();
		if (!childAttribute)
		{
			GatherLODGroups(childNode, outNodes);
		}
		else
		{
			const FbxNodeAttribute::EType childAttributeType = childAttribute->GetAttributeType();

			if (childAttributeType == FbxNodeAttribute::eLODGroup)
			{
				outNodes.push_back(childNode);
			}
			else // We only want to find root-level LOD groups. I.e. not those below another LODGroup
			{
				GatherLODGroups(childNode, outNodes);
			}
		}
	}

	return static_cast<unsigned int>(outNodes.size());
}

bool ModelFactory::FbxMeshToMesh(fbxsdk::FbxScene* aScene, fbxsdk::FbxNode* aMeshNode, FBXSkeleton& aSkeleton, std::vector<FBXMesh::Element>& outMeshes)
{
	const unsigned int numMeshMaterials = aMeshNode->GetMaterialCount();
	outMeshes.reserve(numMeshMaterials);

	std::unordered_multimap<unsigned int, std::pair<size_t, float>> ControlPointWeights;
	float maxExtents[3] = { 0, 0, 0 };
	float minExtents[3] = { 0, 0, 0 };

	FbxMesh* fbxMesh = aMeshNode->GetMesh();

	if (aSkeleton.GetRoot())
	{
		const FbxVector4 fbxTranslation = aMeshNode->GetGeometricTranslation(FbxNode::eSourcePivot);
		const FbxVector4 fbxRotation = aMeshNode->GetGeometricRotation(FbxNode::eSourcePivot);
		const FbxVector4 fbxScale = aMeshNode->GetGeometricScaling(FbxNode::eSourcePivot);
		const fbxsdk::FbxAMatrix rootTransform = FbxAMatrix(fbxTranslation, fbxRotation, fbxScale);

		for (int deformIdx = 0; deformIdx < fbxMesh->GetDeformerCount(); deformIdx++)
		{
			FbxSkin* fbxSkin = reinterpret_cast<FbxSkin*>(fbxMesh->GetDeformer(deformIdx, FbxDeformer::eSkin));

			// If it's not a skin keep looking.
			if (!fbxSkin)
				continue;

			for (int clusterIdx = 0; clusterIdx < fbxSkin->GetClusterCount(); clusterIdx++)
			{
				// So we go through each cluster.
				FbxCluster* fbxCluster = fbxSkin->GetCluster(clusterIdx);

				// We get the transform of the cluster that was present at skin binding time.
				// This is the "resting pose" if you like.
				fbxsdk::FbxAMatrix meshBindTransform;
				fbxCluster->GetTransformMatrix(meshBindTransform);

				// We also need the link transform. In the case of a Skin it will be the
				// transform to go from local Joint Space to World Space.
				fbxsdk::FbxAMatrix linkTransform;
				fbxCluster->GetTransformLinkMatrix(linkTransform);

				// And finally assemble the Bind Pose Transform.
				// This is the default position of the vertices.
				fbxsdk::FbxAMatrix bindPoseInverseTransform = linkTransform.Inverse() * meshBindTransform * rootTransform;
				// Remember to do this or you will get very interesting results :).
				bindPoseInverseTransform = bindPoseInverseTransform.Transpose();

				// The Link in the skin cluster is the actual joint.
				// Because we already collected all joints we should be able to find it using
				// the acceleration map in the Skeleton.

				std::string jointName = fbxCluster->GetLink()->GetName();
				if (size_t namePos = jointName.find_last_of(':'); namePos != std::string::npos)
				{
					jointName = jointName.substr(namePos + 1);
				}
				size_t jointIndex = aSkeleton.BoneNameToIndex[jointName];

				// Store the bind pose on the joint so we can access it later.
				aSkeleton.Bones[jointIndex].BindPoseInverse = FBXMatrixToDXMatrix(bindPoseInverseTransform);

				// Here comes some more control point stuff.
				// We need to collect all the control points that this skin cluster affects.
				// And for those we need to store which joint affects it and its weights.
				for (int i = 0; i < fbxCluster->GetControlPointIndicesCount(); i++)
				{
					const unsigned int c = fbxCluster->GetControlPointIndices()[i];
					const float w = static_cast<float>(fbxCluster->GetControlPointWeights()[i]);
					// This inserts into a multimap.
					// c - control point index.
					// jointIndex - self explanatory.
					// w - the weight for this joint.
					ControlPointWeights.insert({ c, { jointIndex, w } });
				}
			}
		}
	}

	FbxAMatrix tempmatrix = aMeshNode->EvaluateGlobalTransform();
	FbxTransform tempxform = aMeshNode->GetTransform();

	// If we have materials we may need to cut this mesh up into multiple.
	const bool bHasMaterials = aMeshNode->GetMaterialCount() != 0;
	FbxSurfaceMaterial* currentSceneMaterial = nullptr;

	// We need to load per material index since each mesh can only have one material when rendering.
	// This means that if it has more than one they will be separated into two meshes.
	for (int meshMaterialIndex = 0; meshMaterialIndex < aMeshNode->GetMaterialCount() || meshMaterialIndex == 0; meshMaterialIndex++)
	{
		FBXMesh::Element currentMeshData = {};
		currentMeshData.MeshName = aMeshNode->GetName();

		if (bHasMaterials)
		{
			for (int sceneMaterialIndex = 0; sceneMaterialIndex < aScene->GetMaterialCount(); sceneMaterialIndex++)
			{
				FbxSurfaceMaterial* sceneMaterial = aScene->GetMaterial(sceneMaterialIndex);
				FbxSurfaceMaterial* meshNodeMaterial = aMeshNode->GetMaterial(meshMaterialIndex);
				if (sceneMaterial == meshNodeMaterial)
				{
					currentSceneMaterial = sceneMaterial;
					currentMeshData.MaterialIndex = sceneMaterialIndex;
					break;
				}
			}
		}
		else
		{
			currentMeshData.MaterialIndex = 0;
		}

		FbxLayerElementMaterial* fbxElementMaterial = fbxMesh->GetElementMaterial();

		const int fbxMeshPolygonCount = fbxMesh->GetPolygonCount();

		currentMeshData.Vertices.reserve(static_cast<size_t>(fbxMeshPolygonCount) * 3ULL);
		currentMeshData.Indices.reserve(currentMeshData.Vertices.capacity());

		std::unordered_map<size_t, VertexDuplicateData> VertexDuplicateAccelMap;
		VertexDuplicateAccelMap.reserve(currentMeshData.Vertices.capacity());

		unsigned int IndexCounter = 0;
		for (int p = 0; p < fbxMeshPolygonCount; p++)
		{
			if (bHasMaterials)
			{
				// This is the index of the materials in the mesh element array.
				// It doesn't always correspond to the scene material list since the first
				// material here might be material n in the scene.
				const int IndexAtP = fbxElementMaterial->GetIndexArray().GetAt(p);
				FbxSurfaceMaterial* polygonMaterial = aMeshNode->GetMaterial(IndexAtP);
				if (currentSceneMaterial != polygonMaterial)
				{
					continue;
				}
			}

			for (int v = 0; v < 3; v++)
			{
				const int fbxControlPtIndex = fbxMesh->GetPolygonVertex(p, v);
				const FbxVector4 fbxVxPos = fbxMesh->GetControlPointAt(fbxControlPtIndex);
				FbxVector2 fbxVxUVs[4];
				const int fbxNumUVs = fbxMesh->GetElementUVCount();

				const int fbxTextureUVIndex = fbxMesh->GetTextureUVIndex(p, v);
				for (int uv = 0; uv < fbxNumUVs && uv < 4; uv++)
				{
					FbxGeometryElementUV* fbxUvElement = fbxMesh->GetElementUV(uv);
					fbxVxUVs[uv] = fbxUvElement->GetDirectArray().GetAt(fbxTextureUVIndex);
				}

				const int polygonIndex = p * 3 + v;

				FbxVector4 fbxNormal;
				FbxGeometryElementNormal* fbxNormalElement = fbxMesh->GetElementNormal(0);
				GetElementMappingData(fbxNormalElement, fbxControlPtIndex, polygonIndex, fbxNormal);

				FbxVector4 fbxTangent;
				FbxGeometryElementTangent* fbxTangentElement = fbxMesh->GetElementTangent(0);
				GetElementMappingData(fbxTangentElement, fbxControlPtIndex, polygonIndex, fbxTangent);

				FbxVector4 fbxBinormal;
				FbxGeometryElementBinormal* fbxBinormalElement = fbxMesh->GetElementBinormal(0);
				GetElementMappingData(fbxBinormalElement, fbxControlPtIndex, polygonIndex, fbxBinormal);

				// BUG: FBX SDK 2020 doesn't seem to handle vx color winding correctly in certain
				// circumstances. It seems to work in eByVertex-eDirect but not eIndexToDirect?
				// Needs more testing.

				int windCorrection;
				switch (polygonIndex)
				{
				case 1:
					windCorrection = 2;
					break;
				case 2:
					windCorrection = 1;
					break;
				default:
					windCorrection = polygonIndex;
					break;
				}

				//const int windedPolygonIndex = p * 3 + windCorrection;
				const int windedPolygonIndex = polygonIndex;

				FbxColor fbxColors[4];
				const int fbxNumVxColorChannels = fbxMesh->GetElementVertexColorCount();
				for (int col = 0; col < fbxNumVxColorChannels && col < 4; col++)
				{
					FbxGeometryElementVertexColor* colElement = fbxMesh->GetElementVertexColor(col);
					switch (colElement->GetMappingMode())
					{
					case FbxGeometryElement::eByControlPoint:
					{
						switch (fbxNormalElement->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						{
							fbxColors[col] = colElement->GetDirectArray().GetAt(fbxControlPtIndex);
						}
						break;
						case FbxGeometryElement::eIndexToDirect:
						{
							const int Idx = colElement->GetIndexArray().GetAt(fbxControlPtIndex);
							fbxColors[col] = colElement->GetDirectArray().GetAt(Idx);
						}
						break;
						default:
							throw std::exception("Invalid Reference Mode!");
						}
					}
					break;

					case FbxGeometryElement::eByPolygonVertex:
					{
						// DB 221006 Possible that this works as intended without having to use
						// the corrected winding? Needs testing.
						switch (colElement->GetReferenceMode())
						{
						case FbxGeometryElement::eDirect:
						{
							fbxColors[col] = colElement->GetDirectArray().GetAt(windedPolygonIndex);
						}
						break;
						case FbxGeometryElement::eIndexToDirect:
						{
							const int Idx = colElement->GetIndexArray().GetAt(windedPolygonIndex);
							fbxColors[col] = colElement->GetDirectArray().GetAt(Idx);
						}
						break;
						default:
							throw std::exception("Invalid Reference Mode!");
						}
					}
					break;
					}
				}

				FBXVertex vx;
				vx.Position[0] = static_cast<float>(fbxVxPos[0]);
				vx.Position[1] = static_cast<float>(fbxVxPos[1]);
				vx.Position[2] = static_cast<float>(fbxVxPos[2]);
				vx.Position[3] = static_cast<float>(fbxVxPos[3]);

				for (int uv = 0; uv < fbxNumUVs; uv++)
				{
					vx.UVs[uv][0] = static_cast<float>(fbxVxUVs[uv][0]);
					vx.UVs[uv][1] = 1 - static_cast<float>(fbxVxUVs[uv][1]);
				}

				vx.Normal[0] = static_cast<float>(fbxNormal[0]);
				vx.Normal[1] = static_cast<float>(fbxNormal[1]);
				vx.Normal[2] = static_cast<float>(fbxNormal[2]);

				vx.Tangent[0] = static_cast<float>(fbxTangent[0]);
				vx.Tangent[1] = static_cast<float>(fbxTangent[1]);
				vx.Tangent[2] = static_cast<float>(fbxTangent[2]);

				vx.BiNormal[0] = static_cast<float>(fbxBinormal[0]);
				vx.BiNormal[1] = static_cast<float>(fbxBinormal[1]);
				vx.BiNormal[2] = static_cast<float>(fbxBinormal[2]);

				for (int vxColSet = 0; vxColSet < fbxNumVxColorChannels; vxColSet++)
				{
					vx.VertexColors[vxColSet][0] = static_cast<float>(fbxColors[vxColSet][0]);
					vx.VertexColors[vxColSet][1] = static_cast<float>(fbxColors[vxColSet][1]);
					vx.VertexColors[vxColSet][2] = static_cast<float>(fbxColors[vxColSet][2]);
					vx.VertexColors[vxColSet][3] = static_cast<float>(fbxColors[vxColSet][3]);
				}

				if (vx.Position[0] > maxExtents[0])
					maxExtents[0] = vx.Position[0];
				if (vx.Position[1] > maxExtents[1])
					maxExtents[1] = vx.Position[1];
				if (vx.Position[2] > maxExtents[2])
					maxExtents[2] = vx.Position[2];

				if (vx.Position[0] < minExtents[0])
					minExtents[0] = vx.Position[0];
				if (vx.Position[1] < minExtents[1])
					minExtents[1] = vx.Position[1];
				if (vx.Position[2] < minExtents[2])
					minExtents[2] = vx.Position[2];

				unsigned int BoneIDs[] = { 0, 0, 0, 0 };
				float BoneWeights[] = { 0, 0, 0, 0 };

				if (aSkeleton.GetRoot())
				{
					// Since we're making a bit of a complex iteration we need to define the iterator.
					// It's a lot less to type that way.
					typedef std::unordered_multimap<unsigned int, std::pair<size_t, float>>::iterator MMIter;

					// Then we use equal range to get all the data stored for this specific control point.
					std::pair<MMIter, MMIter> values = ControlPointWeights.equal_range(fbxControlPtIndex);

					// This idx is to loop on the 4 indices of ID and Weight.
					int idx = 0;
					for (MMIter it = values.first; it != values.second && idx < 4; ++it)
					{
						std::pair<size_t, float> BoneAndWeight = it->second;
						BoneIDs[idx] = static_cast<unsigned>(BoneAndWeight.first);
						BoneWeights[idx] = BoneAndWeight.second;
						idx++;
					}
				}

				vx.BoneIDs[0] = BoneIDs[0];
				vx.BoneIDs[1] = BoneIDs[1];
				vx.BoneIDs[2] = BoneIDs[2];
				vx.BoneIDs[3] = BoneIDs[3];

				vx.BoneWeights[0] = BoneWeights[0];
				vx.BoneWeights[1] = BoneWeights[1];
				vx.BoneWeights[2] = BoneWeights[2];
				vx.BoneWeights[3] = BoneWeights[3];

				//if (bMergeDuplicateVertices)
				{
					// A drawback of using control points is that we MAY get duplicate vertices.
					// This means we'll need to compare and ensure that it is a unique vert.
					VertexHash Hasher;
					size_t hash = Hasher(vx);
					if (VertexDuplicateAccelMap.find(hash) == VertexDuplicateAccelMap.end())
					{
						VertexDuplicateAccelMap[hash] = { /*{ vx },*/ IndexCounter, hash };
						currentMeshData.Vertices.push_back(vx);
						currentMeshData.Indices.push_back(IndexCounter++);
					}
					else
					{
						currentMeshData.Indices.push_back(VertexDuplicateAccelMap[hash].Idx);
					}
				}
				/*else
				{
					currentMeshData.Vertices.push_back(vx);
					currentMeshData.Indices.push_back(IndexCounter++);
				}*/
			}
		}

		if (!currentMeshData.Vertices.empty())
		{
			/*float extentsCenter[3];
			extentsCenter[0] = 0.5f * (minExtents[0] + maxExtents[0]);
			extentsCenter[1] = 0.5f * (minExtents[1] + maxExtents[1]);
			extentsCenter[2] = 0.5f * (minExtents[2] + maxExtents[2]);

			float boxExtents[3];
			boxExtents[0] = 0.5f * (maxExtents[0] - minExtents[0]);
			boxExtents[1] = 0.5f * (maxExtents[1] - minExtents[1]);
			boxExtents[2] = 0.5f * (maxExtents[2] - minExtents[2]);

			float extentRadius = std::max(boxExtents[0], std::max(boxExtents[1], boxExtents[2]));*/

			/*currentMeshData.BoxSphereBounds = {
				{boxExtents[0], boxExtents[1], boxExtents[2]},
				{extentsCenter[0], extentsCenter[1], extentsCenter[2]},
				extentRadius
			};*/

			//currentMeshData.BoxBounds = Box::FromAABB(currentMeshData.BoxSphereBounds.Center, currentMeshData.BoxSphereBounds.BoxExtents);

			outMeshes.push_back(currentMeshData);
		}

		VertexDuplicateAccelMap.clear();

	}

	ControlPointWeights.clear();

	return true;
}

DirectX::XMMATRIX ModelFactory::FBXMatrixToDXMatrix(fbxsdk::FbxAMatrix& aMatrix)
{
	float matrixArray[16]
	{
		(float)aMatrix.GetColumn(0)[0], (float)aMatrix.GetColumn(0)[1], (float)aMatrix.GetColumn(0)[2], (float)aMatrix.GetColumn(0)[3],
		(float)aMatrix.GetColumn(1)[0], (float)aMatrix.GetColumn(1)[1], (float)aMatrix.GetColumn(1)[2], (float)aMatrix.GetColumn(1)[3],
		(float)aMatrix.GetColumn(2)[0], (float)aMatrix.GetColumn(2)[1], (float)aMatrix.GetColumn(2)[2], (float)aMatrix.GetColumn(2)[3],
		(float)aMatrix.GetColumn(3)[0], (float)aMatrix.GetColumn(3)[1], (float)aMatrix.GetColumn(3)[2], (float)aMatrix.GetColumn(3)[3]
	};
	return DirectX::XMMATRIX(matrixArray);
}

void ModelFactory::GetElementMappingData(fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector4>* anElement, int aFbxContolPointIdx, int aPolygonIdx, fbxsdk::FbxVector4& outData)
{
	switch (anElement->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
	{
		switch (anElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outData.mData[0] = anElement->GetDirectArray().GetAt(aFbxContolPointIdx).mData[0];
			outData.mData[1] = anElement->GetDirectArray().GetAt(aFbxContolPointIdx).mData[1];
			outData.mData[2] = anElement->GetDirectArray().GetAt(aFbxContolPointIdx).mData[2];
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			const int Idx = anElement->GetIndexArray().GetAt(aFbxContolPointIdx);
			outData.mData[0] = anElement->GetDirectArray().GetAt(Idx).mData[0];
			outData.mData[1] = anElement->GetDirectArray().GetAt(Idx).mData[1];
			outData.mData[2] = anElement->GetDirectArray().GetAt(Idx).mData[2];
		}
		break;
		default:
			throw std::exception("Invalid Reference Mode!");
		}
	}
	break;

	case FbxGeometryElement::eByPolygonVertex:
	{
		switch (anElement->GetReferenceMode())
		{
		case FbxGeometryElement::eDirect:
		{
			outData.mData[0] = anElement->GetDirectArray().GetAt(aPolygonIdx).mData[0];
			outData.mData[1] = anElement->GetDirectArray().GetAt(aPolygonIdx).mData[1];
			outData.mData[2] = anElement->GetDirectArray().GetAt(aPolygonIdx).mData[2];
		}
		break;
		case FbxGeometryElement::eIndexToDirect:
		{
			const int Idx = anElement->GetIndexArray().GetAt(aPolygonIdx);
			outData.mData[0] = anElement->GetDirectArray().GetAt(Idx).mData[0];
			outData.mData[1] = anElement->GetDirectArray().GetAt(Idx).mData[1];
			outData.mData[2] = anElement->GetDirectArray().GetAt(Idx).mData[2];
		}
		break;
		default:
			throw std::exception("Invalid Reference Mode!");
		}
	}
	break;
	}
}

bool ModelFactory::GatherSkeleton(FbxNode* aRootNode, FBXSkeleton& inOutSkeleton, std::vector<FBXSkeleton::FBXBone>& outEventBones, std::vector<FBXSkeleton::FBXSocket>& outSockets, int someParentIndex)
{
	int myIndex = someParentIndex;

	//if (aRootNode->GetNodeAttribute()
	//	/*aRootNode->GetNodeAttribute()->GetAttributeType() &&*/
	//	/*aRootNode->GetNodeAttribute()->GetAttributeType() == FbxNodeAttribute::eSkeleton*/)
	//{

		// We're a bone, process as normal
	/*FBXSkeleton::FBXBone bone;
	bone.NamespaceName = aRootNode->GetName();
	bone.ParentIdx = someParentIndex;

	bone.Name = bone.NamespaceName;
	if (const size_t pos = bone.Name.find_last_of(':'); pos != std::string::npos)
	{
		bone.Name = bone.Name.substr(pos + 1);
	}
	inOutSkeleton.Bones.push_back(bone);

	myIndex = static_cast<int>(inOutSkeleton.Bones.size() - 1ULL);

	if (inOutSkeleton.BoneNameToIndex.find(bone.Name) != inOutSkeleton.BoneNameToIndex.end())
	{
		std::throw_with_nested(std::runtime_error("Found more than one bone with the same name [" + bone.NamespaceName + "]!"));
	}

	inOutSkeleton.BoneNameToIndex.insert({ bone.Name, myIndex });

	if (someParentIndex >= 0)
	{
		inOutSkeleton.Bones[someParentIndex].Children.push_back(myIndex);
	}*/

	if (aRootNode->GetNodeAttribute())
	{

		switch (aRootNode->GetNodeAttribute()->GetAttributeType())
		{
		case FbxNodeAttribute::eSkeleton:
		{
			// We're a bone, process as normal
			FBXSkeleton::FBXBone bone;
			bone.NamespaceName = aRootNode->GetName();
			bone.ParentIdx = someParentIndex;

			bone.Name = bone.NamespaceName;
			if (const size_t pos = bone.Name.find_last_of(':'); pos != std::string::npos)
			{
				bone.Name = bone.Name.substr(pos + 1);
			}
			inOutSkeleton.Bones.push_back(bone);

			myIndex = static_cast<int>(inOutSkeleton.Bones.size() - 1ULL);

			if (inOutSkeleton.BoneNameToIndex.find(bone.Name) != inOutSkeleton.BoneNameToIndex.end())
			{
				std::throw_with_nested(std::runtime_error("Found more than one bone with the same name [" + bone.NamespaceName + "]!"));
			}

			inOutSkeleton.BoneNameToIndex.insert({ bone.Name, myIndex });

			if (someParentIndex >= 0)
			{
				inOutSkeleton.Bones[someParentIndex].Children.push_back(myIndex);
			}
		}
		break;
		case FbxNodeAttribute::eNull:
		{
			// We're a something or other. Might be an Event or Socket.

			/*const std::string nodeName = aRootNode->GetName();
			nodeName;
			int a = 0;
			a++;*/
			//if (StringEndsWith(nodeName, "_evt") || StringEndsWith(nodeName, "_event"))
			//{
			//	FBXSkeleton::FBXBone bone;
			//	bone.NamespaceName = nodeName;
			//	bone.ParentIdx = someParentIndex;

			//	outEventBones.push_back(bone);
			//}
			//else if (StringEndsWith(nodeName, "_skt") || StringEndsWith(nodeName, "_socket"))
			//{
			//	FBXSkeleton::FBXSocket socket;
			//	socket.Name = nodeName;
			//	socket.ParentBoneIdx = someParentIndex;

			//	FbxAMatrix globalFrameTransform;
			//	globalFrameTransform = aRootNode->EvaluateGlobalTransform(0);
			//	Matrix GlobalTransform;
			//	Internals::FBXMatrixToArray(globalFrameTransform, GlobalTransform.Data);
			//	outSockets.emplace_back(socket);
			//}
		}
		break;
		default:
		{

		}
		break;
		}
	}


	for (int i = 0; i < aRootNode->GetChildCount(); i++)
	{
		GatherSkeleton(aRootNode->GetChild(i), inOutSkeleton, outEventBones, outSockets, myIndex);
	}

	return !inOutSkeleton.Bones.empty();
}

std::shared_ptr<Animation> ModelFactory::GetAnimation(std::string& aFilePath, const AnimatedMesh* /*aMesh*/)
{
	//Use cache
	auto item = myAnimationCache.find(aFilePath);
	if (item != myAnimationCache.end())
	{
		return item->second;
	}
	std::string path = aFilePath;
	FbxManager* lSdkManager = FbxManager::Create();

	FbxIOSettings* ios = FbxIOSettings::Create(lSdkManager, IOSROOT);
	lSdkManager->SetIOSettings(ios);

	FbxImporter* fbxImporter = FbxImporter::Create(lSdkManager, "");

	std::string filePath = std::string(RELATIVE_ASSET_PATH) + aFilePath.c_str();
	bool lImportStatus = fbxImporter->Initialize(filePath.c_str(), -1, lSdkManager->GetIOSettings());
	if (!lImportStatus)
	{
		LogError("Failed to import animation in FBX importer stage: " + aFilePath);
		return std::shared_ptr<Animation>();
	}

	FbxScene* fbxScene = FbxScene::Create(fbxImporter, "Scene");
	if (!fbxScene)
	{
		LogError("Failed to create scene when loading animation");
		return std::shared_ptr<Animation>();
	}
	if (fbxImporter->Import(fbxScene))
	{
		FbxAxisSystem fbxAxisSystem;
		fbxAxisSystem = FbxAxisSystem::eDirectX;
		fbxAxisSystem.DeepConvertScene(fbxScene);
		FBXSkeleton mdlSkeleton;
		std::vector<FBXSkeleton::FBXBone> skeletonEventBones;
		std::vector<FBXSkeleton::FBXSocket> skeletonSockets;
		GatherSkeleton(fbxScene->GetRootNode(), mdlSkeleton, skeletonEventBones, skeletonSockets, -1);

		const FbxNode* skeletonRoot = fbxScene->FindNodeByName(FbxString(mdlSkeleton.GetRoot()->NamespaceName.c_str()));
		const FbxVector4 fbxTranslation = skeletonRoot->GetGeometricTranslation(::FbxNode::eSourcePivot);
		const FbxVector4 fbxRotation = skeletonRoot->GetGeometricRotation(::FbxNode::eSourcePivot);
		const FbxVector4 fbxScale = skeletonRoot->GetGeometricScaling(::FbxNode::eSourcePivot);
		const FbxAMatrix rootTransform = FbxAMatrix(fbxTranslation, fbxRotation, fbxScale);

		//const int animStackCount = fbxImporter->GetAnimStackCount();

		const FbxAnimStack* animStack = fbxScene->GetCurrentAnimationStack();

		FbxString fbxAnimStackName = animStack->GetName();
		std::string fbxAnimationName = fbxAnimStackName.Buffer();

		const FbxTakeInfo* fbxTakeInfo = fbxScene->GetTakeInfo(fbxAnimStackName);

		const FbxTime::EMode fbxGlobalTimeMode = fbxScene->GetGlobalSettings().GetTimeMode();
		const double fbxFramesPerSecond = FbxTime::GetFrameRate(fbxGlobalTimeMode);
		const FbxTime fbxAnimStart = fbxTakeInfo->mLocalTimeSpan.GetStart();
		const FbxTime fbxAnimEnd = fbxTakeInfo->mLocalTimeSpan.GetStop();

		// The FBX animation may not be frames 0...N and can start at arbitrary places.
		const double fbxAnimTimeStart = fbxAnimStart.GetSecondDouble();
		const double fbxAnimTimeEnd = fbxAnimEnd.GetSecondDouble();
		const double fbxAnimTimeDuration = fbxAnimTimeEnd - fbxAnimTimeStart;

		const FbxLongLong startFrame = fbxAnimStart.GetFrameCount(fbxGlobalTimeMode);
		const FbxLongLong endFrame = fbxAnimEnd.GetFrameCount(fbxGlobalTimeMode);
		const FbxLongLong animTime = endFrame - startFrame + 1;

		FBXAnimation animation;
		animation.Name = aFilePath;
		animation.Length = static_cast<unsigned int>(animTime);
		animation.FramesPerSecond = static_cast<float>(fbxFramesPerSecond);
		animation.Frames.resize(animTime);
		animation.Duration = static_cast<float>(fbxAnimTimeDuration);

		size_t localFrameCounter = 0;

		for (FbxLongLong t = startFrame; t <= endFrame; t++)
		{
			FbxTime currentTime;
			currentTime.SetFrame(t, fbxGlobalTimeMode);

			animation.Frames[localFrameCounter].LocalTransforms.reserve(mdlSkeleton.Bones.size());
			animation.Frames[localFrameCounter].GlobalTransforms.reserve(mdlSkeleton.Bones.size());

			for (FBXSkeleton::FBXBone& Bone : mdlSkeleton.Bones)
			{
				FbxAMatrix localFrameTransform;
				FbxAMatrix globalFrameTransform;

				if (FbxNode* jointNode = fbxScene->FindNodeByName(FbxString(Bone.NamespaceName.c_str())))
				{
					localFrameTransform = jointNode->EvaluateLocalTransform(currentTime);
					globalFrameTransform = jointNode->EvaluateGlobalTransform(currentTime);
				}

				float LocalTransformArray[16] =
				{
					(float)localFrameTransform.GetRow(0)[0], (float)localFrameTransform.GetRow(0)[1], (float)localFrameTransform.GetRow(0)[2], (float)localFrameTransform.GetRow(0)[3],
					(float)localFrameTransform.GetRow(1)[0], (float)localFrameTransform.GetRow(1)[1], (float)localFrameTransform.GetRow(1)[2], (float)localFrameTransform.GetRow(1)[3],
					(float)localFrameTransform.GetRow(2)[0], (float)localFrameTransform.GetRow(2)[1], (float)localFrameTransform.GetRow(2)[2], (float)localFrameTransform.GetRow(2)[3],
					(float)localFrameTransform.GetRow(3)[0], (float)localFrameTransform.GetRow(3)[1], (float)localFrameTransform.GetRow(3)[2], (float)localFrameTransform.GetRow(3)[3]
				};
				float GlobalTransformArray[16] =
				{
					(float)globalFrameTransform.GetRow(0)[0], (float)globalFrameTransform.GetRow(0)[1], (float)globalFrameTransform.GetRow(0)[2], (float)globalFrameTransform.GetRow(0)[3],
					(float)globalFrameTransform.GetRow(1)[0], (float)globalFrameTransform.GetRow(1)[1], (float)globalFrameTransform.GetRow(1)[2], (float)globalFrameTransform.GetRow(1)[3],
					(float)globalFrameTransform.GetRow(2)[0], (float)globalFrameTransform.GetRow(2)[1], (float)globalFrameTransform.GetRow(2)[2], (float)globalFrameTransform.GetRow(2)[3],
					(float)globalFrameTransform.GetRow(3)[0], (float)globalFrameTransform.GetRow(3)[1], (float)globalFrameTransform.GetRow(3)[2], (float)globalFrameTransform.GetRow(3)[3]
				};
				DirectX::XMMATRIX LocalTransform(LocalTransformArray);
				DirectX::XMMATRIX GlobalTransform(GlobalTransformArray);


				animation.Frames[localFrameCounter].LocalTransforms.insert({ Bone.Name, LocalTransform });
				animation.Frames[localFrameCounter].GlobalTransforms.insert({ Bone.Name, GlobalTransform });
			}

			for (FBXSkeleton::FBXSocket& socket : skeletonSockets)
			{
				FbxAMatrix globalFrameTransform;

				if (FbxNode* jointNode = fbxScene->FindNodeByName(FbxString(socket.Name.c_str())))
				{
					globalFrameTransform = jointNode->EvaluateGlobalTransform(currentTime);
				}

				float GlobalTransformArray[16] =
				{
					(float)globalFrameTransform.GetRow(0)[0], (float)globalFrameTransform.GetRow(0)[1], (float)globalFrameTransform.GetRow(0)[2], (float)globalFrameTransform.GetRow(0)[3],
					(float)globalFrameTransform.GetRow(1)[0], (float)globalFrameTransform.GetRow(1)[1], (float)globalFrameTransform.GetRow(1)[2], (float)globalFrameTransform.GetRow(1)[3],
					(float)globalFrameTransform.GetRow(2)[0], (float)globalFrameTransform.GetRow(2)[1], (float)globalFrameTransform.GetRow(2)[2], (float)globalFrameTransform.GetRow(2)[3],
					(float)globalFrameTransform.GetRow(3)[0], (float)globalFrameTransform.GetRow(3)[1], (float)globalFrameTransform.GetRow(3)[2], (float)globalFrameTransform.GetRow(3)[3]
				};
				DirectX::XMMATRIX GlobalTransform(GlobalTransformArray);

				animation.Frames[localFrameCounter].SocketTransforms.insert({ socket.Name, GlobalTransform });
			}

			for (FBXSkeleton::FBXBone& EventBone : skeletonEventBones)
			{
				if (FbxNode* jointNode = fbxScene->FindNodeByName(FbxString(EventBone.NamespaceName.c_str())))
				{
					FbxAMatrix localFrameTransform = jointNode->EvaluateLocalTransform(currentTime);
					float LocalTransformArray[16] =
					{
						(float)localFrameTransform.GetRow(0)[0], (float)localFrameTransform.GetRow(0)[1], (float)localFrameTransform.GetRow(0)[2], (float)localFrameTransform.GetRow(0)[3],
						(float)localFrameTransform.GetRow(1)[0], (float)localFrameTransform.GetRow(1)[1], (float)localFrameTransform.GetRow(1)[2], (float)localFrameTransform.GetRow(1)[3],
						(float)localFrameTransform.GetRow(2)[0], (float)localFrameTransform.GetRow(2)[1], (float)localFrameTransform.GetRow(2)[2], (float)localFrameTransform.GetRow(2)[3],
						(float)localFrameTransform.GetRow(3)[0], (float)localFrameTransform.GetRow(3)[1], (float)localFrameTransform.GetRow(3)[2], (float)localFrameTransform.GetRow(3)[3]
					};
					DirectX::XMMATRIX LocalTransform(LocalTransformArray);
					const float eventMagnitude = std::powf(LocalTransformArray[12] + LocalTransformArray[13] + LocalTransformArray[14], 2);
					if (eventMagnitude > 0)
					{
						animation.Frames[localFrameCounter].TriggeredEvents.insert({ EventBone.Name, true });
					}
				}
			}

			localFrameCounter++;
		}

		std::shared_ptr<Animation> returnAnimation = std::make_shared<Animation>();
		returnAnimation->Name = animation.Name;
		returnAnimation->Length = animation.Length;
		returnAnimation->FramesPerSecond = animation.FramesPerSecond;
		returnAnimation->Frames.resize(animation.Frames.size());
		returnAnimation->Duration = static_cast<float>(animation.Duration);

		for (size_t f = 0; f < returnAnimation->Frames.size(); f++)
		{
			returnAnimation->Frames[f].LocalTransforms.reserve(animation.Frames[f].LocalTransforms.size());

			for (const auto& [boneName, boneTransform] : animation.Frames[f].LocalTransforms)
			{
				DirectX::XMMATRIX localMatrix = boneTransform;
				//memcpy_s(&localMatrix, sizeof(DirectX::XMMATRIX), boneTransform, sizeof(float) * 16);

				Transform t;
				t.SetMatrix(localMatrix);
				returnAnimation->Frames[f].LocalTransforms.emplace(boneName, t);
			}
		}

		myAnimationCache.insert({ path, returnAnimation });
		return returnAnimation;
	}

	return std::shared_ptr<Animation>();
}

AnimationPlayer ModelFactory::GetAnimationPlayer(std::string& aFilePath, const AnimatedMesh* aMesh)
{
	AnimationPlayer instance;
	std::shared_ptr<Animation> animation = GetAnimation(aFilePath, aMesh);
	if (animation)
		instance.Init(animation, aMesh);
	return instance;
}

ModelFactory::~ModelFactory()
{
	for (auto& [key, value] : myMeshCache)
	{
		for (int i = 0; i < value.size(); i++)
		{
			delete value[i];
		}
		value.clear();
	}
	myMeshCache.clear();

	//for (auto& [key, value] : myAnimatedMeshCache)
	//{
	//	for (int i = 0; i < value.size(); i++)
	//	{
	//		//delete value[i];
	//	}
	//	value.clear();
	//}
	myAnimatedMeshCache.clear();
	myAnimationCache.clear();
	myMaterialCache.clear();

	for (auto& [key, value] : myConvexMeshCache)
	{
		for (int i = 0; i < value.size(); i++)
		{
			value[i]->release();
		}
		value.clear();
	}
	myConvexMeshCache.clear();

	for (auto& [key, value] : myTriangleMeshCache)
	{
		for (int i = 0; i < value.size(); i++)
		{
			value[i]->release();
		}
		value.clear();
	}
	myTriangleMeshCache.clear();
}
