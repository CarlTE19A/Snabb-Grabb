#pragma once
#include <wrl/client.h>
//#include <DirectXMath.h>
//#include <memory>
#include "Vertex.h"
//#include <string>
//#include <vector>
#include "math\Transform.h"
#include "../Animation/Pose.h"
#include "../Animation/Skeleton.h"
#include <unordered_map>

class Texture;
class AnimationPlayer;
class ModelShader;

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;


class AnimatedMesh
{
public:
	AnimatedMesh();
	~AnimatedMesh();

	bool Initialize(
		AnimatedMeshVertex someVertex[],
		const unsigned int aVertexCount,
		unsigned int aIndices[],
		const unsigned int& aIndexCount,
		const std::string aFileName,
		const std::string aTextureName,
		std::unordered_map<std::string, Texture>& aMaterialCache
	);

	void Render(ModelShader& aModelShader, Transform& aTransform, const bool anIsHighLighted);

	//const Transform& GetTransform() const { return myTransform; }
	//void SetTransform(const Transform& aTransform) { myTransform = aTransform; }

	inline std::string GetPath() const { return myPath; }

	void SetPose(const LocalSpacePose& aPose);
	void SetPose(const ModelSpacePose& aPose);
	void SetPose(const AnimationPlayer& animationInstance);
	void ResetPose();

	inline void SetSkeleton(Skeleton& aSkeleton) { mySkeleton = std::make_shared<Skeleton>(aSkeleton); hasSetSkeleton = true; }
	const Skeleton* GetSkeleton() const { return &*mySkeleton; }
private:
	std::vector<Texture*> myTextures;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;
	ComPtr<ID3D11Buffer> myBoneBuffer;
	
	unsigned int myIndexCount = 0;
	unsigned int* myIndices = nullptr;

	std::string myPath;
	std::shared_ptr<Skeleton> mySkeleton;
	DirectX::XMMATRIX myBoneTransforms[MAX_ANIMATION_BONES];

	bool hasSetSkeleton = false;
};