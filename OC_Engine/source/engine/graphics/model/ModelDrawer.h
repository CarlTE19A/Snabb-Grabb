#pragma once
#include <vector>
#include <map>
#include "math\Transform.h"

class AnimatedMeshComponent;
class MeshComponent;
class ModelShader;
class Mesh;
using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;

struct InstanceData
{
	DirectX::XMMATRIX modelToWorldInstance;
};

class ModelDrawer
{
public:
	ModelDrawer();
	~ModelDrawer();
	bool Init();

	void Draw(AnimatedMeshComponent& anAnimatedMesh, ModelShader& shader);
	void Draw(MeshComponent& aMesh, ModelShader& shader);
	void DrawBatch(std::vector<MeshComponent*>& aMeshVector, ModelShader& shader);
	void DrawBatch(std::vector<Mesh*>& aMeshVector, std::vector<Transform*>& someTransforms, ModelShader& shader);

	ModelShader& GetPbrAnimatedShader() { return *myPbrAnimatedModelShader; }
	ModelShader& GetPbrShader() { return *myPbrShader; }
	ModelShader& GetUnlitShader() { return *myUnlitShader; }
	ModelShader& GetShadowShader() { return *myShadowShader; }
	ModelShader& GetAnimatedUnlitShader() { return *myAnimatedUnlitShader; }
	ModelShader& GetBloodNovaShader() { return *myBloodNovaVFXShader; }
	ModelShader& GetGBufferShader() { return *myGBufferShader; }
	ModelShader& GetGBufferAnimShader() { return *myGBufferAnimShader; }
	ModelShader& GetScrollMeshShader() { return *myScrollMeshShader; }
	ModelShader& GetBoosterMeshShader() { return *myBoosterMeshShader; }
	ModelShader& GetAnimatedShadowShader() { return *myAnimatedShadowShader; }

private:
	std::unique_ptr<ModelShader> myPbrShader;
	std::unique_ptr<ModelShader> myPbrAnimatedModelShader;
	std::unique_ptr<ModelShader> myUnlitShader;
	std::unique_ptr<ModelShader> myShadowShader;
	std::unique_ptr<ModelShader> myAnimatedUnlitShader;
	std::unique_ptr<ModelShader> myAnimatedShadowShader;
	std::unique_ptr<ModelShader> myBloodNovaVFXShader;
	std::unique_ptr<ModelShader> myGBufferShader;
	std::unique_ptr<ModelShader> myGBufferAnimShader;
	std::unique_ptr<ModelShader> myScrollMeshShader;
	std::unique_ptr<ModelShader> myBoosterMeshShader;

	ComPtr<ID3D11Buffer> myInstanceBuffer;
	std::map<Mesh*, std::vector<InstanceData>> instanceDataMap;
};