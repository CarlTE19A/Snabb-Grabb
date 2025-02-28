#include "stdafx.h"
#include "VolumetricFog.h"
#include <graphics\GraphicsEngine.h>
#include <fstream>
#include <graphics/model/Vertex.h>
#include <random>
#include"imgui/imgui.h"
#include <mainSingelton/MainSingelton.h>
#include <tools/lineDrawer/LineDrawer.h>
#include <d3dcompiler.h>

struct CloudBufferData
{
	Vector4f boundsMin;
	Vector4f boundsMax;
	Vector4f CloudOffset;
	Vector4f PhaseParams;
	float CloudScale;
	float DensityThreshold;
	float DensityMultiplier;
	int numSteps;
	float lightAbsorptionThroughCloud;
	int numStepsLight;
	float darknessThreshold;
	float detailWeight;
};

VolumetricFog::VolumetricFog()
	:isInside(false)
{}

VolumetricFog::~VolumetricFog() {}

void VolumetricFog::Init()
{
	if (!myNoiseTexture.Init(256, 256, 256))
	{
		LogError("Failed to init texture, volumetricclouds.cpp");
	}
	InitShadersAndBuffers();
	myTransform.SetMatrix(DirectX::XMMatrixIdentity());
	myTransform.SetPosition(Vector3f(0.f, -6000.f, 0.f));
	myTransform.SetScale(Vector3f(70000, 10000, 70000));
	auto* instance = GraphicsEngine::GetInstance();
	ID3D11DeviceContext* context = instance->GetContext();
	myNoiseTexture.BindUAV(0, context);
	context->CSSetShader(myCSShader.Get(), nullptr, 0);
	context->Dispatch(32, 32, 32);
	ID3D11UnorderedAccessView* null = nullptr;
	context->CSSetUnorderedAccessViews(0, 1, &null, 0);

	myBoundsMin = Vector4f(myTransform.GetPosition() - myTransform.GetScale() * 0.5f, 0);
	myBoundsMax = Vector4f(myTransform.GetPosition() + myTransform.GetScale() * 0.5f, 0);
	myCloudOffset = { 0.f, 0.f, 0.f, 0.f };
	myPhaseParams = { 0.0f, 0.0f, 0.027f, 0.023f };
	myCloudScale = 0.01f;
	myDensityThreshold = 0.04f;
	myDensityMultiplier = 0.178f;
	//myDensityMultiplier = 0.5f;
	myNumSteps = 50;
	lightAbsorptionThroughCloud = 0.008f;
	numStepsLight = 4;
	darknessThreshold = 0.145f;
	detailWeight = 0.5f;

	isInside = false;
}

void VolumetricFog::Update(const Vector3f aCameraPos)
{
//#ifdef _DEBUG
//	if (ImGui::Begin("Fluffy clouds"))
//	{
//		ImGui::SliderFloat("Cloud scale", &myCloudScale, 0, 1.f);
//		ImGui::SliderFloat("Density threshold", &myDensityThreshold, 0, 1.f);
//		ImGui::SliderFloat("Density mulitiplier", &myDensityMultiplier, 0, 1.f);
//		ImGui::SliderFloat("Light absorption through cloud", &lightAbsorptionThroughCloud, 0, 1.f);
//		ImGui::SliderInt("Num steps lightmarch", &numStepsLight, 0, 50);
//		ImGui::SliderFloat("Darkness threshold", &darknessThreshold, 0, 1.f);
//		ImGui::SliderFloat("Detail weight", &detailWeight, 0, 1.f);
//		ImGui::SliderInt("Num steps", &myNumSteps, 0, 100);
//
//		ImGui::SliderFloat("Phase params X", &myPhaseParams.x, 0, 1);
//		ImGui::SliderFloat("Phase params Y", &myPhaseParams.y, 0, 1);
//		ImGui::SliderFloat("Phase params Z", &myPhaseParams.z, 0, 1);
//		ImGui::SliderFloat("Phase params W", &myPhaseParams.w, 0, 1);
//
//		ImGui::SliderFloat("Cloud offset X", &myCloudOffset.x, 0, 100);
//		ImGui::SliderFloat("Cloud offset Y", &myCloudOffset.y, 0, 100);
//		ImGui::SliderFloat("Cloud offset Z", &myCloudOffset.z, 0, 100);
//	}
//	ImGui::End();
//#endif

	isInside = IsInside(aCameraPos);

	Vector3f newPos = { aCameraPos.x, myTransform.GetPosition().y, aCameraPos.z };
	myTransform.SetPosition(newPos);
	myBoundsMin = Vector4f(myTransform.GetPosition() - myTransform.GetScale() * 0.5f, 0);
	myBoundsMax = Vector4f(myTransform.GetPosition() + myTransform.GetScale() * 0.5f, 0);
}

