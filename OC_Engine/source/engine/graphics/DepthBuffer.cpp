#include "stdafx.h"
#include "DepthBuffer.h"
#include "GraphicsEngine.h"

void DepthBuffer::SetAsActiveTarget(const int aViewportIndex)
{
	if (aViewportIndex < 0) return;
	auto& context = *GraphicsEngine::GetInstance()->GetContext();
	context.OMSetRenderTargets(0, nullptr, GetDepthStencilView());
	context.RSSetViewports(1, &myViewports[aViewportIndex]);
}

void DepthBuffer::SetAsResourceOnSlot(const unsigned int aSlot)
{

	GraphicsEngine::GetInstance()->GetContext()->PSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}

void DepthBuffer::Clear(float aClearDepthValue /* = 1.0f */, uint8_t aClearStencilValue /* = 0 */)
{
	GraphicsEngine::GetInstance()->GetContext()->ClearDepthStencilView(myDepth.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aClearDepthValue, aClearStencilValue);
}

DepthBuffer DepthBuffer::Create(Vector2<int> aSize)
{
	auto& graphicsEngine = *GraphicsEngine::GetInstance();

	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = graphicsEngine.GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
	assert(SUCCEEDED(result));

	ID3D11DepthStencilView* DSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	CD3D11_DEPTH_STENCIL_DESC  test = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
	test.StencilEnable = true;
	test.StencilWriteMask = 0xFF;

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	result = graphicsEngine.GetDevice()->CreateDepthStencilView(texture, &dsvDesc, &DSV);

	assert(SUCCEEDED(result));

	DepthBuffer textureResult;
	textureResult.myDepth = DSV;
	DSV->Release();

	ID3D11ShaderResourceView* SRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = std::numeric_limits<UINT>::max();
	result = graphicsEngine.GetDevice()->CreateShaderResourceView(texture, &srDesc, &SRV);
	assert(SUCCEEDED(result));
	textureResult.mySRV = SRV;
	SRV->Release();
	textureResult.myViewports.emplace_back();
	textureResult.myViewports[0] = {
			0,
			0,
			static_cast<float>(aSize.x),
			static_cast<float>(aSize.y),
			0,
			1
	};
	return textureResult;
}

DepthBuffer DepthBuffer::CreateForShadowAtlas(Vector2<int> aSize)
{
	auto& graphicsEngine = *GraphicsEngine::GetInstance();

	HRESULT result;
	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.Format = DXGI_FORMAT_R32_TYPELESS;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	ID3D11Texture2D* texture;
	result = graphicsEngine.GetDevice()->CreateTexture2D(&desc, nullptr, &texture);
	assert(SUCCEEDED(result));

	ID3D11DepthStencilView* DSV;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};

	CD3D11_DEPTH_STENCIL_DESC  test = CD3D11_DEPTH_STENCIL_DESC{ CD3D11_DEFAULT{} };
	test.StencilEnable = true;
	test.StencilWriteMask = 0xFF;

	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	result = graphicsEngine.GetDevice()->CreateDepthStencilView(texture, &dsvDesc, &DSV);

	assert(SUCCEEDED(result));

	DepthBuffer textureResult;
	textureResult.myDepth = DSV;
	DSV->Release();

	ID3D11ShaderResourceView* SRV;
	D3D11_SHADER_RESOURCE_VIEW_DESC srDesc{};
	srDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDesc.Texture2D.MostDetailedMip = 0;
	srDesc.Texture2D.MipLevels = std::numeric_limits<UINT>::max();
	result = graphicsEngine.GetDevice()->CreateShaderResourceView(texture, &srDesc, &SRV);
	assert(SUCCEEDED(result));
	textureResult.mySRV = SRV;
	SRV->Release();

	textureResult.myHeight = aSize.x;
	textureResult.myWidth = aSize.y;
	return textureResult;
}

int DepthBuffer::RegisterForViewport(const Vector2i aSize)
{
	//Find available space for the viewport
	D3D11_VIEWPORT viewport;
	bool foundSpace = FindAvailableSpace(aSize, viewport);
	if (!foundSpace) 
	{
		// Couldn't find space for the viewport
		return -1;
	}

	// Add the viewport to the list
	myViewports.push_back(viewport);
	return (int)myViewports.size() - 1; // Return the index of the newly added viewport
}

bool DepthBuffer::FindAvailableSpace(const Vector2i aSize, D3D11_VIEWPORT& viewport)
{
	for (float y = 0; y <= (float)myHeight - aSize.y; ++y)
	{
		for (float x = 0; x <= (float)myWidth - aSize.x; ++x)
		{
			// Check if the space is available
			if (!IsSpaceOccupied(x, y, aSize)) {
				// Found available space, set the viewport and return true
				viewport.TopLeftX = x;
				viewport.TopLeftY = y;
				viewport.Width = (float)aSize.x;
				viewport.Height = (float)aSize.y;
				viewport.MinDepth = 0.0f;
				viewport.MaxDepth = 1.0f;
				return true;
			}
		}
	}
	// Couldn't find available space
	return false;
}

bool DepthBuffer::IsSpaceOccupied(float x, float y, const Vector2i aSize) const
{
	for (const auto& existingViewport : myViewports) 
	{
		float existingRight = existingViewport.TopLeftX + existingViewport.Width;
		float existingBottom = existingViewport.TopLeftY + existingViewport.Height;
		if (!(existingRight <= x || existingViewport.TopLeftX >= x + aSize.x ||
			existingBottom <= y || existingViewport.TopLeftY >= y + aSize.y)) 
		{
			// There is overlap, space is occupied
			return true;
		}
	}
	// No overlap, space is available
	return false;
}