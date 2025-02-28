#pragma once
#include <graphics\light\AmbientLight.h>

using Microsoft::WRL::ComPtr;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;

class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	void Init(const std::wstring aCubeMapPath);
	void Render();
private:
	bool InitShadersAndBuffers();
	
	AmbientLight myCubeMap;
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;
	ComPtr<ID3D11Buffer> myVertexBuffer;
	ComPtr<ID3D11Buffer> myIndexBuffer;

	unsigned int myIndexCount = 0;
};