void VolumetricFog::Render()
{
	auto* instance = GraphicsEngine::GetInstance();
	ID3D11DeviceContext* context = instance->GetContext();

	if (!isInside)
	{
		instance->UseDepth(true);
		instance->SetDepthStencilState(DepthStencilState::ReadLessEqual);
		instance->SetBlendState(BlendState::AddAndFadeBackground);
		instance->SetRasterizerState(RasterizerState::NoFaceCulling);
		instance->UpdateGPUStates();
	}

	auto objectBuffer = instance->GetObjectBuffer();
	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		auto result = context->Map(objectBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		if (FAILED(result)) return;

		ObjectBufferData* data = (ObjectBufferData*)mappedBuffer.pData;
		data->modelToWorld = myTransform.GetMatrix() * DirectX::XMMatrixIdentity();

		context->Unmap(objectBuffer.Get(), 0);

		context->VSSetConstantBuffers(1, 1, objectBuffer.GetAddressOf());
	}

	{
		D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
		context->Map(myCloudBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);

		CloudBufferData* data = (CloudBufferData*)mappedBuffer.pData;
		data->boundsMin = myBoundsMin;
		data->boundsMax = myBoundsMax;
		data->CloudOffset = myCloudOffset;
		data->PhaseParams = myPhaseParams;
		data->CloudScale = myCloudScale;
		data->lightAbsorptionThroughCloud = lightAbsorptionThroughCloud;
		data->numStepsLight = numStepsLight;
		data->DensityMultiplier = myDensityMultiplier;
		data->DensityThreshold = myDensityThreshold;
		data->numSteps = myNumSteps;
		data->detailWeight = detailWeight;
		data->darknessThreshold = darknessThreshold;

		context->Unmap(myCloudBuffer.Get(), 0);
		context->PSSetConstantBuffers(8, 1, myCloudBuffer.GetAddressOf());
	}

	myNoiseTexture.BindSRV(8, context);

	if (!isInside)
	{
		context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		unsigned int stride = sizeof(SpecialVertex);
		unsigned int offset = 0;
		context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(myInputLayout.Get());
		context->PSSetShader(myPixelShader.Get(), nullptr, 0);
		context->VSSetShader(myVertexShader.Get(), nullptr, 0);

		context->DrawIndexed(myIndexCount, 0, 0);
	}
	else
	{
		////testing with fullscreen pass
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		context->IASetInputLayout(nullptr);
		context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
		context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
		context->PSSetShader(myPixelShaderFullscreen.Get(), nullptr, 0);
		context->VSSetShader(myVertexShaderFullscreen.Get(), nullptr, 0);
		context->Draw(3, 0);
	}

	//You stupid
	ID3D11ShaderResourceView* null2 = nullptr;
	context->PSSetShaderResources(8, 1, &null2);

	instance->SetDepthStencilState(DepthStencilState::Write);
	instance->SetRasterizerState(RasterizerState::BackfaceCulling);
	instance->UpdateGPUStates();

//#ifdef _DEBUG
//	// Get the LineDrawer instance
//	LineDrawer& lineDrawer = MainSingleton::GetInstance().GetLineDrawer();
//	// Define a debug line
//	DebugLine line;
//	line.color = { 0, 0, 1, 1 };
//	// Render the edges of the AABB
//	// Front face
//	line.start = { myBoundsMin.x, myBoundsMin.y, myBoundsMin.z };
//	line.end = { myBoundsMax.x, myBoundsMin.y, myBoundsMin.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMax.x, myBoundsMin.y, myBoundsMin.z };
//	line.end = { myBoundsMax.x, myBoundsMax.y, myBoundsMin.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMax.x, myBoundsMax.y, myBoundsMin.z };
//	line.end = { myBoundsMin.x, myBoundsMax.y, myBoundsMin.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMin.x, myBoundsMax.y, myBoundsMin.z };
//	line.end = { myBoundsMin.x, myBoundsMin.y, myBoundsMin.z };
//	lineDrawer.Draw(line);
//
//	// Back face
//	line.start = { myBoundsMin.x, myBoundsMin.y, myBoundsMax.z };
//	line.end = { myBoundsMax.x, myBoundsMin.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMax.x, myBoundsMin.y, myBoundsMax.z };
//	line.end = { myBoundsMax.x, myBoundsMax.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMax.x, myBoundsMax.y, myBoundsMax.z };
//	line.end = { myBoundsMin.x, myBoundsMax.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMin.x, myBoundsMax.y, myBoundsMax.z };
//	line.end = { myBoundsMin.x, myBoundsMin.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//
//	// Connect front and back faces
//	line.start = { myBoundsMin.x, myBoundsMin.y, myBoundsMin.z };
//	line.end = { myBoundsMin.x, myBoundsMin.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMax.x, myBoundsMin.y, myBoundsMin.z };
//	line.end = { myBoundsMax.x, myBoundsMin.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMax.x, myBoundsMax.y, myBoundsMin.z };
//	line.end = { myBoundsMax.x, myBoundsMax.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//
//	line.start = { myBoundsMin.x, myBoundsMax.y, myBoundsMin.z };
//	line.end = { myBoundsMin.x, myBoundsMax.y, myBoundsMax.z };
//	lineDrawer.Draw(line);
//	lineDrawer.DrawAllLines();
//#endif	
}

