#pragma once
#include "Component.h"
#include <wrl/client.h>
#include "graphics\model\Vertex.h"

struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

class PointLight;

class PointLightComponent : public Component
{
public:
	PointLightComponent(PointLight* aPointLight);

	~PointLightComponent() override;

	void Update(const float /*aDeltaTime*/, Scene& aScene) override;

	void ImguiUpdate();

	void Render();

	void Reset() override
	{
		//Does not need reset
	}

	PointLight& GetPointLight();
private:
	void InitSphereMesh();

	std::vector<unsigned int> CreateSphereIndices(int slices, int stacks);
	std::vector<SimplestVertex> CreateSphereVertices(float radius, int slices, int stacks);

	PointLight* myPointLight;
	unsigned int myIndexCount = 0;
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;
	ComPtr<ID3D11Buffer> myLightBuffer;

	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;
};