#pragma once
#include <wrl/client.h>
//#include <string>

//#include "math/Vector.h"
#include "math/Matrix.h"
#include "math/Transform.h"
#include <vector>
#include "Vertex.h"
#include "graphics/Texture.h"
#include <unordered_map>
#include <d3d11.h>

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11Buffer;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

//class Texture;
class ModelShader;

class Mesh
{
public:
	Mesh();
	~Mesh();
	bool Initialize(
		MeshVertex[],
		const unsigned int aVertexCount,
		unsigned int[],
		const unsigned int& aIndexCount,
		const std::string& aFileName,
		const std::string& aTextureName,
		std::unordered_map<std::string, Texture>& aMaterialCache
	);
	void Render(ModelShader& aModelShader, Transform& aTransform, const bool isHighlighted);
	//const Transform& GetTransform() const { return myTransform; }
	//void SetTransform(const Transform& aTransform) { myTransform = aTransform; }
	void ChangeMaterial(Texture& aTexture, int index);

	size_t GetTextureCount() { return myTextures.size(); };
	Texture& GetTexture(int index);

	inline std::string GetPath() const { return myPath; }

	Transform myTransform;
	void BindTextures();

	ComPtr<ID3D11Buffer> GetVertexBuffer() { return myVertexBuffer; }
	ComPtr<ID3D11Buffer> GetIndexBuffer() { return myIndexBuffer; }
	unsigned int myIndexCount = 0;
private:
	unsigned int* myIndices = nullptr;
	//Transform myTransform;

	std::vector<Texture*> myTextures;

	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	std::string myPath;
	std::string myName;
};