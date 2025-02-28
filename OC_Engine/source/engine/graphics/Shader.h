#pragma once
#include <vector>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;
struct ID3D11BlendState;
struct D3D11_INPUT_ELEMENT_DESC;

class Shader
{
public:
	Shader();
	~Shader();

	virtual bool Init(const char* aVertexShaderPath, const char* aPixelShaderPath, std::vector<D3D11_INPUT_ELEMENT_DESC> aInputLayout);

	ID3D11VertexShader* GetVertexShader() { return myVertexShader; }
	ID3D11PixelShader* GetPixelShader() { return myPixelShader; }
	ID3D11InputLayout* GetInputLayout() { return myInputLayout; }


	void SetShader();
protected:
	ID3D11VertexShader* myVertexShader;
	ID3D11PixelShader* myPixelShader;
	ID3D11InputLayout* myInputLayout;
	ID3D11BlendState* myBlendState;
};

