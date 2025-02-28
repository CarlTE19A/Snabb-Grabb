#pragma once
#include <array>
#include <memory>
#include <math/Vector.h>
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;
struct ID3D11Texture2D;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct D3D11_VIEWPORT;

class DepthBuffer;

class GBuffer
{
public:
	enum class GBufferTexture
	{
		WorldPosition, // Wastes space and can be reconstructed from depth and screen pos, but we store it for simplicity
		Albedo,
		Normal, // stored as 0.5f + 0.5f*normal
		Material,
		EffectsAndCustom, // Emissive in R, rest is unused
		Count
	};
	std::array<ComPtr<ID3D11Texture2D>, (int)GBufferTexture::Count> myTextures;
	std::array<ComPtr<ID3D11RenderTargetView>, (int)GBufferTexture::Count> myRTVs;
	std::array<ComPtr<ID3D11ShaderResourceView>, (int)GBufferTexture::Count> mySRVs;
	std::shared_ptr<const D3D11_VIEWPORT> myViewport;
public:
	GBuffer();
	~GBuffer();
	static GBuffer Create(Vector2<int> aSize);
	void ClearTextures();
	void SetAsActiveTarget(DepthBuffer* aDepthBuffer = nullptr);
	void SetAsResourceOnSlot(GBufferTexture aTexture, unsigned int aSlot);
	void SetAllAsResources(unsigned aSlot);
	void ClearAllAsResources(unsigned int aSlot);
};