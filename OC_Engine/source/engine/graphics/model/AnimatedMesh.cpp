#include "stdafx.h"

#include "AnimatedMesh.h"
#include "graphics\GraphicsEngine.h"
//#include <d3d11.h>
//#include "../Texture.h"
//#include "StringHelper.h"
//#include "Paths.h"
#include <fstream>
#include "graphics\Animation\AnimationPlayer.h"
#include "ModelShader.h"



AnimatedMesh::AnimatedMesh()
{
	ResetPose();
}

AnimatedMesh::~AnimatedMesh()
{
	/*myVertexBuffer->Release();
	myIndexBuffer->Release();*/
	myTextures.clear();
}

bool AnimatedMesh::Initialize(
	AnimatedMeshVertex someVertex[],
	const unsigned int aVertexCount, unsigned int aIndices[],
	const unsigned int& aIndexCount,
	const std::string aFileName,
	const std::string aTextureName,
	std::unordered_map<std::string, Texture>& aMaterialCache
)
{
	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();

	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(AnimatedMeshVertex) * aVertexCount;
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };

		vertexData.pSysMem = someVertex;
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
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	{
		std::vector<std::string> texturePaths;
		
		texturePaths.push_back(aTextureName + "_c");
		texturePaths.push_back(aTextureName + "_n");
		texturePaths.push_back(aTextureName + "_m");
		texturePaths.push_back(aTextureName + "_fx");

		myTextures.resize(texturePaths.size());
		for (size_t i = 0; i < texturePaths.size(); i++)
		{
			auto item = aMaterialCache.find(texturePaths[i]);
			if (item != aMaterialCache.end())
			{
				myTextures[i] = &item->second;
			}
			else if(i == 0)
			{
				myTextures[i] = &aMaterialCache.find("missing_texture")->second;
				LogError("Missing Texture: " + aFileName + " M: " + texturePaths[i]);
			}
		}
	}

	{
		D3D11_BUFFER_DESC matrixVertexBufferDesc;
		matrixVertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		matrixVertexBufferDesc.ByteWidth = sizeof(DirectX::XMMATRIX) * MAX_ANIMATION_BONES;
		matrixVertexBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		matrixVertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		matrixVertexBufferDesc.MiscFlags = 0;
		matrixVertexBufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&matrixVertexBufferDesc, NULL, &myBoneBuffer);
		if (FAILED(result))
		{
			return false;
		}
	}

	myIndexCount = aIndexCount;

	return true;
}

void AnimatedMesh::Render(ModelShader& aModelShader, Transform& aTransform, const bool anIsHighLighted)
{
	if (!hasSetSkeleton) return;


	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	auto objectBuffer = instance->GetObjectBuffer();

	D3D11_MAPPED_SUBRESOURCE mappedVResource;
	HRESULT boneResult = context->Map(myBoneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVResource);
	if (FAILED(boneResult))
	{
		//INFO_PRINT("Error in rendering!");
		return;
	}
	char* dataVPtr = (char*)mappedVResource.pData;
	memcpy(dataVPtr, myBoneTransforms, sizeof(DirectX::XMMATRIX) * MAX_ANIMATION_BONES);

	context->Unmap(myBoneBuffer.Get(), 0);
	context->VSSetConstantBuffers(4, 1, myBoneBuffer.GetAddressOf());

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	auto result = context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

	if (FAILED(result)) return;

	ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
	data->modelToWorld = aTransform.GetMatrix() * DirectX::XMMatrixIdentity();
	data->isHighlight = (int)anIsHighLighted;

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

	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	unsigned int stride = sizeof(AnimatedMeshVertex);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);

	aModelShader.SetShader(context);

	context->DrawIndexed(myIndexCount, 0, 0);
}

void AnimatedMesh::SetPose(const LocalSpacePose& pose)
{
	ModelSpacePose modelSpacePose;
	mySkeleton->ConvertPoseToModelSpace(pose, modelSpacePose);

	SetPose(modelSpacePose);
}

void AnimatedMesh::SetPose(const ModelSpacePose& pose)
{
	mySkeleton->ApplyBindPoseInverse(pose, myBoneTransforms);
}

void AnimatedMesh::SetPose(const AnimationPlayer& animationInstance)
{
	SetPose(animationInstance.GetLocalSpacePose());
}

void AnimatedMesh::ResetPose()
{
	for (int i = 0; i < MAX_ANIMATION_BONES; i++)
	{
		myBoneTransforms[i] = DirectX::XMMatrixIdentity();
	}
}

