#include "stdafx.h"
#include "FullscreenEffect.h"
#include "GraphicsEngine.h"
#include <fstream>
#include <d3dcompiler.h>

FullscreenEffect::FullscreenEffect()
{
}

bool FullscreenEffect::Init(const char* aPixelShaderPath)
{
	HRESULT result;
	auto device = GraphicsEngine::GetInstance()->GetDevice();
	std::string vsData;
	// Load shaders
	std::string shaderPath = STATIC_SHADER_PATH;
	std::ifstream vsFile;
	vsFile.open(shaderPath + "Fullscreen_VS.cso", std::ios::binary);

	vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	result = device->CreateVertexShader(vsData.data(), vsData.size(), nullptr, &myVertexShader);
	if (FAILED(result))
	{
		return false;
	}
	vsFile.close();
	std::ifstream psFile;
	psFile.open(shaderPath + aPixelShaderPath, std::ios::binary);

	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	result = device->CreatePixelShader(psData.data(), psData.size(), nullptr, &myPixelShader);
	if (FAILED(result))
	{
		return false;
	}
	psFile.close();

	return true;
}

void FullscreenEffect::Render()
{
	auto context = GraphicsEngine::GetInstance()->GetContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);
	context->Draw(3, 0);
}

void FullscreenEffect::RecompileShader(ID3D11Device* aDevice, const wchar_t* aPixelShaderPath)
{
	aDevice;
	aPixelShaderPath;
#ifdef _DEBUG
	// Compile the shader from file
	ID3DBlob* compiledShader = nullptr;
	ID3DBlob* errorBlob = nullptr;

	HRESULT hr = D3DCompileFromFile(
		aPixelShaderPath, // Path to the shader file
		nullptr, // Macro definitions (can be nullptr)
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // Include handler (can be nullptr)
		"main", // Entry point function name
		"ps_5_0", // Pixel shader model
		0, // Compile flags (can be 0)
		0, // Additional compile flags (can be 0)
		&compiledShader, // Compiled shader blob
		&errorBlob // Error blob (used for error messages)
	);

	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
			errorBlob->Release();
		}
		return;
	}

	if (errorBlob)
	{
		errorBlob->Release();
	}

	// Create the pixel shader
	ID3D11PixelShader* pixelShader = nullptr;
	hr = aDevice->CreatePixelShader(
		compiledShader->GetBufferPointer(),
		compiledShader->GetBufferSize(),
		nullptr,
		&pixelShader
	);

	if (FAILED(hr))
	{
		compiledShader->Release();
		return;
	}

	// Release the compiled shader blob, as it's no longer needed
	compiledShader->Release();

	myPixelShader = nullptr;
	myPixelShader = pixelShader;
#endif
}
