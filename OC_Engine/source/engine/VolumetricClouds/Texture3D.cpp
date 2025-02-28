#include "stdafx.h"
#include "Texture3D.h"

#include <graphics\GraphicsEngine.h>
#include <graphics\DepthBuffer.h>
#include <fstream>

Texture3D::Texture3D()
: mTextureSize(0, 0, 0)
{}

Texture3D::~Texture3D(){}

bool Texture3D::Init(const int aWidth, const int aHeigth, const int aDepth)
{
	HRESULT result;
	mTextureSize.x = aWidth;
	mTextureSize.y = aHeigth;
	mTextureSize.z = aDepth;
	D3D11_TEXTURE3D_DESC desc;
	desc.Width = mTextureSize.x;
	desc.Height = mTextureSize.y;
	desc.Depth = mTextureSize.z;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	auto& device = GraphicsEngine::GetInstance()->GetDevice();

	ID3D11Texture3D* colorMap = 0;
	result = device->CreateTexture3D(&desc, 0, &colorMap);
	if (FAILED(result))
	{
		LogError("Failed to init 3D texture, during create texture");
		return false;
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	srvDesc.Texture3D.MostDetailedMip = 0;
	srvDesc.Texture3D.MipLevels = 1;

	result = device->CreateShaderResourceView(colorMap, &srvDesc, &mySRV);
	if (FAILED(result))
	{
		LogError("Failed to init 3D texture, during create srv");
		return false;
	}
	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE3D; //or D3D11_UAV_DIMENSION_BUFFER??
	uavDesc.Texture3D.MipSlice = 0;
	uavDesc.Texture3D.FirstWSlice = 0; // Specifies the starting depth slice
	uavDesc.Texture3D.WSize = desc.Depth; // Specifies the number of slices to use

	result = device->CreateUnorderedAccessView(colorMap, &uavDesc, &myUAV);
	if (FAILED(result))
	{
		LogError("Failed to init 3D texture, during create UAV");
		return false;
	}
	colorMap->Release();

	return true;
}

void Texture3D::BindSRV(int slot, ID3D11DeviceContext* context)
{
	context->PSSetShaderResources(slot, 1, mySRV.GetAddressOf());
}

void Texture3D::BindUAV(const int slot, ID3D11DeviceContext* context)
{
	context->CSSetUnorderedAccessViews(slot, 1, myUAV.GetAddressOf(), 0);
}
