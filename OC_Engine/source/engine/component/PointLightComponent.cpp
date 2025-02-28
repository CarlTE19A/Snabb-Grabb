#include "stdafx.h"

#include "PointLightComponent.h"
#include "graphics\light\PointLight.h"
#include "graphics/GraphicsEngine.h"
#include "graphics/light/PointLight.h"
#include "gameObject/GameObject.h"

#include <fstream>

struct PointLightBufferData
{
	Vector3f myPosition;
	float myRange;
	Vector4f myColorAndIntensity;
};

PointLightComponent::PointLightComponent(PointLight* aPointLight) : myPointLight(aPointLight)
{
	InitSphereMesh();
}

PointLightComponent::~PointLightComponent() {}

void PointLightComponent::Update(const float /*aDeltaTime*/, Scene&)
{
	myPointLight->SetTransform(*myParent->GetTransform());
	//GraphicsEngine::GetInstance()->AddPointLight(*myPointLight);
}

void PointLightComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Point Light"))
	{
		//Color
		Vector4f color = myPointLight->GetColor();
		float floatColor[4] = { color.x, color.y, color.z, color.w };
		ImguiNativeUpdate();
		ImGui::ColorEdit4("Color", floatColor);
		if (color.x != floatColor[0] || color.y != floatColor[1] || color.z != floatColor[2] || color.w != floatColor[3])
		{
			color = Vector4f(floatColor[0], floatColor[1], floatColor[2], floatColor[3]);
			myPointLight->SetColor(color);
		}

		//Intensity
		float intensity = myPointLight->GetIntensity();
		ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10000.0f);
		if (intensity != myPointLight->GetIntensity())
		{
			myPointLight->SetIntensity(intensity);
		}

		float range = myPointLight->GetRange();
		ImGui::SliderFloat("Range", &range, 0.0f, 10000.0f);
		if (range != myPointLight->GetRange())
		{
			myPointLight->SetRange(range);
		}

		ImGui::TreePop();
	}
}

void PointLightComponent::Render()
{
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	auto objectBuffer = instance->GetObjectBuffer();
	
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		auto result = context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		if (FAILED(result)) return;

		ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
		data->modelToWorld = myPointLight->GetTransform().GetMatrix() * DirectX::XMMatrixIdentity();

		context->Unmap(objectBuffer.Get(), 0);

		context->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
		context->PSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
	}

	{
		PointLightBufferData data = {};
		data.myPosition = myPointLight->GetTransform().GetPosition();
		data.myRange = myPointLight->GetRange();
		data.myColorAndIntensity = myPointLight->GetColor();
		data.myColorAndIntensity.w = myPointLight->GetIntensity();

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &data, sizeof(PointLightBufferData));
		context->Unmap(myLightBuffer.Get(), 0);

		context->VSSetConstantBuffers(6, 1, myLightBuffer.GetAddressOf());
		context->PSSetConstantBuffers(6, 1, myLightBuffer.GetAddressOf());
	}

	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	unsigned int stride = sizeof(SimplestVertex);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(myInputLayout.Get());
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);
	context->VSSetShader(myVertexShader.Get(), nullptr, 0);

	context->DrawIndexed(myIndexCount, 0, 0);
}

std::vector<SimplestVertex> PointLightComponent::CreateSphereVertices(float radius, int slices, int stacks)
{
	std::vector<SimplestVertex> vertices;

	for (int i = 0; i <= stacks; ++i)
	{
		float phi = static_cast<float>(i) / stacks * 3.14159265359f;

		for (int j = 0; j <= slices; ++j)
		{
			float theta = static_cast<float>(j) / slices * 2.0f * 3.14159265359f;

			SimplestVertex vertex;
			vertex.x = radius * sin(phi) * cos(theta);
			vertex.y = radius * cos(phi);
			vertex.z = radius * sin(phi) * sin(theta);
			vertex.w = 1.0f;

			vertices.push_back(vertex);
		}
	}

	return vertices;
}

std::vector<unsigned int> PointLightComponent::CreateSphereIndices(int slices, int stacks)
{
	std::vector<unsigned int> indices;

	for (int i = 0; i < stacks; ++i)
	{
		for (int j = 0; j < slices; ++j)
		{
			int first = i * (slices + 1) + j;
			int second = first + slices + 1;

			indices.push_back(static_cast<unsigned int>(first + 1));
			indices.push_back(static_cast<unsigned int>(second));
			indices.push_back(static_cast<unsigned int>(first));

			indices.push_back(static_cast<unsigned int>(first + 1));
			indices.push_back(static_cast<unsigned int>(second + 1));
			indices.push_back(static_cast<unsigned int>(second));
		}
	}

	return indices;
}



PointLight& PointLightComponent::GetPointLight() { return *myPointLight; }

void PointLightComponent::InitSphereMesh()
{
	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	int stacks = 10;
	int slices = 10;

	std::vector<unsigned int> indices = CreateSphereIndices(stacks, slices);
	std::vector<SimplestVertex> vertices = CreateSphereVertices(myPointLight->GetRange(), stacks, slices);
	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(SimplestVertex) * (UINT)vertices.size();
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };

		vertexData.pSysMem = vertices.data();
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
		{}
	}

	myIndexCount = static_cast<unsigned int>(indices.size());
	{
		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<unsigned int>(indices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = indices.data();
		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
		{}
	}

	{ //Create pointlight buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(PointLightBufferData);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&bufferDesc, NULL, myLightBuffer.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			//return false;
		}
	}

	std::string vsData;
	{
		// Load shaders
		std::string shaderPath = STATIC_SHADER_PATH;
		std::ifstream vsFile;
		vsFile.open(shaderPath + "DeferredPointLight_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
		if (FAILED(result))
		{
			//return false;
		}
		vsFile.close();
		std::ifstream psFile;
		psFile.open(shaderPath + "DeferredPointLight_PS.cso", std::ios::binary);

		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
		if (FAILED(result))
		{
			//return false;
		}
		psFile.close();
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	result = device->CreateInputLayout(layout, 1, vsData.data(), vsData.size(), &myInputLayout);
	if (FAILED(result))
	{
		//return false;
	}
}
