#include "stdafx.h"
#include "ModelDrawer.h"
#include "ModelShader.h"
#include "component\mesh\AnimatedMeshComponent.h"
#include "component\mesh\MeshComponent.h"
#include "graphics\GraphicsEngine.h"
#include "mainSingelton/MainSingelton.h"
#include "events\inputMapper\InputMapper.h"
#include <gameObject\GameObject.h>



constexpr size_t BATCH_SIZE = 10024;

ModelDrawer::ModelDrawer() {}

ModelDrawer::~ModelDrawer() {}

bool ModelDrawer::Init()
{
	{
		D3D11_BUFFER_DESC objectBufferDesc;
		objectBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		objectBufferDesc.ByteWidth = sizeof(InstanceData) * BATCH_SIZE;
		objectBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		objectBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		objectBufferDesc.MiscFlags = 0;
		objectBufferDesc.StructureByteStride = 0;

		auto* instance = GraphicsEngine::GetInstance();
		auto device = instance->GetDevice();
		HRESULT result = device->CreateBuffer(&objectBufferDesc, nullptr, &myInstanceBuffer);
		if (FAILED(result))
		{
			LogError("Failed to create instance buffer for meshes");
		}
	}

	myPbrShader = std::make_unique<ModelShader>();
	if (!myPbrShader->Init("pbr_default_VS.cso", "pbr_default_PS.cso"))
	{
		return false;
	}

	myGBufferShader = std::make_unique<ModelShader>();
	if (!myGBufferShader->Init("pbr_default_VS.cso", "GBuffer_PS.cso"))
	{
		return false;
	}

	myBloodNovaVFXShader = std::make_unique<ModelShader>();
	if (!myBloodNovaVFXShader->Init("pbr_default_VS.cso", "vfx_PS.cso"))
	{
		return false;
	}

	myPbrAnimatedModelShader = std::make_unique<ModelShader>();
	if (!myPbrAnimatedModelShader->Init("AnimatedPBRModelShader_VS.cso", "pbr_default_PS.cso"))
	{
		return false;
	}

	myUnlitShader = std::make_unique<ModelShader>();
	if (!myUnlitShader->Init("ModelShader_VS.cso", "ModelShader_PS.cso"))
	{
		return false;
	}

	myShadowShader = std::make_unique<ModelShader>();
	if (!myShadowShader->Init("ModelShader_VS.cso", ""))
	{
		//need pixelshader to be null
		//return false;
	}

	myAnimatedShadowShader = std::make_unique<ModelShader>();
	if (!myAnimatedShadowShader->Init("AnimatedPBRModelShader_VS.cso", ""))
	{
		//need pixelshader to be null
		//return false;
	}

	myAnimatedUnlitShader = std::make_unique<ModelShader>();
	if (!myAnimatedUnlitShader->Init("AnimatedPBRModelShader_VS.cso", "ModelShader_PS.cso"))
	{
		return false;
	}

	myGBufferAnimShader = std::make_unique<ModelShader>();
	if (!myGBufferAnimShader->Init("AnimatedPBRModelShader_VS.cso", "GBuffer_PS.cso"))
	{
		return false;
	}

	myScrollMeshShader = std::make_unique<ModelShader>();
	if (!myScrollMeshShader->Init("ModelShader_VS.cso", "vfx_PS.cso"))
	{
		return false;
	}

	myBoosterMeshShader = std::make_unique<ModelShader>();
	if (!myBoosterMeshShader->Init("ModelShader_VS.cso", "booster_PS.cso"))
	{
		return false;
	}

	return true;
}

void ModelDrawer::Draw(AnimatedMeshComponent& anAnimatedMesh, ModelShader& shader)
{
	anAnimatedMesh.Render(shader);
}

void ModelDrawer::Draw(MeshComponent& aMesh, ModelShader& shader)
{
	aMesh.Render(shader);
}

