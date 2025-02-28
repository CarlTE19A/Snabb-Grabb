#include <stdafx.h>

#include "SkyBox.h"
#include <graphics\GraphicsEngine.h>
#include <graphics\model\Vertex.h>
#include <fstream>
#include <component\CameraComponent.h>


SkyBox::SkyBox()
{
}

SkyBox::~SkyBox()
{
}

void SkyBox::Init(const std::wstring aCubeMapPath)
{
	myCubeMap.SetCubemap(aCubeMapPath);
	if (!InitShadersAndBuffers())
	{
		LogError("Failed to initilize skybox");
	}
}

void SkyBox::Render()
{
	auto* instance = GraphicsEngine::GetInstance();
	ID3D11DeviceContext* context = instance->GetContext();


	context->PSSetShaderResources(9, 1, myCubeMap.GetCubemap());

	instance->SetDepthStencilState(DepthStencilState::ReadLessEqual);
	instance->SetRasterizerState(RasterizerState::NoFaceCulling);
	instance->UpdateGPUStates();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(myInputLayout.Get());
	unsigned int stride = sizeof(SimplestVertex);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);
	context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	context->DrawIndexed(myIndexCount, 0, 0);
}

bool SkyBox::InitShadersAndBuffers()
{
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	HRESULT result;

	std::vector<SimplestVertex> cubeVertices = {
	{ -0.5f, -0.5f, -0.5f, 1.0f}, // 0
	{ 0.5f, -0.5f, -0.5f, 1.0f },  // 1
	{ 0.5f, 0.5f, -0.5f, 1.0f },   // 2
	{ -0.5f, 0.5f, -0.5f, 1.0f },  // 3
	{ -0.5f, -0.5f, 0.5f, 1.0f },  // 4
	{ 0.5f, -0.5f, 0.5f, 1.0f },   // 5
	{ 0.5f, 0.5f, 0.5f, 1.0f },    // 6
	{ -0.5f, 0.5f, 0.5f, 1.0f }    // 7
	};

	std::vector<unsigned int> cubeIndices = {
	0, 3, 2, 2, 1, 0,  // Front face (CCW)
	1, 2, 6, 6, 5, 1,  // Right face (CCW)
	4, 5, 6, 6, 7, 4,  // Back face (CCW)
	3, 0, 4, 4, 7, 3,  // Left face (CCW)
	3, 7, 6, 6, 2, 3,  // Top face (CCW)
	0, 1, 5, 5, 4, 0   // Bottom face (CCW)
	};

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
			LogError("Failed to create vertex buffer, skybox");
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
			LogError("Failed to create index buffer, skybox");
			return false;
		}
	}

	
	std::string vsData;
	{
		std::string shaderPath = STATIC_SHADER_PATH;
		// Load shaders
		std::ifstream vsFile;
		vsFile.open(shaderPath + "SkyBox_VS.cso", std::ios::binary);

		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
		if (FAILED(result))
		{
			LogError("Failed to create vertex shader, skybox");
			return false;
		}
		vsFile.close();

		std::ifstream psFile;
		psFile.open(shaderPath + "SkyBox_PS.cso", std::ios::binary);

		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
		if (FAILED(result))
		{
			LogError("Failed to create pixel shader, skybox");
			return false;
		}
		psFile.close();
	}

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	int numElements = sizeof(layout) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	result = device->CreateInputLayout(layout, numElements, vsData.data(), vsData.size(), &myInputLayout);
	if (FAILED(result))
	{
		LogError("Failed to create input layout, skybox");
		return false;
	}

	return true;
}
