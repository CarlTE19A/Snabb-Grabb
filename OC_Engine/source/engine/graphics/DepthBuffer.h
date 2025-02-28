#pragma once
#include <wrl/client.h>
#include <math\Vector2.h>
#include <d3d11.h>
#include <vector>

using Microsoft::WRL::ComPtr;
struct ID3D11DepthStencilView;
struct ID3D11Texture2D;
struct D3D11_VIEWPORT;
struct ID3D11ShaderResourceView;

class DepthBuffer
{
public:
	std::vector<D3D11_VIEWPORT> myViewports;
	ComPtr<ID3D11ShaderResourceView> mySRV;

	static DepthBuffer Create(Vector2<int> aSize);

	void Clear(float aClearDepthValue = 1.0f, uint8_t aClearStencilValue = 0u);
	ID3D11DepthStencilView* GetDepthStencilView() { return myDepth.Get(); };

	void SetAsActiveTarget(const int aViewportIndex);
	void SetAsResourceOnSlot(const unsigned int aSlot);

	//for shadowatlas only
	static DepthBuffer CreateForShadowAtlas(Vector2<int> aSize);
	int RegisterForViewport(const Vector2i aSize);

private:
	ComPtr<ID3D11DepthStencilView> myDepth = 0;

	//for shadow atlas
	int myWidth;
	int myHeight;
	bool FindAvailableSpace(const Vector2i aSize, D3D11_VIEWPORT& viewport);
	bool IsSpaceOccupied(float x, float y, const Vector2i aSize) const;
};