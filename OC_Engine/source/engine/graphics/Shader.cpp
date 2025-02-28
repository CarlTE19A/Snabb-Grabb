#include "stdafx.h"

//#include "Shader.h"
#include <fstream>
//#include <d3d11.h>
#include "GraphicsEngine.h"

Shader::Shader()
{

}

Shader::~Shader()
{
	/*if (myVertexShader != nullptr)
	{
		myVertexShader->Release();
		myVertexShader = nullptr;
	}
	if (myPixelShader != nullptr)
	{
		myPixelShader->Release();
		myPixelShader = nullptr;
	}
	if (myInputLayout != nullptr)
	{
		myInputLayout->Release();
		myInputLayout = nullptr;
	}*/

}

bool Shader::Init(const char* aVertexShaderPath, const char* aPixelShaderPath, std::vector<D3D11_INPUT_ELEMENT_DESC> aInputLayout)
{
	char* vsData = nullptr;
	int vsDataSize = 0;
	{ // Vertex Shader
		FILE* fileptr;
		fopen_s(&fileptr, aVertexShaderPath, "rb");
		if (fileptr == 0)
			return false;

		fseek(fileptr, 0L, SEEK_END);
		vsDataSize = ftell(fileptr);
		fseek(fileptr, 0L, SEEK_SET);

		vsData = (char*)malloc(sizeof(char) * vsDataSize);
		if (vsData == 0)
		{
			puts("Failed to create buffer for loading model!");
			return false;
		}
		fread_s(vsData, vsDataSize, vsDataSize, 1, fileptr);
		fclose(fileptr);

		HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreateVertexShader(vsData, vsDataSize, nullptr, &myVertexShader);
		if (FAILED(result))
			return false;
	}
	
	char* psData = nullptr;
	int psDataSize = 0;
	{ // Pixel Shader
		FILE* fileptr;
		fopen_s(&fileptr, aPixelShaderPath, "rb");
		if (fileptr == 0)
			return false;

		fseek(fileptr, 0L, SEEK_END);
		psDataSize = ftell(fileptr);
		fseek(fileptr, 0L, SEEK_SET);

		psData = (char*)malloc(sizeof(char) * psDataSize);
		if (vsData == 0)
		{
			puts("Failed to create buffer for loading model!");
			return false;
		}
		fread_s(psData, psDataSize, psDataSize, 1, fileptr);
		fclose(fileptr);

		HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreatePixelShader(psData, psDataSize, nullptr, &myPixelShader);
		if (FAILED(result))
			return false;

	}

	int numElements = sizeof(aInputLayout[0]) / sizeof(D3D11_INPUT_ELEMENT_DESC);

	{ // Input Layout
		HRESULT result = GraphicsEngine::GetInstance()->GetDevice()->CreateInputLayout(&aInputLayout[0], numElements, vsData, vsDataSize, &myInputLayout);
		if (FAILED(result))
		{
			free(vsData);
			free(psData);
			return false;
		}
	}

	free(vsData);
	free(psData);
	return true;
}

void Shader::SetShader()
{
	auto* context = GraphicsEngine::GetInstance()->GetContext();
	context->VSSetShader(myVertexShader, nullptr, 0);
	context->PSSetShader(myPixelShader, nullptr, 0);
	context->IASetInputLayout(myInputLayout);
}
