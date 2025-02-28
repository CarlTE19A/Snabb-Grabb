#pragma once
#include <wrl/client.h>
#include <dxgiformat.h>
#include "math\Vector.h"
#include <memory>
using Microsoft::WRL::ComPtr;

struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;
struct ID3D11Buffer;

class DepthBuffer;

class RenderTarget
{
	ComPtr<ID3D11ShaderResourceView> mySRV;
	ComPtr<ID3D11RenderTargetView> myRenderTarget;

public:
	RenderTarget();
	~RenderTarget();

	static RenderTarget Create(Vector2<int> aSize, DXGI_FORMAT aFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	static RenderTarget Create(ID3D11Texture2D* aTexture);

	void Clear(float* aClearColor);
	void SetAsActiveTarget(DepthBuffer* aDepth = nullptr);
	void SetAsResourceOnSlot(unsigned int aSlot) const;
	std::shared_ptr<const D3D11_VIEWPORT> myViewport;
};