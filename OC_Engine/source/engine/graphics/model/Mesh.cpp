#include "stdafx.h"

#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
//#include "Mesh.h"
#include "../../external/stb_image.h"
//#include <d3d11.h>
#include <fstream>
//#include <vector>
#include "math/Timer.h"
#include "../DDSTextureLoader11.h"
#include "../GraphicsEngine.h"
#include "Component/CameraComponent.h"
#include "../GraphicsData.h"
#include "ModelShader.h"

Mesh::Mesh() = default;
Mesh::~Mesh()
{
	//myVertexBuffer->Release();
	//myIndexBuffer->Release();
	myTextures.clear();
}

bool Mesh::Initialize(
	MeshVertex aVerts[],
	const unsigned int aVertexCount,
	unsigned int aIndices[],
	const unsigned int& aIndexCount,
	const std::string& aFileName,
	const std::string& aTextureName,
	std::unordered_map<std::string, Texture>& aMaterialCache
)
{
	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();

	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(MeshVertex) * aVertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };

		//myTransform = {};
		/*myTransform(4, 1) = aPosition.x;
		myTransform(4, 2) = aPosition.y;
		myTransform(4, 3) = aPosition.z;*/
		//myTransform.SetMatrix(DirectX::XMMatrixIdentity());
		//myTransform.SetPosition(aPosition);

		vertexData.pSysMem = aVerts;
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	{
		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * aIndexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = aIndices;
		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData,
			&myIndexBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	{
		std::vector<std::string> texturePaths;
		std::string path = aFileName.substr(0, aFileName.find_last_of("/") + 1);

		texturePaths.push_back(aTextureName + "_c");
		texturePaths.push_back(aTextureName + "_n");
		texturePaths.push_back(aTextureName + "_m");
		texturePaths.push_back(aTextureName + "_fx");

		myTextures.resize(texturePaths.size());//reserve(texturePaths.size());
		for (size_t i = 0; i < texturePaths.size(); i++)
		{
			auto item = aMaterialCache.find(texturePaths[i]);
			if (item != aMaterialCache.end())
			{
				myTextures[i] = &item->second;
			}
			else if (i == 0)
			{
				myTextures[i] = &aMaterialCache.find("missing_texture")->second;
				LogError("Missing Texture: " + aFileName + " M: " + texturePaths[i]);
			}
		}
	}
	myName = aTextureName;
	myIndexCount = aIndexCount;
	return true;
}

void Mesh::Render(ModelShader& aModelShader, Transform& aTransform, const bool isHighlighted)
{
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	auto objectBuffer = instance->GetObjectBuffer();

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	auto result = context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	if (FAILED(result)) return;

	ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
	data->modelToWorld = aTransform.GetMatrix() * DirectX::XMMatrixIdentity();
	data->isHighlight = (int)isHighlighted;
#ifdef _DEBUG
	if (isHighlighted)
	{
		std::cout << "It do be selected hello?";
	}
#endif

	context->Unmap(objectBuffer.Get(), 0);

	context->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
	context->PSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());

	for (int i = 0; i < (int)myTextures.size(); i++)
	{
		// cube map reserves slot 0
		if (myTextures[i] != nullptr)
		{
			myTextures[i]->Bind(i + 1, context);
		}
	}

	unsigned int stride = sizeof(MeshVertex);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	aModelShader.SetShader(context);

	context->DrawIndexed(myIndexCount, 0, 0);
}

void Mesh::ChangeMaterial(Texture& aTexture, int index)
{
	myTextures[index] = nullptr;
	myTextures[index] = &aTexture;
}
Texture& Mesh::GetTexture(int index)
{
	return *myTextures[index];
}

void Mesh::BindTextures()
{
	auto* context = GraphicsEngine::GetInstance()->GetContext();
	for (int i = 0; i < (int)myTextures.size(); i++)
	{
		// cube map reserves slot 0
		if (myTextures[i] != nullptr)
		{
			myTextures[i]->Bind(i + 1, context);
		}
	}
}