bool VolumetricFog::IsInside(const Vector3f aCameraPos)
{
	if (myBoundsMax.x < aCameraPos.x) return false;
	if (myBoundsMin.x > aCameraPos.x) return false;
	if (myBoundsMax.y < aCameraPos.y) return false;
	if (myBoundsMin.y > aCameraPos.y) return false;
	if (myBoundsMax.z < aCameraPos.z) return false;
	if (myBoundsMin.z > aCameraPos.z) return false;
	return true; // AABB överlappar
}

bool VolumetricFog::InitShadersAndBuffers()
{
	std::vector<SpecialVertex> cubeVertices = {
	{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f }, // 0
	{ 0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f },  // 1
	{ 0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f },   // 2
	{ -0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 1.0f },  // 3
	{ -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f },  // 4
	{ 0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f},    // 5
	{ 0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 1.0f },    // 6
	{ -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f}     // 7
	};

	std::vector<unsigned int> cubeIndices = {
	0, 3, 2, 2, 1, 0,  // Front face (CCW)
	1, 2, 6, 6, 5, 1,  // Right face (CCW)
	4, 5, 6, 6, 7, 4,  // Back face (CCW)
	3, 0, 4, 4, 7, 3,  // Left face (CCW)
	3, 7, 6, 6, 2, 3,  // Top face (CCW)
	0, 1, 5, 5, 4, 0   // Bottom face (CCW)
	};

	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	{
		// Create vertex buffer
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = sizeof(SpecialVertex) * (UINT)cubeVertices.size();
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		D3D11_SUBRESOURCE_DATA vertexData = { 0 };

		vertexData.pSysMem = cubeVertices.data();
		result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
		{
			LogError("Failed to create vertex buffer, volumetric fog");
			return false;
		}
	}

	myIndexCount = static_cast<unsigned int>(cubeIndices.size());
	{
		// Create index buffer
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned int) * static_cast<unsigned int>(cubeIndices.size());
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = cubeIndices.data();
		// Create the index buffer.
		result = device->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
		{
			LogError("Failed to create index buffer, volumetric fog");
			return false;
		}
	}

	//create cloud buffer
	{
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(CloudBufferData);
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		result = device->CreateBuffer(&bufferDesc, NULL, myCloudBuffer.ReleaseAndGetAddressOf());
		if (FAILED(result))
		{
			LogError("failed to create cloudbuffer volumetricfog.cpp");
			return false;
		}
	}

	std::string vsData;
	{
		std::string shaderPath = STATIC_SHADER_PATH;
		// Load shaders
		std::ifstream vsFile;
		vsFile.open(shaderPath + "VolumetricClouds_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
		if (FAILED(result))
		{
			LogError("Failed to create vertex shader, volumetric fog");
			return false;
		}
		vsFile.close();

		std::string csData;
		std::ifstream csFile;
		csFile.open(shaderPath + "PerlinNoiseGen_CS.cso", std::ios::binary);

		csData = { std::istreambuf_iterator<char>(csFile), std::istreambuf_iterator<char>() };
		result = device->CreateComputeShader(csData.data(), csData.size(), nullptr, &myCSShader);
		if (FAILED(result))
		{
			LogError("Failed to create compute shader, volumetric fog");
			return false;
		}
		csFile.close();
		std::ifstream psFile;
		psFile.open(shaderPath + "VolumetricFog_PS.cso", std::ios::binary);

		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
		if (FAILED(result))
		{
			LogError("Failed to create pixel shader, volumetric fog");
			return false;
		}
		psFile.close();
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	int numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);
	result = device->CreateInputLayout(layout, numElements, vsData.data(), vsData.size(), &myInputLayout);
	if (FAILED(result))
	{
		LogError("Failed to create input layout, volumetric fog");
		return false;
	}

	{
		std::string shaderPath = STATIC_SHADER_PATH;
		// Load shaders
		std::ifstream vsFile;
		vsFile.open(shaderPath + "VolumetricFog_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShaderFullscreen);
		if (FAILED(result))
		{
			LogError("Failed to create vertex shader, volumetric fog");
			return false;
		}
		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "VolumetricFogFS_PS.cso", std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShaderFullscreen);
		if (FAILED(result))
		{
			LogError("Failed to create pixel shader, volumetric fog");
			return false;
		}
		psFile.close();
	}

	return true;
}
