#include "stdafx.h"

#include "SpotLightComponent.h"
#include "graphics\light\SpotLight.h"
#include "graphics/GraphicsEngine.h"
#include "gameObject/GameObject.h"
#include <fstream>

struct SpotLightBufferData
{
	DirectX::XMMATRIX lightProjection;
	Vector3f myPosition;
	float myRange;
	Vector4f myColorAndIntensity;
	Vector3f myDirection;
	float trash;
	float myOuterLimit;
	float myInnerLimit;
	Vector2f padding;
	Vector2f ViewPortSize;
	Vector2f TopLeftXY;
};

SpotLightComponent::SpotLightComponent(SpotLight* aSpotLight) : mySpotLight(aSpotLight)
{
	InitSphereMesh();
}

SpotLightComponent::~SpotLightComponent()
{
	delete mySpotLight;
}

void SpotLightComponent::Update(const float /*aDeltaTime*/, Scene&)
{
	mySpotLight->SetTransform(*myParent->GetTransform());
}

void SpotLightComponent::ImguiUpdate()
{
	if (ImGui::BeginTabItem("Spot Light"))
	{
		//Color
		Vector4f color = mySpotLight->GetColor();
		float floatColor[4] = { color.x, color.y, color.z, color.w };
		ImguiNativeUpdate();
		ImGui::ColorEdit4("Color", floatColor);
		if (color.x != floatColor[0] || color.y != floatColor[1] || color.z != floatColor[2] || color.w != floatColor[3])
		{
			color = Vector4f(floatColor[0], floatColor[1], floatColor[2], floatColor[3]);
			mySpotLight->SetColor(color);
		}

		//Intensity
		float intensity = mySpotLight->GetIntensity();
		ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10000.0f);
		if (intensity != mySpotLight->GetIntensity())
		{
			mySpotLight->SetIntensity(intensity);
		}

		//Range
		float range = mySpotLight->GetRange();
		ImGui::SliderFloat("Range", &range, 0.0f, 10000.0f);
		if (range != mySpotLight->GetRange())
		{
			mySpotLight->SetRange(range);
		}

		//Limits
		float limits[2] = { mySpotLight->GetInnerLimit(), mySpotLight->GetOuterLimit() };
		ImGui::SliderFloat2("Limits", limits, 0.0f, 3.1415f);
		if (limits[0] != mySpotLight->GetInnerLimit() || limits[1] != mySpotLight->GetOuterLimit())
		{
			mySpotLight->SetInnerLimit(limits[0]);
			mySpotLight->SetOuterLimit(limits[1]);
		}

		ImGui::TreePop();
	};
}

void SpotLightComponent::Render() 
{
	if (!IsActive() || !myParent->IsActive()) return;
	auto* instance = GraphicsEngine::GetInstance();
	auto* context = instance->GetContext();
	auto objectBuffer = instance->GetObjectBuffer();

	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		auto result = context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		if (FAILED(result)) return;

		ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
		data->modelToWorld = mySpotLight->GetTransform().GetMatrix() * DirectX::XMMatrixIdentity();

		context->Unmap(objectBuffer.Get(), 0);

		context->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
		context->PSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
	}

	{
		SpotLightBufferData data = {};
		data.myPosition = mySpotLight->GetTransform().GetPosition();
		data.myRange = mySpotLight->GetRange();
		data.myColorAndIntensity = mySpotLight->GetColor();
		data.myColorAndIntensity.w = mySpotLight->GetIntensity();
		data.myDirection = mySpotLight->GetTransform().GetForward();
		data.myInnerLimit = mySpotLight->GetInnerLimit();
		data.myOuterLimit = mySpotLight->GetOuterLimit();
		data.ViewPortSize = mySpotLight->myViewPortSize;
		data.TopLeftXY = mySpotLight->myTopLeftXY;
		data.lightProjection = DirectX::XMMatrixInverse(nullptr, mySpotLight->GetTransform().GetMatrix()) * mySpotLight->GetLightProjectionMatrix();

		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
		memcpy(mappedBuffer.pData, &data, sizeof(SpotLightBufferData));
		context->Unmap(myLightBuffer.Get(), 0);

		context->VSSetConstantBuffers(7, 1, myLightBuffer.GetAddressOf());
		context->PSSetConstantBuffers(7, 1, myLightBuffer.GetAddressOf());
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

SpotLight& SpotLightComponent::GetSpotLight() { return *mySpotLight; }

std::vector<SimplestVertex> SpotLightComponent::CreateSphereVertices(float radius, int slices, int stacks)
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

std::vector<unsigned int> SpotLightComponent::CreateSphereIndices(int slices, int stacks)
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

void SpotLightComponent::InitSphereMesh()
{
	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	int stacks = 10;
	int slices = 10;

	std::vector<unsigned int> indices = CreateSphereIndices(stacks, slices);
	std::vector<SimplestVertex> vertices = CreateSphereVertices(mySpotLight->GetRange(), stacks, slices);
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
		{
		}
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
		{
		}
	}

	{ //Create pointlight buffer
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(SpotLightBufferData);
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
		psFile.open(shaderPath + "DeferredSpotLight_PS.cso", std::ios::binary);

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