void ModelDrawer::DrawBatch(std::vector<MeshComponent*>& aMeshVector, ModelShader& shader)
{
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	for (auto meshComponent : aMeshVector)
	{
		if (!meshComponent->IsActive() || !meshComponent->GetOwner()->IsActive()) continue;
		for (auto mesh : meshComponent->GetMesh())
		{
			if (instanceDataMap.find(mesh) == instanceDataMap.end())
			{
				instanceDataMap[mesh] = {};
				instanceDataMap[mesh].reserve(32);
			}
			InstanceData data;
			//data.modelToWorldInstance = meshComponent->GetOwner()->GetTransform()->GetMatrix() * DirectX::XMMatrixIdentity();
			data.modelToWorldInstance = meshComponent->GetOwner()->MultiplyWithParent().GetMatrix() * DirectX::XMMatrixIdentity();
			instanceDataMap[mesh].push_back(data);
		}
	}

	for (auto it = instanceDataMap.begin(); it != instanceDataMap.end();)
	{
		auto* mesh = it->first;

		if (it->second.size() == 0)
		{
			it = instanceDataMap.erase(it);
			continue;
		}

		mesh->BindTextures();
		// Set the vertex buffer with appropriate stride
		ID3D11Buffer* vertexBuffers[2] = { mesh->GetVertexBuffer().Get(), myInstanceBuffer.Get() };
		UINT strides[2];
		strides[0] = sizeof(MeshVertex);
		strides[1] = sizeof(InstanceData);
		UINT offsets[2] = { 0, 0 };
		shader.SetShader(context);

		// Update the instance buffer
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &it->second[0], sizeof(InstanceData) * it->second.size());
		context->Unmap(myInstanceBuffer.Get(), 0);

		context->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
		context->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexedInstanced(mesh->myIndexCount, (UINT)it->second.size(), 0, 0, 0);
		it->second.clear();
	}
	instanceDataMap.clear();
}

void ModelDrawer::DrawBatch(std::vector<Mesh*>& aMeshVector, std::vector<Transform*>& someTransforms, ModelShader& shader)
{
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();

	for (int i = 0; i < someTransforms.size(); i++)
	{
		if (instanceDataMap.find(aMeshVector[0]) == instanceDataMap.end())
		{
			instanceDataMap[aMeshVector[0]] = {};
			instanceDataMap[aMeshVector[0]].reserve(someTransforms.size() * 16);
		}
		InstanceData data;
		//data.modelToWorldInstance = meshComponent->GetOwner()->GetTransform()->GetMatrix() * DirectX::XMMatrixIdentity();
		data.modelToWorldInstance = someTransforms[i]->GetMatrix() * DirectX::XMMatrixIdentity();
		instanceDataMap[aMeshVector[0]].push_back(data);
	}

	for (auto it = instanceDataMap.begin(); it != instanceDataMap.end();)
	{
		auto* mesh = it->first;

		if (it->second.size() == 0)
		{
			it = instanceDataMap.erase(it);
			continue;
		}

		mesh->BindTextures();
		// Set the vertex buffer with appropriate stride
		ID3D11Buffer* vertexBuffers[2] = { mesh->GetVertexBuffer().Get(), myInstanceBuffer.Get() };
		UINT strides[2];
		strides[0] = sizeof(MeshVertex);
		strides[1] = sizeof(InstanceData);
		UINT offsets[2] = { 0, 0 };
		shader.SetShader(context);

		// Update the instance buffer
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &it->second[0], sizeof(InstanceData) * it->second.size());
		context->Unmap(myInstanceBuffer.Get(), 0);

		context->IASetVertexBuffers(0, 2, vertexBuffers, strides, offsets);
		context->IASetIndexBuffer(mesh->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
		context->DrawIndexedInstanced(mesh->myIndexCount, (UINT)it->second.size(), 0, 0, 0);
		it->second.clear();
	}
	instanceDataMap.clear();
}