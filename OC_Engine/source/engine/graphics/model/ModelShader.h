#pragma once
#include <wrl/client.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11DeviceContext;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

class ModelShader
{
public:
	ModelShader();
	~ModelShader();

	bool Init(const std::string aVertexShaderFile, const std::string aPixelShaderFile);

	void SetShader(ID3D11DeviceContext* aContext);

private:
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;
	ComPtr<ID3D11InputLayout> myInputLayout;
};