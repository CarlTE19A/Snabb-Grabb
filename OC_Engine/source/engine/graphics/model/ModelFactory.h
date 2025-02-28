#pragma once
#include <unordered_map>
#include <memory>
#include "FBXLoadStructs.h"
#include <graphics\Texture.h>


class Mesh;
class AnimatedMesh;
class MeshComponent;
struct ID3D11Device;
struct AnimatedMeshVertex;
struct Animation;
class AnimationPlayer;
struct Skeleton;

struct aiNode;
struct aiScene;
struct aiAnimation;

namespace fbxsdk
{
	class FbxNode;
	class FbxScene;
	class FbxAMatrix;
	class FbxVector4;
	template <class Type>
	class FbxLayerElementTemplate;
};
namespace physx
{
	class PxConvexMesh;
	class PxTriangleMesh;
}

class ModelFactory
{
public:
	void LoadMaterial(const std::string aMaterialPath);

	bool LoadTriangleMeshData(std::string aFilePath, bool& staticPath, bool& isConvex, std::string aNodeName = "None" );
	std::vector<Mesh*> Load(std::string aFilePath, std::string aNodeName = "None", bool staticPath = false);
	std::vector<AnimatedMesh*> Load(std::string aFilePath, const bool anAnimatedMesh);
	std::vector<physx::PxConvexMesh*> GetConvexPhysxMesh(std::string aCasheName);
	std::vector<physx::PxTriangleMesh*> GetTrianglePhysxMesh(std::string aCasheName);
	std::unordered_map<std::string, std::vector<physx::PxTriangleMesh*>>& GetTriangleCache() { return myTriangleMeshCache; }
	AnimationPlayer GetAnimationPlayer(std::string& aFilePath, const AnimatedMesh* aMesh);
	~ModelFactory();
private:
	void Process(
		const std::string& aFileName,
		const aiNode* node,
		const aiScene* scene,
		std::vector<Mesh*>& aMeshVector,
		const std::string aNodeName = "None"
	);

	//For Physx collision stuff :)
	bool Process(const std::string& aFileName,
		const aiNode* aNode,
		const aiScene* aScene,
		std::vector<physx::PxTriangleMesh*>& aTriangleMeshes,
		std::vector<physx::PxConvexMesh*>& convexMeshes,
		const std::string aNodeName = "None",
		bool isConvex = true);

	bool Process(const std::string& aFilePath, FBXMesh& aMesh);

	size_t GatherMeshNodes(fbxsdk::FbxNode* aRootNode, std::vector<fbxsdk::FbxNode*>& someSceneMeshNodes);
	size_t GatherLODGroups(fbxsdk::FbxNode* aRootNode, std::vector<fbxsdk::FbxNode*>& outNodes);
	bool FbxMeshToMesh(fbxsdk::FbxScene* aScene, fbxsdk::FbxNode* aMeshNode, FBXSkeleton& aSkeleton, std::vector<FBXMesh::Element>& outMeshes);
	DirectX::XMMATRIX FBXMatrixToDXMatrix(fbxsdk::FbxAMatrix& aMatrix);

	void GetElementMappingData(fbxsdk::FbxLayerElementTemplate<fbxsdk::FbxVector4>* anElement, int aFbxContolPointIdx, int aPolygonIdx, fbxsdk::FbxVector4& outData);
	bool GatherSkeleton(fbxsdk::FbxNode* aRootNode, FBXSkeleton& inOutSkeleton, std::vector<FBXSkeleton::FBXBone>& outEventBones, std::vector<FBXSkeleton::FBXSocket>& outSockets, int someParentIndex);

	std::shared_ptr<Animation> GetAnimation(std::string& aFilePath, const AnimatedMesh* aMesh);

	std::unordered_map<std::string, Texture> myMaterialCache;
	std::unordered_map<std::string, std::vector<Mesh*>> myMeshCache;
	std::unordered_map<std::string, FBXMesh> myAnimatedMeshCache;
	std::unordered_map<std::string, std::shared_ptr<Animation>> myAnimationCache;
	std::unordered_map<std::string, std::vector<physx::PxConvexMesh*>> myConvexMeshCache;
	std::unordered_map<std::string, std::vector<physx::PxTriangleMesh*>> myTriangleMeshCache;
};