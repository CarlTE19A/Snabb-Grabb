#pragma once
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct D3D11_VIEWPORT;

class FullscreenEffect
{
public:
	~FullscreenEffect() = default;
	FullscreenEffect();
	bool Init(const char* aPixelShaderPath);
	void Render();
	void RecompileShader(ID3D11Device * aDevice, const wchar_t* aPixelShaderPath);
private:
	ComPtr<ID3D11VertexShader> myVertexShader;
	ComPtr<ID3D11PixelShader> myPixelShader;

	//ComPtr<ID3D11Texture2D> myEffectTexture;
	//ComPtr<ID3D11ShaderResourceView> myEffectSRV;
	//ComPtr<ID3D11RenderTargetView> myEffectRTV;
	////ComPtr<ID3D11Buffer> myEffectBuffer;
	//D3D11_VIEWPORT* myViewport;
};