#pragma once
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;

class GraphicsEngine;
class DepthBuffer;

class Texture3D
{
public:
	Texture3D();
	~Texture3D();

	bool Init(const int aWidth, const int aHeigth, const int aDepth);
	void BindSRV(int slot, ID3D11DeviceContext* context);
	void BindUAV(const int slot, ID3D11DeviceContext* context);
	ComPtr<ID3D11UnorderedAccessView> myUAV;
	ComPtr<ID3D11ShaderResourceView> mySRV;
private:
	Vector3<int> mTextureSize;
	//UINT mNumElements;
};